# ロボコン フルーツ自動回収システム

[高専ロボコン](https://official-robocon.com/kosen/) 2023 では、つり下がっているフルーツを回収するのが競技課題である。ロボットに載せてあるスマホのカメラから、[YOLO V8](https://ultralytics.com/yolov8) でリアルタイムでフルーツを検出し、フルーツの座標を [ROS2 humble](https://docs.ros.org/en/humble/index.html) でパブリッシュする。それを、サブスクライブして、横方向移動装置、上昇装置を自動で動かす。

# 使い方

```
sh setup.sh # スマホ画面をカメラ化
ros2 launch src/robocon2023_b_dash/launch/all_launch.py # プログラム起動
colcon build --cmake-clean-cache
```

## topic

トピックは Int16MultiArray 形式

```python:subscriber_member_function.py
array = [state, x, y, kind]
```

| 配列番号 | 名前  | 説明                                                                                 |
| :------: | :---: | ------------------------------------------------------------------------------------ |
|    0     | state | 0 ノード起動成功フルーツ未検出(カメラ起動待ち) 1 フルーツ検出 2 カメラ未接続         |
|    1     |   x   | -50 から 50 までの整数。0 なら中央、正の値なら右方向に移動、負の値なら、左方向に移動 |
|    2     |   y   | 0 から 100 までの整数。画面下なら 0、画面の上なら 100 使う予定はない                 |
|    3     | kind  | 0 ぶどう,1 ブルーベリー,2 ミックスフルーツ 上昇装置の自動化用                        |

| クラス番号 | 配列番号 | 名前(日本語)     |
| :--------: | :------: | ---------------- |
|     1      |    0     | ブルーベリー     |
|     2      |    1     | ぶどう           |
|     3      |    2     | ミックスフルーツ |

## コマンド

初期設定(ライブラリのインストール)
pip のインストールが必要

```
pip3 install ultralytics
pip3 install opencv-python
pip3 install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cpu
pip3 install openvino
```

```
cd ros2_ws
source install/setup.bash
ros2 topic echo /robocon2023_b_dash/UpperOut
ros2 topic echo /robocon2023_b_dash/XmoveOut
ros2 topic echo /robocon2023_b_dash/image_recognition
ros2 run rqt_console rqt_console
```
