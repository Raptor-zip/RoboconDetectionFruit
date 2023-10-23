# ロボコン フルーツ自動回収システム

[高専ロボコン](https://official-robocon.com/kosen/) 2023 では、つり下がっているフルーツを回収するのが競技課題である。ロボットに載せてあるスマホのカメラから、[YOLO V8](https://ultralytics.com/yolov8) でリアルタイムでフルーツを検出し、フルーツの座標を [ROS2 humble](https://docs.ros.org/en/humble/index.html) でパブリッシュする。それを、サブスクライブして、横方向移動装置、上昇装置を自動で動かす。

# 使い方

## 初期設定(ライブラリのインストール)

pip がインストールされていない場合は、インストールする
以下のプログラムで YOLO を動かすための ultralytics と Android の画面を web カメラ化する scrcpy,ffmpeg をインストールする

```powershell
cd
pip install ultralytics # 時間かかる
pip install opencv-python # 時間かかる
pip install torch torchvision torchaudio # 時間かかる
sudo apt install ffmpeg libsdl2-2.0-0 adb wget \
                 gcc git pkg-config meson ninja-build libsdl2-dev \
                 libavcodec-dev libavdevice-dev libavformat-dev libavutil-dev \
                 libswresample-dev libusb-1.0-0 libusb-1.0-0-dev
git clone https://github.com/Genymobile/scrcpy
cd scrcpy
./install_release.sh
cd
sudo apt install v4l2loopback-dkms
```

<!-- pip3 install openvino -->

## 初期設定が終わったら

スマホの開発者オプションの USB デバッグを ON にする

パソコンとオドメトリボードを USB C to C で接続する

スマホとパソコンを USB C to C で接続する

パソコンで端末を開く

```powershell
sh setup.sh # スマホ画面をカメラ化
```

USB デバッグを許可する(表示されない場合もある)

パソコンでスマホの画面を見て、動作しているか確認する

スマホの自動回転を縦向きの状態でオフにする

自動クリックを開く

カメラアプリを開く

動画モードにする

設定をタップする

色合い・明るさで、色合いを暖色にする

ビデオ解像度を HD にする

タッチ追尾フォーカスをオフにする

電子式手振れ補正をスタンダードにする

自動クリックを再生する

ホルダーにスマホを装着する

スマホの半押しボタンを上に上げる

パソコンでスマホの画面を見て、動作しているか再び確認する

新しい端末を開き、以下のコードを実行

```powershell
colcon build --cmake-clean-cache
ros2 launch src/robocon2023_b_dash/launch/all_launch.py # プログラム起動
```

## 上下左右が反転しているとき

setup.sh の最後に`--lock-video-orientation=2`を追加する

## topic

トピックは Int16MultiArray 形式

```python:subscriber_member_function.py
array = [state, x, y, kind]
```

| 配列番号 | 名前  | 説明                                                                                 |
| :------: | :---: | ------------------------------------------------------------------------------------ |
|    0     | state | 0 ノード起動成功フルーツ未検出(カメラ起動待ち) 1 フルーツ検出 2 カメラ未接続         |
|    1     |   x   | -50 から 50 までの整数。0 なら中央、正の値なら右方向に移動、負の値なら、左方向に移動 |
|    2     |   y   | 0 から 100 までの整数。画面下なら 0、画面の上なら 100                                |
|    3     | kind  | 0 ぶどう,1 ブルーベリー,2 ミックスフルーツ                                           |

| クラス番号 | 配列番号 | 名前(日本語)     |
| :--------: | :------: | ---------------- |
|     1      |    0     | ブルーベリー     |
|     2      |    1     | ぶどう           |
|     3      |    2     | ミックスフルーツ |

## まみむ memo

```
cd ros2_ws
source install/setup.bash
ros2 topic echo /robocon2023_b_dash/UpperOut
ros2 topic echo /robocon2023_b_dash/XmoveOut
ros2 topic echo /robocon2023_b_dash/image_recognition
ros2 run rqt_console rqt_console
```
