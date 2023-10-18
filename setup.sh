sudo modprobe v4l2loopback video_nr=9
scrcpy --v4l2-sink=/dev/video9 -m 640 --max-fps=20 --window-x=100 --always-on-top
# --lock-video-orientation=2