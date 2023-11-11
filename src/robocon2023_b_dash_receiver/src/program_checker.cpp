#include <stdio.h>
#include <stdlib.h>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joy.hpp"

using std::placeholders::_1;
using namespace std::chrono_literals;
int Time;
int old_time;

class JoySubscriber : public rclcpp::Node
{
public:
    JoySubscriber()
        : Node("Joy_Subscriber")
    {
        subscription_1 = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy", rclcpp::QoS(10), std::bind(&JoySubscriber::topic_callback1, this, _1));
        subscription_2 = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy2", rclcpp::QoS(10), std::bind(&JoySubscriber::topic_callback2, this, _1));
        timer_ = this->create_wall_timer(2000ms, std::bind(&JoySubscriber::timer_callback, this));
    }
private:
    void topic_callback1(const sensor_msgs::msg::Joy::SharedPtr recv_msg)
    {
        sensor_msgs::msg::Joy *joy_msg = recv_msg.get();  
        Time = joy_msg->header.stamp.nanosec; 
    }
    void topic_callback2(const sensor_msgs::msg::Joy::SharedPtr recv_msg)
    {
        sensor_msgs::msg::Joy *joy_msg = recv_msg.get();  
        Time = joy_msg->header.stamp.nanosec; 
    }
    void timer_callback()
    {
      if(Time == 1) {
        system("screen -X -S robocon2023 kill");
        system("gnome-terminal -- screen -S robocon2023 -- ros2 run robocon2023_b_dash_receiver receiver --ros-args -r  __ns:=/robocon2023_b_dash");
        RCLCPP_INFO(this->get_logger(), "error");
      }else {
        RCLCPP_INFO(this->get_logger(), "successfully");
      }
      Time = 1;
    }
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_1, subscription_2;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  system("rm -rf /var/run/screen/S-noita/*");//Please enter your computer name in "noita"
  system("gnome-terminal -- screen -S robocon2023 -- ros2 run robocon2023_b_dash_receiver receiver --ros-args -r  __ns:=/robocon2023_b_dash");
  rclcpp::spin(std::make_shared<JoySubscriber>());
  system("screen -X -S robocon2023 kill");
  rclcpp::shutdown();
  return 0;
}
