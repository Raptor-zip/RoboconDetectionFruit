# dousakakunizumi 10/5

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
conf = 0

# Areas of absolute recognition
margin = [0.1, 0.2, 0.2 , 0.2]

save = False
# start webcam
cap = cv2.VideoCapture(9)
# cap.set(3, 1920)  # 横
# cap.set(4, 1080)  # 縦
cap.set(3, 480)
cap.set(4, 360)
cap.set(5, 60)  # fps
conf_threshold = 80  # confidence threshold(信頼度の閾値)
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
        super().__init__('minimal_publisher')
        self.publisher_ = self.create_publisher(Int16MultiArray, 'image_recognition', 10)
        timer_period = 0.03  # seconds 33fps設定
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        global state
        global x
        global y
        global kind
        global conf
        msg = Int16MultiArray()
        msg.data = [state, x, y, kind, conf]
        self.publisher_.publish(msg)
        # self.get_logger().info('Publishing: "%s"' % msg.data)
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
    global conf
    state = 2
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
    else:
        print("カメラkidousippai")
        state = 2

    fps_setting = cap.get(cv2.CAP_PROP_FPS)
    print("FPS(Setting):", '{:11.02f}'.format(fps_setting))
    timer = cv2.TickMeter()
    timer.start()
    # 各変数の初期値設定
    count = 0
    max_count = 30
    fps = 0
    # model = YOLO("/home/roboconb/python/fruits/v8-v9-0917.pt")
    # model = YOLO("/home/roboconb/python/fruits/v8-v22-1005.pt")
    # model = YOLO("/home/roboconb/python/fruits/v8-v23-1005.pt")
    model = YOLO("/home/roboconb/detection-model/v8-v24-1007.pt")
    # model = YOLO("/home/roboconb/python/fruits/v8-v9-0917_"+str(imgsize)+"_openvino_model")
    # model = YOLO("/home/roboconb/python/fruits/v8-v22-1005-openvino-model")
    # model = YOLO("/home/roboconb/python/fruits/640int8_openvino_model")
    # model = YOLO("/home/roboconb/python/fruits/output512_openvino_model")
    # model = YOLO("/home/roboconb/python/fruits/output.onnx")
    # model = YOLO("/home/roboconb/python/fruits/640int8.onnx")
    # model = YOLO("/home/roboconb/python/fruits/v8-v9-0917-640.onnx")
    # model = YOLO("/home/roboconb/python/fruits/v8-v22-1005.onnx")
    # model = YOLO("yolo-Weights/yolov8n.pt")

    # classNames = ["kiwi", "orange", "peach", "blueberry", "grape", "mix"]
    classNames = ["blueberry", "grape", "mix"]


    while True:
        if count == max_count:
            timer.stop()
            fps = max_count / timer.getTimeSec()
            # リセットと再スタート
            timer.reset()
            count = 0
            timer.start()

        success, img = cap.read()
        results = model(img, stream=True, int8=False, half=False, show=False, imgsz=640, classes=(1,2), conf=0.75) # 416 de 30fps v23 pt 576(27) 544(30) 512(30) 480(30)

        # value reset
        cls = 0
        center_x = 0
        move_value = 0
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
                print(str(f'{fps:.0f}')+"FPS")

            x = 0
            y = 0
            kind = 0
            conf = 0
            print(boxes.xyxy)
            if len(boxes) == 1:
                x1, y1, x2, y2 = boxes.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(
                    x2), int(y2)  # convert to int values
                state = 1
                center_x = (x1 + x2) / 2 / width
                center_y = (y1 + y2) / 2 / height
                kind = int(boxes.cls[0])
                state = 1
                # ryouiki ni haitteiruka kakunin
                print("ccccccccccccccccccccccccccccccccccccccccccccc")
                print(margin[1])
                print(center_x)
                if margin[1] < center_x:
                    print(1111111111111111111111111111111111)
                if margin[3] < center_x:
                    print(3333333333333333333333333333333333)
                if margin[0] < center_y:
                    print(0000000000000000000000000000000000)
                if margin[2] < center_y:
                    print(2222222222222222222222222222222222)
                
                if abs((x1 + x2) / 2 - width / 2) < abs(center_x / 2 - width / 2):
                    center_x = (x1 + x2) / 2
                    move_value = round(center_x / width * 100 - 50)
                x = move_value
                y = round((y1+y2)/2/height*100)  # 今後治す
                kind = int(boxes.cls[0])
                conf = int(f'{boxes.conf[0].item()*100:.0f}')
            else:
                fruit_array = []
                for box in boxes:
                    x1, y1, x2, y2 = box.xyxy[0]
                    x1, y1, x2, y2 = int(x1), int(y1), int(
                        x2), int(y2)  # convert to int values
                    print(x1)
                    print(x2)
                    print(y1)
                    print(y2)
                    center_x = (x1 + x2) / 2 / width
                    center_y = (y1 + y2) / 2 / height
                    kind = int(box.cls[0])
                    state = 1
                    # ryouiki ni haitteiruka kakunin
                    print("ccccccccccccccccccccccccccccccccccccccccccccc")
                    print(margin[1])
                    print(center_x)
                    if margin[1] < center_x:
                        print(1111111111111111111111111111111111)
                    if margin[3] > center_x:
                        print(3333333333333333333333333333333333)
                    if margin[0] > center_y:
                        print(0000000000000000000000000000000000)
                    if margin[2] < center_y:
                        print(2222222222222222222222222222222222)
                    if(margin[1] > center_x and margin[3] < center_x and margin[0] > center_y and margin[2] < center_y):
                        print("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv")
                        fruit_array.append([center_x, center_y, kind])
                        # rennsouhairetu ni box naino mono wo tuika siteiku
                for i in range(len(fruit_array)):
                    if fruit_array[i][1] < fruit_array[i][1]:
                        fruit_array[i][1].pop
                print("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
                print(fruit_array)
                conf = 0

        if show == True:
            resized = cv2.resize(img, None, None, 0.8, 0.8)
            cv2.imshow('Webcam', resized)

        if save == True:
            video.write(img)  # 動画を1フレームずつ保存する

        count += 1


if __name__ == '__main__':
    main()
