#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joy.hpp"

using std::placeholders::_1;

void signal_handler(int signum);

int serial_port;
uint8_t data[32] = {"hello world\n"};

class KiMDJoySubscriber : public rclcpp::Node
{
public:
  KiMDJoySubscriber()
      : Node("minimal_subscriber")
  {
    subscription_ = this->create_subscription<sensor_msgs::msg::Joy>(
        "joy", rclcpp::QoS(10), std::bind(&KiMDJoySubscriber::topic_callback, this, _1));
  }

private:
  void topic_callback(sensor_msgs::msg::Joy::SharedPtr msg)
  {
    RCLCPP_INFO(this->get_logger(), "I got joy data!!");
    data[31]++;
    write(serial_port, data, sizeof(data));
  }
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
};

int main(int argc, char *argv[])
{
  signal(SIGINT, signal_handler);
  serial_port = open("/dev/ttyACM0", O_RDWR);
  if (serial_port < 0)
  {
    printf("open error\n");
    return -1;
  }
  struct termios tio;    // シリアル通信設定
  tio.c_cflag += CREAD;  // 受信有効
  tio.c_cflag += CLOCAL; // ローカルライン（モデム制御なし）
  tio.c_cflag += CS8;    // データビット:8bit
  tio.c_cflag += 0;      // ストップビット:1bit
  tio.c_cflag += 0;      // パリティ:None
  cfsetispeed(&tio, B9600);
  cfsetospeed(&tio, B9600);

  cfmakeraw(&tio); // RAWモード

  tcsetattr(serial_port, TCSANOW, &tio); // デバイスに設定を行う

  ioctl(serial_port, TCSETS, &tio); // ポートの設定を有効にする

  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<KiMDJoySubscriber>());

  return 0;
}

void signal_handler(int signum)
{
  printf("Signal_handler Called\n");
  rclcpp::shutdown();
  close(serial_port);
}