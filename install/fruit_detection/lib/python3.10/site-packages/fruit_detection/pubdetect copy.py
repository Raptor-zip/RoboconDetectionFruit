from ultralytics import YOLO
import cv2
import datetime
from concurrent.futures import ThreadPoolExecutor
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int16MultiArray

state = 0
x = 0
y = 0
kind = 0

save = False
# start webcam
cap = cv2.VideoCapture(0)
# cap.set(3, 1920)  # 横
# cap.set(4, 1080)  # 縦
cap.set(3, 640)
cap.set(4, 480)
cap.set(5, 60)  # fps
conf_threshold = 80  # confidence threshold(信頼度の閾値)
show = False
# show = False
# cap.set(11, 480) # コントラスト
# cap.set(15, 100)

# CV_CAP_PROP_CONTRAST 画像のコントラスト（カメラの場合のみ）．11
# CV_CAP_PROP_SATURATION 画像の彩度（カメラの場合のみ）．12
# CV_CAP_PROP_HUE 画像の色相（カメラの場合のみ）．13
# CV_CAP_PROP_GAIN 画像のゲイン（カメラの場合のみ）．14
# CV_CAP_PROP_EXPOSURE 露出（カメラの場合のみ）．15


class MinimalPublisher(Node):
    def __init__(self):
        super().__init__('minimal_publisher')
        self.publisher_ = self.create_publisher(Int16MultiArray, 'topic', 10)
        timer_period = 0.0333  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        global state
        global x
        global y
        global kind
        print("つぎああああああああああああああ")
        print(state)
        print(x)
        print(y)
        print(kind)
        msg = Int16MultiArray()
        msg.data = [state, x, y, kind]
        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%s"' % msg.data)
        self.i += 1


def main():
    with ThreadPoolExecutor(max_workers=2) as executor:
        executor.submit(pub)
        executor.submit(detect)


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
    if save == True:
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
        print("カメラの初期化に成功")

    fps_setting = cap.get(cv2.CAP_PROP_FPS)
    print("FPS(Setting):", '{:11.02f}'.format(fps_setting))
    timer = cv2.TickMeter()
    timer.start()
    # 各変数の初期値設定
    count = 0
    max_count = 30
    fps = 0

    # model = YOLO(
    #     "C:/Users/Rapto/マイドライブ/学校/ロボコン/フルーツ検出/学習済み/v8v90917/weights/v8-v9-0917.pt")
    # model = YOLO("resource/v8-v9-0917.pt")
    model = YOLO("/home/ros-industrial/fruit/v8-v9-0917.pt")
    # model = YOLO("yolo-Weights/yolov8n.pt")

    # object classes
    classNames = ["kiwi", "orange", "peach", "blueberry", "grape", "mix"]

    while True:
        if count == max_count:
            timer.stop()
            fps = max_count / timer.getTimeSec()
            # リセットと再スタート
            timer.reset()
            count = 0
            timer.start()

        success, img = cap.read()
        results = model(img, stream=True)

        cls = 0
        center = 0
        x1, y1, x2, y2 = 0, 0, 0, 0
        # coordinates
        for r in results:
            boxes = r.boxes

            # 読み込んだ画像の高さと幅を取得
            height = img.shape[0]
            width = img.shape[1]
            width_threshold = 0.05  # 真ん中10%

            if show == True:
                # object details
                font = cv2.FONT_HERSHEY_SIMPLEX
                fontScale = 1.3
                color = (255, 255, 255)
                thickness = 3

                # センターライン
                # cv2.line(img, pt1=(int(width / 2) , 0), pt2=(int(width / 2), height), color=(255 , 255 , 255), thickness=1)
                cv2.line(img, pt1=(int(width / 2 + width * width_threshold / 2), 0), pt2=(int(
                    width / 2 + width * width_threshold / 2), height), color=(0, 255, 255), thickness=2)
                # 中央左の線
                cv2.line(img, pt1=(int(width / 2 - width * width_threshold / 2), 0), pt2=(int(
                    width / 2 - width * width_threshold / 2), height), color=(0, 255, 255), thickness=2)

                # FPS表示
                cv2.putText(img, str(
                    f'{fps:.0f}')+"FPS", [30, 110], font, fontScale, (0, 0, 0), int(thickness + 10))
                cv2.putText(img, str(f'{fps:.0f}')+"FPS",
                            [30, 110], font, fontScale, color, thickness)
                cv2.putText(img, str(len(boxes))+" of fruits",
                            [30, 150], font, fontScale, color, thickness)

            else:
                print("       "+str(f'{fps:.0f}')+"FPS")

            for box in boxes:
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(
                    x2), int(y2)  # convert to int values
                conf = int(f'{box.conf[0].item()*100:.0f}')  # 信頼度の取得

                if conf > conf_threshold:
                    if show == True:
                        cv2.rectangle(img, (x1, y1), (x2, y2),
                                      (246, 205, 87), 3)
                        cls = int(box.cls[0])
                        org = [x1 - 10, y1 - 10]
                        cv2.putText(img, str(classNames[cls])+str(conf)+"%("+str(x1)+","+str(y1)+"),("+str(
                            x2)+","+str(y2)+")", org, font, fontScale, (0, 0, 0), thickness + 10)
                        cv2.putText(img, str(classNames[cls])+str(conf)+"%("+str(x1)+","+str(
                            y1)+"),("+str(x2)+","+str(y2)+")", org, font, fontScale, color, thickness)

                    # yの値が大きい場合のほう→xの値の大きいほうとしないと、近くにあって右にあるものと、遠くにあって真ん中にある状況だった場合、遠くのものに合わせられちゃう
                    if abs((x1 + x2) / 2 - width / 2) < abs(center / 2 - width / 2):
                        center = (x1 + x2) / 2
                        move_value = round(center / width - 0.5, 3)
                        print(move_value)

        global state
        global x
        global y
        global kind

        state = 1
        x = center
        y = y1 + y2
        kind = cls

        if show == True:
            resized = cv2.resize(img, None, None, 0.8, 0.8)
            cv2.imshow('Webcam', resized)

        if save == True:
            video.write(img)  # 動画を1フレームずつ保存する

        count += 1


if __name__ == '__main__':
    main()
