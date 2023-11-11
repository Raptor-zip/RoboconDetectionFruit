from ultralytics import YOLO
import cv2
import datetime
from concurrent.futures import ThreadPoolExecutor
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int16MultiArray
from ultralytics.utils import LOGGER # logger wo tukautameni pakutta
import statistics # 最頻値を算出
# robocon_wsから実行される

state = 2
x = 0
y = 0
kind = 0

save = True
show = False #true ni dekinai

cap = cv2.VideoCapture(9)
cap.set(3, 640)  # 横
cap.set(4, 480)  # 縦
cap.set(5, 30)  # fps 50fpsに直す！！

def main():
    with ThreadPoolExecutor(max_workers=2) as executor:
        executor.submit(pub)
        executor.submit(detect)


class MinimalPublisher(Node):
    def __init__(self):
        super().__init__('fruit_detect_node')
        self.publisher_ = self.create_publisher(Int16MultiArray, 'image_recognition', 10)
        timer_period = 0.02  # seconds 50fps設定
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        global state
        global x
        global y
        global kind
        # LOGGER.info("kinddddddddddddddddddddddddddddddd %d" % kind)
        msg = Int16MultiArray()
        msg.data = [state, x, y, kind, 0]
        self.publisher_.publish(msg)
        # self.get_logger().info('Pub:"%s"' % msg.data)
        self.i += 1


def pub(args=None):
    rclpy.init(args=args)
    minimal_publisher = MinimalPublisher()
    rclpy.spin(minimal_publisher)
    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    # minimal_publisher.destroy_node()
    # rclpy.shutdown()


