# 0
a 0 O

# 1
box ni haittetemo onazi  
b o  
b x

# 2
box ni haitteru  
   1    2  
c  o    x  box hantei y hantei  
c  x    o  box hantei y hantei  
d  o    o  y hantei  
e  x    x  y hantei


ros2 launch src/robocon2023_b_dash/launch/all_launch.py

--always-on-top
   Make scrcpy window always on top (above other windows).

-b, --bit-rate=value
   Encode the video at the gitven bit-rate, expressed in bits/s. Unit suffixes are supported:
   'K' (x1000) and 'M' (x1000000).
   Default is 8000000.

--codec-options=key[:type]=value[,...]
   Set a list of comma-separated key:type=value options for the device encoder.
   The possible values for 'type' are 'int' (default), 'long', 'float' and 'string'.
   The list of possible codec options is available in the Android documentation:
   <https://d.android.com/reference/android/media/MediaFormat>

--crop=width:height:x:y
   Crop the device screen on the server.
   The values are expressed in the device natural orientation (typically, portrait for a phone,
   landscape for a tablet). Any --max-size value is cmoputed on the cropped size.

--disable-screensaver
   Disable screensaver while scrcpy is running.

--display=id
   Specify the display id to mirror.
   The list of possible display ids can be listed by:
      adb shell dumpsys display
   (search "mDisplayId=" in the output)
   Default is 0.

--display-buffer=ms
   Add a buffering delay (in milliseconds) before displaying. This increases latency to
   compensate for jitter.
   Default is 0 (no buffering).

--encoder=name
   Use a specific MediaCodec encoder (must be a H.264 encoder).

--force-adb-forward
   Do not attempt to use "adb reverse" to connect to the device.

--forward-all-clicks
   By default, right-click triggers BACK (or POWER on) and middle-click triggers HOME. This
   option disables these shortcuts and forwards the clicks to the device instead.

-f, --fullscreen
   Start in fullscreen.

-K, --hid-keyboard
   Simulate a physical keyboard by using HID over AOAv2.
   It provides a better experience for IME users, and allows to generate non-ASCII characters,
   contrary to the default injection method.
   It may only work over USB, and is currently only supported on Linux.
   The keyboard layout must be configured (once and for all) on the device, via Settings ->
   System -> Languages and input -> Physical keyboard. This settings page can be started
   directly: `adb shell am start -a android.settings.HARD_KEYBOARD_SETTINGS`.
   However, the option is only available when the HID keyboard is enabled (or a physical
   keyboard is connected).

-h, --help
   Print this help.

--legacy-paste
   Inject computer clipboard text as a sequence of key events on Ctrl+v (like MOD+Shift+v).
   This is a workaround for some devices not behaving as expected when setting the device
   clipboard programmatically.

--lock-video-orientation[=value]
   Lock video orientation to value.
   Possible values are "unlocked", "initial" (locked to the initial orientation), 0, 1, 2 and
   3. Natural device orientation is 0, and each increment adds a 90 degrees rotation
   counterclockwise.
   Default is "unlocked".
   Passing the option without argument is equivalent to passing "initial".

--max-fps=value
   Limit the frame rate of screen capture (officially supported since Android 10, but may work
   on earlier versions).

-m, --max-size=value
   Limit both the width and height of the video to value. The other dimension is computed so
   that the device aspect-ratio is preserved.
   Default is 0 (unlimited).

--no-clipboard-autosync
   By default, scrcpy automatically synchronizes the computer clipboard to the device clipboard
   before injecting Ctrl+v, and the device clipboard to the computer clipboard whenever it
   changes.
   This option disables this automatic synchronization.

-n, --no-control
   Disable device control (mirror the device in read-only).

-N, --no-display
   Do not display device (only when screen recording or V4L2 sink is enabled).

--no-key-repeat
   Do not forward repeated key events when a key is held down.

