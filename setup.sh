sudo modprobe v4l2loopback video_nr=9
scrcpy --v4l2-sink=/dev/video9 -m 1536 --max-fps=60 --window-x=100 --always-on-top --lock-video-orientation=2  #gamen haten
# ros2 launch robocon_ws/src/robocon2023_b_dash/launch/all_launch.py
# scrcpy --v4l2-sink=/dev/video9 -m 700 --max-fps=20 --lock-video-orientation=2 #gamen haten
# cd /home/roboconb/robocon_ws &
# colcon build &
# source install/setup.bash &