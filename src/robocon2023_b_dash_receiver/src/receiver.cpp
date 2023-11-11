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
#include <math.h>
#define MARGIN_OF_ERROR 2.0E-2
#include "robocon2023_b_dash_receiver/DualShock.h"
#define SERIAL_PORT "/dev/ttyUSB0"
int fd;      // ファイルディスクリプタ
int len;

typedef union {
  DualShock_TypeDef joy[2];
  uint8_t bin[16];
}Robocon2023_USB920_recv_TypeDef;

Robocon2023_USB920_recv_TypeDef usb;

uint8_t data[16];
uint8_t cobs[sizeof(data) + 2];
using namespace std::chrono_literals;

class JoyPublisher : public rclcpp::Node
{
  public:
    JoyPublisher()
    : Node("Joy_Publisher")
    {
      publisher_1 = this->create_publisher<sensor_msgs::msg::Joy>("joy", rclcpp::QoS(10));
      publisher_2 = this->create_publisher<sensor_msgs::msg::Joy>("joy2", rclcpp::QoS(10));
      timer_ = this->create_wall_timer(
      50ms, std::bind(&JoyPublisher::topic_callback, this));
      timer_2 = this->create_wall_timer(
      5ms, std::bind(&JoyPublisher::timer_callback, this));
    }
  private:
    void topic_callback()
    {
      sensor_msgs::msg::Joy joy_msg[2];
      if(len == sizeof(cobs) && cobs[sizeof(cobs) - 1] == 0) {
        RCLCPP_INFO(this->get_logger(), "I got joy data!!");
        for(int j = 0; j < 2; j++) {
          joy_msg[j].axes.resize(8);
          joy_msg[j].buttons.resize(13);
          joy_msg[j].header.frame_id = std::string("joy");
          joy_msg[j].header.stamp = this->get_clock()->now();
          joy_msg[j].axes[0] = (-usb.joy[j].JoyStickData.Lx / 127.5) + 1;
          joy_msg[j].axes[1] = (usb.joy[j].JoyStickData.Ly / 127.5) - 1;
          joy_msg[j].axes[2] = (-usb.joy[j].JoyStickData.Analog_L2 / 127.5) + 1;
          joy_msg[j].axes[3] = (-usb.joy[j].JoyStickData.Rx / 127.5) + 1;
          joy_msg[j].axes[4] = (usb.joy[j].JoyStickData.Ry / 127.5) - 1;
          joy_msg[j].axes[5] = (-usb.joy[j].JoyStickData.Analog_R2 / 127.5) + 1;

          for(int i = 0; i < 5; i++) {
            if(i != 2) {
              if(fabsf(joy_msg[j].axes[i]) < MARGIN_OF_ERROR) {
                joy_msg[j].axes[i] = 0;
              }
            }
          }
          if(usb.joy[j].JoyStickData.Analog_L2 != 0) {
            joy_msg[j].buttons[6] = 1;
          }else {
            joy_msg[j].buttons[6] = 0;
          }
          if(usb.joy[j].JoyStickData.Analog_R2 != 0) {
            joy_msg[j].buttons[7] = 1;
          }else {
            joy_msg[j].buttons[7] = 0;
          }
          uint8_t bit[16];
            for(int i = 0; i < 16; i++) {
              bit[i] = (usb.joy[j].JoyStickData.button_status >> i) & 1;
            }
            for(int i = 0; i < 6; i++) {
              if(bit[i] == 1) {
                joy_msg[j].buttons[i] = 1;
              }else {
                joy_msg[j].buttons[i] = 0;
              }
            }
            for(int i = 8; i < 13; i++) {
              if(bit[i - 2] == 1) {
                joy_msg[j].buttons[i] = 1;
              }else {
                joy_msg[j].buttons[i] = 0;
              }
            }
            if(bit[11] == 1) {
              joy_msg[j].axes[6] = 1;
            }else if(bit[13] == 1) {
              joy_msg[j].axes[6] = -1;
            }else {
              joy_msg[j].axes[6] = 0;
            }
            if(bit[12] == 1) {
              joy_msg[j].axes[7] = 1;
            }else if(bit[14] == 1) {
              joy_msg[j].axes[7] = -1;
            }else {
              joy_msg[j].axes[7] = 0;
            }
        }
      }
      publisher_1->publish(joy_msg[0]);
      publisher_2->publish(joy_msg[1]);
    }
    void timer_callback()
    {
      len = read(fd, cobs, sizeof(cobs));
      tcflush(fd, TCIFLUSH);
      if(0 < len && len == sizeof(cobs) && cobs[sizeof(cobs) - 1] == 0){
        for (int i = 0; i < sizeof(data); i++) { //全パケットをデコード結果用の配列に取り込む（先頭と末尾を除く）
        data[i] = cobs[i + 1];
      }
      int i = 0;
      while(i <= sizeof(data)) {
        i = i + cobs[i];
        data[i - 1] = 0;
      }
      memcpy(usb.bin, data, sizeof(data));
        RCLCPP_INFO(this->get_logger(), "successfully");
      }else if(0 >= len) {
      RCLCPP_INFO(this->get_logger(), "error");
      }
    }
    rclcpp::TimerBase::SharedPtr timer_, timer_2;
    rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr publisher_1, publisher_2;
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
    rclcpp::shutdown();

    close(fd);                              // デバイスのクローズ
    return 0;
}