def detect():
    global state
    global x
    global y
    global kind
    state = 2

    # video
    if save == True:
        w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))  # カメラの横幅を取得
        h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))  # カメラの縦幅を取得
        fourcc = cv2.VideoWriter_fourcc(
            'm', 'p', '4', 'v')  # 動画保存時のfourcc設定（mp4用）
        # 動画の仕様（ファイル名、fourcc, FPS, サイズ）
        dt_now = datetime.datetime.now()
        video = cv2.VideoWriter(dt_now.strftime(
            '%H%M%S')+'.mp4', fourcc, 15, (w, h))

    if cap.isOpened():
        LOGGER.info("カメラの起動に成功")
        state = 0
    else:
        LOGGER.info("カメラの起動に失敗")
    fps_setting = cap.get(cv2.CAP_PROP_FPS)
    LOGGER.info("FPS(Setting): %d" % round(fps_setting))
    timer = cv2.TickMeter()
    timer.start()
    # 各変数の初期値設定
    count = 0
    max_count = 30
    fps = 30

    count_video = 0

    # 1フレームごとの一番上中央と近い物を1つ、配列に入れ、そのなかで最瀕値のフルーツに、高さを合わせる
    model = YOLO("src/fruit_detection/fruit_detection/v8-v26-1017.pt")
    # model = YOLO("src/fruit_detection/fruit_detection/v8-v26-1017_openvino_model")
    # model = YOLO("src/fruit_detection/fruit_detection/v8-v26-1017.onnx")


    past_frame_array= []
    past_frame_array_x = []

    x_temp = 0
    x_temp_count = 0
    x_temp_count_2 = 0

    margin_top = [0,30,35,40] # 上からの余白。認識しても無視する。何もないとき,ブルーベリー,ぶどう,ミックス

    while True:
        if count == max_count:
            timer.stop()
            fps = max_count / timer.getTimeSec()
            # リセットと再スタート
            timer.reset()
            count = 0
            timer.start()

        success, img = cap.read()
        results = model(img, stream=True, int8=False, half=False, show=True, imgsz=640, classes=(0,1,2), conf=0.6) # 416 de 30fps v23 pt 576(27) 544(30) 512(30) 480(30)

        height = img.shape[0]
        width = img.shape[1]

        for r in results:
            boxes = r.boxes
            fruit_array_all = []
            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(
                    x2), int(y2)  # convert to int values
                center_x = round((x1 + x2) / 2 / width * 100 - 50)
                center_y = round((y1 + y2) / 2 / height * 100)
                fruit_array_all.append([center_x, center_y, int(box.cls[0])])
                if save == True:
                    cv2.rectangle(img, (x1,y1),(x2,y2),(230,205,87),3)
            if fruit_array_all == []:
                # 何も認識しなかったら
                past_frame_array.append(None)
                state = 0
            else:
                # これいか8行ぐらいをxmoveとプログラム合わせる
                for i in range(len(fruit_array_all)-1)[::-1]:
                    range_from_top_central_1 = fruit_array_all[i][0]/2**2 + fruit_array_all[i][1]**2 # 距離
                    range_from_top_central_2 = fruit_array_all[i+1][0]/2**2 + fruit_array_all[i+1][1]**2 # 距離
                    if range_from_top_central_1 > range_from_top_central_2:
                        fruit_array_all.pop(i+1)
                past_frame_array.append(fruit_array_all[0][2])
            if len(past_frame_array) == fps * 2: # 2秒おきにkindを更新する
                past_frame_array.append(past_frame_array[9])# 重み付けする(直近のフレームを2倍にする)
                past_frame_array = [x for x in past_frame_array if x is not None] # 配列からNoneを取り除く
                if len(past_frame_array) > 0:
                    state = 1
                    # kind = statistics.mode(past_frame_array) # kind_arrayの最瀕値を求める
                    past_frame_array.clear()
                    # LOGGER.info("kind更新---------------------")


            # xmoveのプログラム
            # 10フレームの配列がプラスかマイナスで頻出したものの、同符号のなかで中央値を出す。それと、最新を比べて、異符号かつ変化量が大きすぎたら、無視する
            fruit_array_all = []
            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(
                    x2), int(y2)  # convert to int values
                center_x = round((x1 + x2) / 2 / width * 100 - 50)
                center_y = round((y1 + y2) / 2 / height * 100)
                fruit_array_all.append([center_x, center_y, int(box.cls[0])])

            # fruit_array_all = [[1,2,3],[-50,99,1],[25,25,2]]
            if fruit_array_all == []:
                # そもそも、おかしくね？これだと、認識しなったらkind更新されない
                # 何も認識しなかったら
                past_frame_array_x.append([0,0,0]) # none代入したほうが良くない？
                y = 99 # yは途切れてもいいため、補正する前に出す 0にしたいけど、そうすると20UPがずっと続いちゃうかも
                state = 0
            else:
                if len(fruit_array_all) > 1:
                    i = 0
                    # LOGGER.info(fruit_array_all)
                    while 1 < len(fruit_array_all): # fruit_array_allが1個になるまで絞る
                        range_from_top_central_1 = fruit_array_all[i][0]/2**2 + fruit_array_all[i][1]**2 # 距離
                        range_from_top_central_2 = fruit_array_all[i+1][0]/2**2 + fruit_array_all[i+1][1]**2 # 距離
                        if range_from_top_central_1 > range_from_top_central_2:
                            fruit_array_all.pop(i)
                        else:
                            fruit_array_all.pop(i+1)
                    # LOGGER.info(fruit_array_all)
                fruit_array_all = fruit_array_all[0]

                y = fruit_array_all[1] # yは途切れてもいいため、補正する前に出す
                past_frame_array_x.append(fruit_array_all)
            if len(past_frame_array_x) > fps * 1: # 1秒以上前の情報を評価対象から外す
                past_frame_array_x.pop(0)
            past_frame_plus_array = []
            past_frame_minus_array = []
            if past_frame_array_x != []:
                for i in range(len(past_frame_array_x))[::-1]:
                    if past_frame_array_x[i][0] > 0:
                        # +なら
                        past_frame_plus_array.append(past_frame_array_x[i][0])
                    else:
                        # -なら
                        past_frame_minus_array.append(past_frame_array_x[i][0])
            x_past_median = 0

            if len(past_frame_plus_array) > len(past_frame_minus_array):
                x_past_median = statistics.median(past_frame_plus_array)
            else:
                if len(past_frame_minus_array) != 0:# どっちも0ではないなら
                    x_past_median = statistics.median(past_frame_minus_array)
            x_last = x_temp # x_tempは前回の情報
            cv2.putText(img, str(x_last), (0, 50), cv2.FONT_HERSHEY_PLAIN, 4, (255, 255, 255), 5, cv2.LINE_AA)
            # fruit_array_allが一個になってない
            if fruit_array_all != []: # fruit_array_allがあったらだめながきがする x_past_median != 0 and
                if fruit_array_all[0] != 0:
                    if x_last == 0:
                        x = fruit_array_all[0]
                        x_temp = x

                        cv2.rectangle(img, (int((fruit_array_all[0]+50)/100*width), 0), (int((fruit_array_all[0]+50)/100*width), height), (255,255,0), thickness=3, lineType=cv2.LINE_8)
                    else:
                        state = 1
                        if x_last / fruit_array_all[0] <= 0: # x_past_median / fruit_array_all[0][0] < 0 or
                            # LOGGER.info("222")
                            # if abs(fruit_array_all[0][0] - x_past_median) < 20: # 変化量を測定
                            #     LOGGER.info("225")
                            #     x = x_past_median
                            # else:
                            #     LOGGER.info("無視229")

                            if abs(fruit_array_all[0] - x_last) < 20:
                                # LOGGER.info("230 %d" % fruit_array_all[0])
                                x = fruit_array_all[0]
                                kind = fruit_array_all[2]
                                x_temp = x

                                x_temp_count = 0
                                x_temp_count_2 = 0

                                cv2.rectangle(img, (int((fruit_array_all[0]+50)/100*width), 0), (int((fruit_array_all[0]+50)/100*width), height), (255,255,0), thickness=3, lineType=cv2.LINE_8)
                                cv2.rectangle(img, (int((fruit_array_all[0]+50+20)/100*width), 0), (int((fruit_array_all[0]+50+20)/100*width), height), (0,0,255), thickness=2, lineType=cv2.LINE_8)
                                cv2.rectangle(img, (int((fruit_array_all[0]+50-20)/100*width), 0), (int((fruit_array_all[0]+50-20)/100*width), height), (0,0,255), thickness=2, lineType=cv2.LINE_8)

                            else:
                                LOGGER.info("異符号で距離20以上")
                                x = 0
                                x_temp_count += 1
                                if x_temp_count > fps * 1:
                                    x_temp_count = 0
                                    x_temp = 0
                                    cv2.putText(img, "00000000000000", (100, 50), cv2.FONT_HERSHEY_PLAIN, 4, (255, 255, 255), 5, cv2.LINE_AA)
                                    LOGGER.info("xtemp00000000000000000000000000")
                                # LOGGER.info("232                %d" % x)

                                cv2.rectangle(img, (int((x_last+50)/100*width), 0), (int((x_last+50)/100*width), height), (0,255,255), thickness=3, lineType=cv2.LINE_8)
                                cv2.rectangle(img, (int((x_last+50+20)/100*width), 0), (int((x_last+50+20)/100*width), height), (0,0,255), thickness=2, lineType=cv2.LINE_8)
                                cv2.rectangle(img, (int((x_last+50-20)/100*width), 0), (int((x_last+50-20)/100*width), height), (0,0,255), thickness=2, lineType=cv2.LINE_8)
                        else:
                            if abs(fruit_array_all[0] - x_last) < 20:
                                x = fruit_array_all[0]
                                kind = fruit_array_all[2]
                                x_temp = x

                                x_temp_count = 0
                                x_temp_count_2 = 0

                                cv2.rectangle(img, (int((fruit_array_all[0]+50)/100*width), 0), (int((fruit_array_all[0]+50)/100*width), height), (255,255,0), thickness=3, lineType=cv2.LINE_8)
                                cv2.rectangle(img, (int((fruit_array_all[0]+50+20)/100*width), 0), (int((fruit_array_all[0]+50+20)/100*width), height), (0,0,255), thickness=2, lineType=cv2.LINE_8)
                                cv2.rectangle(img, (int((fruit_array_all[0]+50-20)/100*width), 0), (int((fruit_array_all[0]+50-20)/100*width), height), (0,0,255), thickness=2, lineType=cv2.LINE_8)
                            else:
                                LOGGER.info("同符号で距離20以上")
                                x = 0
                                x_temp_count_2 += 1
                                if x_temp_count_2 > fps:
                                    x_temp_count_2 = 0
                                    x_temp = 0
                                    cv2.putText(img, "2222222222", (100, 50), cv2.FONT_HERSHEY_PLAIN, 4, (255, 255, 255), 5, cv2.LINE_AA)
                                    LOGGER.info("xtemp22222222222222222222222222")

                                cv2.rectangle(img, (int((x_last+50)/100*width), 0), (int((x_last+50)/100*width), height), (0,255,255), thickness=3, lineType=cv2.LINE_8)
                                cv2.rectangle(img, (int((x_last+50+20)/100*width), 0), (int((x_last+50+20)/100*width), height), (0,0,255), thickness=2, lineType=cv2.LINE_8)
                                cv2.rectangle(img, (int((x_last+50-20)/100*width), 0), (int((x_last+50-20)/100*width), height), (0,0,255), thickness=2, lineType=cv2.LINE_8)
                else:
                    x = 0
            else:
                x = 0

            # LOGGER.info("x:%d y:%d fruit:%s fps:%d" % (x,y,kind,fps))
            # LOGGER.info("x:%d" % x)
            # if x == 0:
            #     LOGGER.info("-----------------NO fps:%d" % fps)
            # else:
            #     LOGGER.info("-----------------x:%d y:%d fruit:%s fps:%d" % (x,y,kind,fps))
        LOGGER.info("%d fps" % fps)


        if save == True:
            video.write(img)  # 動画を1フレームずつ保存する
            # LOGGER.info(count_video)
            # cv2.imwrite(str(count_video)+'.jpg', img)
            if count_video > 1000:
                LOGGER.info("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
                video.release()

        count += 1

        count_video += 1


if __name__ == '__main__':
    main()
