# 動作確認済み


from ultralytics import YOLO
import cv2


def main():
    # start webcam
    cap = cv2.VideoCapture(0)
    # cap.set(3, 1920)
    # cap.set(4, 1080)
    cap.set(3, 640)
    cap.set(4, 480)
    fps_setting = cap.get(cv2.CAP_PROP_FPS)
    print("FPS(Setting):", '{:11.02f}'.format(fps_setting))
    timer = cv2.TickMeter()
    timer.start()
    # 各変数の初期値設定
    count = 0
    max_count = 30
    fps = 0

    show = True
    # show = False

    model = YOLO("/home/ros-industrial/fruit/v8-v9-0917.pt")
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
        # half=FalseにしないとAMDだとエラー出た imgszは可変
        results = model(img, stream=True, half=False, imgsz=256)

        # coordinates
        for r in results:
            boxes = r.boxes

            # 読み込んだ画像の高さと幅を取得
            height = img.shape[0]
            width = img.shape[1]
            width_threshold = 0.05  # 真ん中10%

            if show == True:
                # センターライン
                # cv2.line(img, pt1=(int(width / 2) , 0), pt2=(int(width / 2), height), color=(255 , 255 , 255), thickness=1)
                cv2.line(img, pt1=(int(width / 2 + width * width_threshold / 2), 0), pt2=(int(
                    width / 2 + width * width_threshold / 2), height), color=(0, 255, 255), thickness=2)
                # 中央左の線
                cv2.line(img, pt1=(int(width / 2 - width * width_threshold / 2), 0), pt2=(int(
                    width / 2 - width * width_threshold / 2), height), color=(0, 255, 255), thickness=2)

            # object details
            font = cv2.FONT_HERSHEY_SIMPLEX
            fontScale = 1.3
            color = (255, 255, 255)
            thickness = 3

            if show == False:
                print("       "+str(f'{fps:.0f}'))

            if show == True:
                # FPS表示
                cv2.putText(img, str(
                    f'{fps:.0f}')+"FPS", [30, 110], font, fontScale, (0, 0, 0), int(thickness + 10))
                cv2.putText(img, str(f'{fps:.0f}')+"FPS",
                            [30, 110], font, fontScale, color, thickness)

            cv2.putText(img, str(len(boxes))+"length",
                        [30, 140], font, fontScale, color, thickness)

            center = 0
            _y = 10000
            for box in boxes:
                # bounding box
                x1, y1, x2, y2 = box.xyxy[0]
                x1, y1, x2, y2 = int(x1), int(y1), int(
                    x2), int(y2)  # convert to int values
                conf = int(f'{box.conf[0].item()*100:.0f}')  # 信頼度の取得

                # print(x1)
                # print(y1)
                # print(x2)
                # print(y2)

                # put box in cam
                if conf > 70:
                    if show == True:
                        cv2.rectangle(img, (x1, y1), (x2, y2),
                                      (246, 205, 87), 3)
                        cv2.drawMarker(img, (int((x1+x2)/2), int((y1+y2)/2)), (0, 0, 0),
                                       markerType=cv2.MARKER_STAR, markerSize=30, thickness=10)
                        cv2.drawMarker(img, (int((x1+x2)/2), int((y1+y2)/2)), (0, 255, 0),
                                       markerType=cv2.MARKER_STAR, markerSize=30, thickness=4)
                        cls = int(box.cls[0])
                        org = [x1 - 10, y1 - 10]
                        cv2.putText(img, str(classNames[cls])+str(conf)+"%("+str(x1)+","+str(y1)+"),("+str(
                            x2)+","+str(y2)+")", org, font, fontScale, (0, 0, 0), thickness + 10)
                        cv2.putText(img, str(classNames[cls])+str(conf)+"%("+str(x1)+","+str(
                            y1)+"),("+str(x2)+","+str(y2)+")", org, font, fontScale, color, thickness)

                    # org = [x1 - 700 , y2 + 40]

                    # cv2.putText(img, classNames[cls], org, font, fontScale, color, thickness)
                    # cv2.putText(img, str(classNames[cls])+str(conf)+"%("+str(x1)+","+str(y1)+"),("+str(x2)+","+str(y2)+")", org, font, fontScale, (0,0,0), thickness + 10)
                    # cv2.putText(img, str(classNames[cls])+str(conf)+"%("+str(x1)+","+str(y1)+"),("+str(x2)+","+str(y2)+")", org, font, fontScale, color, thickness)
                    if (y1 + y2) / 2 < _y:
                        center = (x1 + x2) / 2
                        _y = (y1 + y2) / 2
                    # cv2.putText(img, "Center:"+str(center), [30,120], font, fontScale, (0,0,0), thickness + 10)
                    # cv2.putText(img, "Center:"+str(center), [30,120], font, fontScale, color, thickness)
            if center != 0:
                if center < (width / 2 - (width * (width_threshold / 2))):
                    if show == True:
                        cv2.putText(img, "Move to Left", [
                                    30, 50], font, fontScale*1.2, (0, 0, 0), thickness + 10)
                        cv2.putText(img, "Move to Left", [
                                    30, 50], font, fontScale*1.2, (0, 255, 255), thickness)
                    else:
                        print("Left")

                if (width / 2 - (width * (width_threshold / 2))) < center < (width * 0.5 + (width * (width_threshold / 2))):
                    if show == True:
                        cv2.putText(img, "Center", [
                                    30, 50], font, fontScale*1.2, (0, 0, 0), thickness + 10)
                        cv2.putText(img, "Center", [
                                    30, 50], font, fontScale*1.2, (0, 255, 0), thickness)
                    else:
                        print("Center")

                if center > (width / 2 + (width * (width_threshold / 2))):
                    if show == True:
                        cv2.putText(img, "Move to Right", [
                                    30, 50], font, fontScale*1.2, (0, 0, 0), thickness + 10)
                        cv2.putText(img, "Move to Right", [
                                    30, 50], font, fontScale*1.2, (255, 255, 0), thickness)
                    else:
                        print("Right")

        if show == True:
            resized = cv2.resize(img, None, None, 0.8, 0.8)
            cv2.imshow('Webcam', resized)
        count += 1

        if cv2.waitKey(1) == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