--no-mipmaps
   If the renderer is OpenGL 3.0+ or OpenGL ES 2.0+, then mipmaps are automatically generated
   to improve downscaling quality. This option disables the generation of mipmaps.

-p, --port=port[:port]
   Set the TCP port (range) used by the client to listen.
   Default is 27183:27199.

--power-off-on-close
   Turn the device screen off when closing scrcpy.

--prefer-text
   Inject alpha characters and space as text events instead ofkey events.
   This avoids issues when combining multiple keys to enter a special character, but breaks the
   expected behavior of alpha keys in games (typically WASD).

--push-target=path
   Set the target directory for pushing files to the device by drag & drop. It is passed as is
   to "adb push".
   Default is "/sdcard/Download/".

--raw-key-events
   Inject key events for all input keys, and ignore text events.

-r, --record=file.mp4
   Record screen to file.
   The format is determined by the --record-format option if set, or by the file extension
   (.mp4 or .mkv).

--record-format=format
   Force recording format (either mp4 or mkv).

--render-driver=name
   Request SDL to use the given render driver (this is just a hint).
   Supported names are currently "direct3d", "opengl", "opengles2", "opengles", "metal" and
   "software".
   <https://wiki.libsdl.org/SDL_HINT_RENDER_DRIVER>

--rotation=value
   Set the initial display rotation.
   Possible values are 0, 1, 2 and 3. Each increment adds a 90 degrees rotation
   counterclockwise.

-s, --serial=serial
   The device serial number. Mandatory only if several devices are connected to adb.

--shortcut-mod=key[+...][,...]
   Specify the modifiers to use for scrcpy shortcuts.
   Possible keys are "lctrl", "rctrl", "lalt", "ralt", "lsuper" and "rsuper".
   A shortcut can consist in several keys, separated by '+'. Several shortcuts can be
   specified, separated by ','.
   For example, to use either LCtrl+LAlt or LSuper for scrcpy shortcuts, pass
   "lctrl+lalt,lsuper".
   Default is "lalt,lsuper" (left-Alt or left-Super).

-S, --turn-screen-off
   Turn the device screen off immediately.

-t, --show-touches
   Enable "show touches" on start, restore the initial value on exit.
   It only shows physical touches (not clicks from scrcpy).

--tunnel-host=ip
   Set the IP address of the adb tunnel to reach the scrcpy server. This option automatically
   enables --force-adb-forward.
   Default is localhost.

--tunnel-port=port
   Set the TCP port of the adb tunnel to reach the scrcpy server. This option automatically
   enables --force-adb-forward.
   Default is 0 (not forced): the local port used for establishing the tunnel will be used.

--v4l2-sink=/dev/videoN
   Output to v4l2loopback device.
   It requires to lock the video orientation (see --lock-video-orientation).

--v4l2-buffer=ms
   Add a buffering delay (in milliseconds) before pushing frames. This increases latency to
   compensate for jitter.
   This option is similar to --display-buffer, but specific to V4L2 sink.
   Default is 0 (no buffering).

-V, --verbosity=value
   Set the log level (verbose, debug, info, warn or error).
   Default is info.

-v, --version
   Print the version of scrcpy.

-w, --stay-awake
   Keep the device on while scrcpy is running, when the device is plugged in.

--tcpip[=ip[:port]]
   Configure and reconnect the device over TCP/IP.
   If a destination address is provided, then scrcpy connects to this address before starting.
   The device must listen on the given TCP port (default is 5555).
   If no destination address is provided, then scrcpy attempts to find the IP address of the
   current device (typically connected over USB), enables TCP/IP mode, then connects to this
   address before starting.

--window-borderless
   Disable window decorations (display borderless window).

--window-title=text
   Set a custom window title.

--window-x=value
   Set the initial window horizontal position.
   Default is "auto".

--window-y=value
   Set the initial window vertical position.
   Default is "auto".

--window-width=value
   Set the initial window width.
   Default is 0 (automatic).

--window-height=value
   Set the initial window height.
   Default is 0 (automatic).

