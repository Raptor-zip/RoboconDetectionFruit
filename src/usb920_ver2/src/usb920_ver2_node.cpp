#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstdint>
#include <signal.h>
#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <functional>
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joy.hpp"

#include "usb920_ver2/DualShock.h"
#define SERIAL_PORT "/dev/ttyUSB0"
int fd;      // ファイルディスクリプタ
int len;

// DualShock_TypeDef usb;
typedef union {
  DualShock_TypeDef joy[2];
  uint8_t bin[16];
}Robocon2023_USB920_recv_TypeDef;

Robocon2023_USB920_recv_TypeDef usb;

uint8_t data[16];
uint8_t cobs[sizeof(data) + 2];
using namespace std::chrono_literals;

// std::vector<float> axes[8];
// std::vector<float> button[11];

class JoyPublisher : public rclcpp::Node
{
  public:
    JoyPublisher()
    : Node("Joy_Publisher")
    {
      publisher_ = this->create_publisher<sensor_msgs::msg::Joy>("joy1", rclcpp::QoS(10));
      timer_ = this->create_wall_timer(
      20ms, std::bind(&JoyPublisher::topic_callback, this));
      timer_2 = this->create_wall_timer(
      10ms, std::bind(&JoyPublisher::timer_callback, this));
    }
  private:
    void topic_callback()
    {
      RCLCPP_INFO(this->get_logger(), "I got joy data!!");
      sensor_msgs::msg::Joy joy_msg;
      joy_msg.axes.resize(8);
      joy_msg.buttons.resize(11);
      joy_msg.header.frame_id = std::string("joy1");
      joy_msg.axes[0] = (-usb.joy[0].JoyStickData.Lx / 127.5) + 1;
      joy_msg.axes[1] = (usb.joy[0].JoyStickData.Ly / 127.5) - 1;
      joy_msg.axes[2] = (-usb.joy[0].JoyStickData.Analog_L2 / 127.5) + 1;
      joy_msg.axes[3] = (-usb.joy[0].JoyStickData.Rx / 127.5) + 1;
      joy_msg.axes[4] = (usb.joy[0].JoyStickData.Ry / 127.5) - 1;
      joy_msg.axes[5] = (-usb.joy[0].JoyStickData.Analog_R2 / 127.5) + 1;
      if(usb.joy[0].JoyStickData.button_status != 0) {
        for(int i = 0; i < 11; i++) {
          if(usb.joy[0].JoyStickData.button_status == i + 1) {
            joy_msg.buttons[i] = 1;
            break;
          }
        }
      }else {
        for(int i = 0; i < 11; i++) {
          joy_msg.buttons[i] = 0;
        }
      }
      if(usb.joy[0].JoyStickData.button_status >= 14) {
        if(usb.joy[0].JoyStickData.button_status % 2 == 0) {
          joy_msg.axes[6] = 15 - usb.joy[0].JoyStickData.button_status;
        }else {
          joy_msg.axes[7] = 16 - usb.joy[0].JoyStickData.button_status;
        }
      }
      publisher_->publish(joy_msg);
    }
    void timer_callback()
    {
      len = read(fd, cobs, sizeof(cobs));
      tcflush(fd, TCIFLUSH);
      for (int i = 0; i < sizeof(data); i++) { //全パケットをデコード結果用の配列に取り込む（先頭と末尾を除く）
        data[i] = cobs[i + 1];
      }
      int i = 0;
      while(i <= sizeof(data)) {
        i = i + cobs[i];
        data[i - 1] = 0;
      }
      if(0 < len){
        printf("succesfully\n");
      }else if(0 >= len) {
      printf("error\n");
      }
      if(len == sizeof(cobs) && cobs[sizeof(cobs) - 1] == 0) {
        memcpy(usb.bin, data, sizeof(data));
        printf("%d ", usb.joy[0].JoyStickData.button_status);
        printf("%d ", usb.joy[0].JoyStickData.Analog_L2);
        printf("%d ", usb.joy[0].JoyStickData.Analog_R2);
        printf("%d ", usb.joy[0].JoyStickData.Lx);
        printf("%d ", usb.joy[0].JoyStickData.Ly);
        printf("%d ", usb.joy[0].JoyStickData.Rx);
        printf("%d ", usb.joy[0].JoyStickData.Ry);
      }
      printf("\n");
    }
    rclcpp::TimerBase::SharedPtr timer_, timer_2;
    // rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr publisher_;
    size_t count_;
};

int main(int argc, char ** argv)
{                       
    struct termios tio;                 // シリアル通信設定
    int baudRate = B115200;

    fd = open(SERIAL_PORT, O_RDWR);     // デバイスをオープンする
    if (fd < 0) {
        printf("open error\n");
        return -1;
    }

    tio.c_cflag += CREAD;               // 受信有効
    tio.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
    tio.c_cflag += CS8;                 // データビット:8bit
    tio.c_cflag += 0;                   // ストップビット:1bit
    tio.c_cflag += 0;                   // パリティ:None

    cfsetispeed( &tio, baudRate );
    cfsetospeed( &tio, baudRate );

    cfmakeraw(&tio);                    // RAWモード

    tcsetattr( fd, TCSANOW, &tio );     // デバイスに設定を行う

    ioctl(fd, TCSETS, &tio);            // ポートの設定を有効にする

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<JoyPublisher>());
    // rclcpp::shutdown();

    // close(fd);                              // デバイスのクローズ
    return 0;
}

void signal_handler(int signum)
{
    printf("Signal_handler Called\n");
    rclcpp::shutdown();
    close(fd);
}