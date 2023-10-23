# dousakakunin mati  10/9

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

# Areas of absolute recognition
margin = [0.1, 0.8, 0.8 , 0.2]
save = True
# start webcam
cap = cv2.VideoCapture(9)
# cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G')) # korega nai to  timeout site kamera ga ninsiki sare nai kamo 10/9
cap.set(3, 640)  # 横
cap.set(4, 480)  # 縦
cap.set(5, 60)  # fps
# show = True
show = False #true ni dekinai
# cap.set(11, 480) # コントラスト
# cap.set(15, 100)

# CV_CAP_PROP_CONTRAST 画像のコントラスト（カメラの場合のみ）．11
# CV_CAP_PROP_SATURATION 画像の彩度（カメラの場合のみ）．12
# CV_CAP_PROP_HUE 画像の色相（カメラの場合のみ）．13
# CV_CAP_PROP_GAIN 画像のゲイン（カメラの場合のみ）．14
# CV_CAP_PROP_EXPOSURE 露出（カメラの場合のみ）．15


def main():
    with ThreadPoolExecutor(max_workers=2) as executor:
        executor.submit(pub)
        executor.submit(detect)


class MinimalPublisher(Node):
    def __init__(self):
        super().__init__('fruit_detect_node')
        self.publisher_ = self.create_publisher(Int16MultiArray, 'image_recognition', 10)
        timer_period = 0.05  # seconds 20fps設定
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
    # if save == True:
    fps = int(cap.get(cv2.CAP_PROP_FPS))  # カメラのFPSを取得
    w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))  # カメラの横幅を取得
    h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))  # カメラの縦幅を取得
    fourcc = cv2.VideoWriter_fourcc(
        'm', 'p', '4', 'v')  # 動画保存時のfourcc設定（mp4用）
    # 動画の仕様（ファイル名、fourcc, FPS, サイズ）
    dt_now = datetime.datetime.now()
    video = cv2.VideoWriter(dt_now.strftime(
        '%H%M%S')+'.mp4', fourcc, fps, (w, h))

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
    fps = 0

    count_video = 0

    # 1フレームごとの一番上中央と近い物を1つ、配列に入れ、そのなかで最瀕値のフルーツに、高さを合わせる
    model = YOLO("src/fruit_detection/fruit_detection/v8-v26-1017.pt")
    # model = YOLO("src/fruit_detection/fruit_detection/v8-v26-1017_openvino_model")
    # model = YOLO("src/fruit_detection/fruit_detection/v8-v26-1017.onnx")
    # model = YOLO("fruit_detection/v8-v26-1017.pt")


    past_frame_array= []
    past_frame_array_x = []

    x_temp = 0
    x_temp_count = 0

    while True:
        if count == max_count:
            timer.stop()
            fps = max_count / timer.getTimeSec()
            # リセットと再スタート
            timer.reset()
            count = 0
            timer.start()

        success, img = cap.read()
        results = model(img, stream=True, int8=False, half=False, show=False, imgsz=640, classes=(0,1,2), conf=0.75) # 416 de 30fps v23 pt 576(27) 544(30) 512(30) 480(30)

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
                    cv2.rectangle(img, (x1,y1),(x2,y2),(246,205,87),3)
            if fruit_array_all == []:
                # 何も認識しなかったら
                past_frame_array.append(None)
                state = 0
            else:
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
                    kind = statistics.mode(past_frame_array) # kind_arrayの最瀕値を求める
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

            if len(fruit_array_all) == 0:
                # そもそも、おかしくね？これだと、認識しなったらkind更新されない
                # 何も認識しなかったら
                past_frame_array_x.append([0,0,0]) # none代入したほうが良くない？
                y = 99 # yは途切れてもいいため、補正する前に出す 0にしたいけど、そうすると20UPがずっと続いちゃうかも
                state = 0
            else:
                for i in range(len(fruit_array_all)-1)[::-1]:
                    # 1子しかないときどうなるの？
                    range_from_top_central_1 = fruit_array_all[i][0]/2**2 + fruit_array_all[i][1]**2 # 距離
                    range_from_top_central_2 = fruit_array_all[i+1][0]/2**2 + fruit_array_all[i+1][1]**2 # 距離
                    if range_from_top_central_1 > range_from_top_central_2:
                        fruit_array_all.pop(i+1)
                y = fruit_array_all[0][1] # yは途切れてもいいため、補正する前に出す
                past_frame_array_x.append(fruit_array_all[0])
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
            # fruit_array_allが一個になってない
            if x_past_median != 0 and fruit_array_all != []: # fruit_array_allがあったらだめながきがする
                if fruit_array_all[0][0] != 0:
                    state = 1
                    if x_last / fruit_array_all[0][0] <= 0: # x_past_median / fruit_array_all[0][0] < 0 or
                        # LOGGER.info("222")
                        # if abs(fruit_array_all[0][0] - x_past_median) < 20: # 変化量を測定
                        #     LOGGER.info("225")
                        #     x = x_past_median
                        # else:
                        #     LOGGER.info("無視229")
                        if abs(fruit_array_all[0][0] - x_last) < 20:
                            # LOGGER.info("230 %d" % fruit_array_all[0][0])
                            x = fruit_array_all[0][0]
                            x_temp = x
                            # x_temp_count += 1
                            # if x_temp_count > fps * 1.5:
                            #     x_temp_count = 0
                            #     x_temp = 0
                        else:
                            x = 0
                            # LOGGER.info("232                %d" % x)
                    else:
                        # fruit_array_allとx_past_medianとlast_arrayが同符号なら
                        # LOGGER.info("239 %d" % fruit_array_all[0][0])
                        x = fruit_array_all[0][0]
                        x_temp = x

            # LOGGER.info("x:%d y:%d fruit:%s fps:%d" % (x,y,kind,fps))
            # LOGGER.info("x:%d" % x)
            # if x == 0:
            #     LOGGER.info("-----------------NO fps:%d" % fps)
            # else:
            #     LOGGER.info("-----------------x:%d y:%d fruit:%s fps:%d" % (x,y,kind,fps))

        if save == True or show == True:
            # object details
            font = cv2.FONT_HERSHEY_SIMPLEX
            fontScale = 1.3
            color = (255, 255, 255)
            thickness = 3

            # print(246)
            # cv2.drawMarker(img, (int((x+50)/100*width), int(y/100*height)),(246,205,87), markerType=cv2.MARKER_STAR, markerSize=30, thickness=4)
            cv2.rectangle(img, (int((x+50)/100*width), 0), (int((x+50)/100*width), height), (246,205,87), thickness=4, lineType=cv2.LINE_8)
            # cv2.drawMarker(img, (int((x+50)/100*width), int(y/100*height)),(255,255,255), markerType=cv2.MARKER_STAR, markerSize=30, thickness=1)
            # print(249)
            # resized = cv2.resize(img, None, None, 0.8, 0.8)
            # cv2.imshow('Webcam', resized)

        if save == True:
            video.write(img)  # 動画を1フレームずつ保存する
            # LOGGER.info(count_video)
            LOGGER.info("%d fps" % fps)
            if count_video > fps * 20 and count_video > 1500:
                # LOGGER.info("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee")
                video.release()

        count += 1

        count_video += 1


if __name__ == '__main__':
    main()
