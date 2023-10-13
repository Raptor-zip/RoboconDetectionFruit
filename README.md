# ロボコン フルーツ回収位置調整システム

[高専ロボコン](https://official-robocon.com/kosen/) 2023 では、つり下がっているフルーツを回収するのが競技課題である。ロボットに載せてあるカメラから、[YOLO V8](https://ultralytics.com/yolov8) でリアルタイムでフルーツを検出し、回収装置(お助けアイテム)の位置を調整する。フルーツの座標を [ROS2 humble](https://official-robocon.com/kosen/) で publish する。

# 使い方

## publish する topic

publish するトピックは Int16MultiArray 形式

```python:subscriber_member_function.py
array = [state, x, y, kind, conf]
```

| 番号 | 名前  | 説明                                                                                                     |
| :--: | :---: | -------------------------------------------------------------------------------------------------------- |
|  0   | state | 0 ノード起動成功フルーツ未検出(カメラ起動待ち) 1 フルーツ検出 2 カメラ未接続                             |
|  1   |   x   | -50 から 50 までの整数。0 なら中央、正の値なら右方向に移動、負の値なら、左方向に移動                     |
|  2   |   y   | 0 から 100 までの整数。画面下なら 0、画面の上なら 100 使う予定はない                                     |
|  3   | kind  | 0 キウイ,1 みかん,2 もも,3 ぶどう,4 ブルーベリー,5 ミックスフルーツ 使う予定はないが、昇降用に使えるかも |
|  4   | conf  | 精度 使う予定はない                                                                                      |

| クラス番号 | 配列番号 | 名前(日本語)     | 名前(英語) |
| :--------: | :------: | ---------------- | ---------- |
|     1      |    0     | キウイ           | kiwi       |
|     2      |    1     | みかん           | orange     |
|     3      |    2     | もも             | peach      |
|     4      |    3     | ブルーベリー     | blueberry  |
|     5      |    4     | ぶどう           | grape      |
|     6      |    5     | ミックスフルーツ | mix        |

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
ros2 run fruit_detection listener # subscriberノード起動
ros2 run fruit_detection pubdetect # publisherノード起動
```

# エラー対応

```
FPS(Setting):        0.00
(中略)
AttributeError: 'NoneType' object has no attribute 'shape'
```

カメラが接続されていない。
カメラを接続する USB ポートを変更する。
仮想環境なら、カメラにアクセスできるように設定する。

```
FPS(Setting):        30.00
(中略)
FileNotFoundError: '/home/ros-industrial/fruit/hoge.pt' does not exist
```

モデルがみつかっていない。
モデルのパスが間違っている。
絶対リンクで参照する。
OpenVINO の場合は、`.bin`と`.xml`が入っているフォルダーを指定する。
torch の場合は、`.pt`を直接、指定する。

# 仕組み

## 概要

映像と画像を撮影し、映像を 1 秒に 2 枚の間隔で画像化し、5311 枚の画像を用意した。
[Roboflow](https://roboflow.com)でラベル付をした。画像を回転させたものやぼかしをいれたもの、明るさを変えたものを作り、画像 11151 枚のデータセットを作成し、YOLO V8 で学習した。

`.pt`を`.onnx`に変換し、それを`.bin`と`.xml`に変換した。
大会でロボットに積むパソコンは GPU がないため、Torch や Intel CPU のみでできる OpenVINO を採用した。

## プログラムの概要

[OpenVINO](https://openvino.jp/)を使って物体検出した。
[OpenCV](https://opencv.org)でパソコンに接続した USB カメラからリアルタイムで画像を取得する。
`rclpy`で publish する

`subscriber_member_function.py`で subscribe し、結果を表示する。

## 軽量化

FP16 で量子化する予定。
