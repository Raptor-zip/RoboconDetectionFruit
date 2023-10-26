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
x = y = kind = 0

save = False
show = True

cap = cv2.VideoCapture(9)
cap.set(3, 640)  # 横
cap.set(4, 480)  # 縦
cap.set(5, 30)  # fps

def main():
    with ThreadPoolExecutor(max_workers=2) as executor:
        executor.submit(pub)
        executor.submit(detect)


class MinimalPublisher(Node):
    def __init__(self):
        super().__init__('fruit_detect_node')
        self.publisher_ = self.create_publisher(Int16MultiArray, 'image_recognition', 10)
        timer_period = 0.033  # seconds 30fps設定
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        global state,x,y,kind
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
    global state,x,y,kind
    state = 2

    if save == True:
        w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))  # カメラの横幅を取得
        h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))  # カメラの縦幅を取得
        fourcc = cv2.VideoWriter_fourcc(
            'm', 'p', '4', 'v')  # 動画保存時のfourcc設定（mp4用）
        # 動画の仕様（ファイル名、fourcc, FPS, サイズ）
        dt_now = datetime.datetime.now()
        video = cv2.VideoWriter(dt_now.strftime(
            '%H%M%S')+'.mp4', fourcc, 15, (w, h)) # 動画のFPS変えるの忘れないでね！

    if cap.isOpened():
        LOGGER.info("カメラの起動に成功")
        state = 0
    else:
        LOGGER.info("カメラの起動に失敗")
    fps_setting = cap.get(cv2.CAP_PROP_FPS)
    LOGGER.info("FPS(Setting): %d" % round(fps_setting))

    model = YOLO("src/fruit_detection/fruit_detection/v8-v26-1017.pt")
    # model = YOLO("src/fruit_detection/fruit_detection/v8-v26-1017_openvino_model")
    # model = YOLO("src/fruit_detection/fruit_detection/v8-v26-1017.onnx")

    timer = cv2.TickMeter()
    timer.start()
    # 変数の宣言
    count = 0
    max_count = 30
    fps = 30
    count_video = 0
    x_temp = x_temp_count = 0
    fit = False # 上中央にフルーツがいったときに動かないようにする

    margin_top = [0,20,25,30] # 上からの余白。認識しても無視する。何もないとき,ブルーベリー,ぶどう,ミックス
    while True:
        if count == max_count:
            timer.stop()
            fps = max_count / timer.getTimeSec()
            timer.reset()
            count = 0
            timer.start()
        img = cap.read()
        success, img = cap.read()
        results = model(img, stream=True, int8=False, half=False, show=False, imgsz=640, classes=(0,1,2), conf=0.6)

        height = img.shape[0]
        width = img.shape[1]
        cv2.rectangle(img, (0, int(margin_top[kind+1]/100*height)), (width, int(margin_top[kind+1]/100*height)), (255,255,255), thickness=2, lineType=cv2.LINE_8)

        for r in results:
            boxes = r.boxes
            fruit_array_all = []
            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(
                    x2), int(y2)  # convert to int values
                center_y = round((y1 + y2) / 2 / height * 100)
                # cv2.putText(img, str(kind), (200, 50), cv2.FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 2, cv2.LINE_AA)
                if center_y > margin_top[kind+1]:
                    # 検出したフルーツがmargin_topよりも下にあるなら
                    fruit_array_all.append([round((x1 + x2) / 2 / width * 100 - 50), center_y, int(box.cls[0])])
                    if save == True or show == True:
                        cv2.rectangle(img, (x1,y1),(x2,y2),(230,205,87),2)
                else:
                    if save == True or show == True:
                        cv2.rectangle(img, (x1,y1),(x2,y2),(255,255,255),2)
            if fruit_array_all == []:
                state = 0
            else:
                state = 1
                if len(fruit_array_all) > 1:
                    while 1 < len(fruit_array_all): # fruit_array_allが1個になるまで絞る
                        range_from_top_central_1 = fruit_array_all[0][0]/2**2 + fruit_array_all[0][1]**2 # 距離
                        range_from_top_central_2 = fruit_array_all[1][0]/2**2 + fruit_array_all[1][1]**2 # 距離
                        if range_from_top_central_1 > range_from_top_central_2:
                            fruit_array_all.pop(0)
                        else:
                            fruit_array_all.pop(1)
                fruit_array_all = fruit_array_all[0]

                # y = fruit_array_all[1] # yは途切れてもいいため、無補正
                # cv2.rectangle(img, (0, int(fruit_array_all[1]/100*height)), (width, int(fruit_array_all[1]/100*height)), (0,255,255), thickness=1, lineType=cv2.LINE_8)
            x_last = x_temp # x_tempは前回の情報
            cv2.putText(img, str(x_last), (0, 50), cv2.FONT_HERSHEY_PLAIN, 2, (255, 255, 255), 2, cv2.LINE_AA)
            if state == 1:
                if x_last == 0:
                    x = fruit_array_all[0]
                    y = fruit_array_all[1]
                    kind = fruit_array_all[2]
                    x_temp = x
                    cv2.rectangle(img, (int((fruit_array_all[0]+50)/100*width), 0), (int((fruit_array_all[0]+50)/100*width), height), (0,255,255), thickness=2, lineType=cv2.LINE_8)
                else:
                    cv2.rectangle(img, (int((x_last+50+20)/100*width), 0), (int((x_last+50+20)/100*width), height), (0,0,255), thickness=1, lineType=cv2.LINE_8)
                    cv2.rectangle(img, (int((x_last+50-20)/100*width), 0), (int((x_last+50-20)/100*width), height), (0,0,255), thickness=1, lineType=cv2.LINE_8)
                    if abs(fruit_array_all[0] - x_last) < 20:
                        # LOGGER.info("230 %d" % fruit_array_all[0])
                        x = fruit_array_all[0]
                        y = fruit_array_all[1]
                        kind = fruit_array_all[2] # 消すの忘れないでね
                        x_temp = x
                        x_temp_count = 0

                        cv2.rectangle(img, (int((fruit_array_all[0]+50)/100*width), 0), (int((fruit_array_all[0]+50)/100*width), height), (0,255,255), thickness=2, lineType=cv2.LINE_8)

                    else:
                        # LOGGER.info("距離20以上")
                        x = 0
                        y = -1
                        x_temp_count += 1
                        if x_temp_count > fps * 1 and x_temp_count > 10:
                            x_temp = x_temp_count = 0
                            cv2.putText(img, "00000000000000", (100, 50), cv2.FONT_HERSHEY_PLAIN, 4, (255, 255, 255), 5, cv2.LINE_AA)
                            LOGGER.info("                  x_tempリセット")
                        # LOGGER.info("232                %d" % x)


                # cv2.rectangle(img, (0, int((y-margin_top[kind+1])/100*height)), (width, int((y-margin_top[kind+1])/100*height)), (255,255,255), thickness=2, lineType=cv2.LINE_8)
                # お助けアイテムに近づいたときに移動を制限する
                if state == 1 and abs(x) < 5 and y-margin_top[kind+1] < 10:
                    fit = True
                    x = 0
                    cv2.putText(img, str(y-margin_top[kind+1]), (100, 50), cv2.FONT_HERSHEY_PLAIN, 4, (255, 255, 255), 5, cv2.LINE_AA)
            else:
                x = 0
                y = -1

            cv2.rectangle(img, (0, int(y/100*height)), (width, int(y/100*height)), (0,255,255), thickness=1, lineType=cv2.LINE_8)

            LOGGER.info("fruit:%s fps:%d" % (kind,fps))
            # LOGGER.info("states: %d x:%d y:%d fruit:%s fps:%d" % (state,x,y,kind,fps))


        if show == True:
            cv2.imshow("Image", img)
            cv2.waitKey(1)
        if save == True:
            video.write(img)  # 動画を1フレームずつ保存する
            # LOGGER.info(count_video)
            # cv2.imwrite(str(count_video)+'.jpg', img)
            if count_video > 800:
                LOGGER.info("ビデオ録画終了")
                video.release()

        count += 1

        count_video += 1


if __name__ == '__main__':
    main()
