#include <stdio.h>
#include <stdlib.h>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joy.hpp"

using std::placeholders::_1;
using namespace std::chrono_literals;
int Time;

class JoySubscriber : public rclcpp::Node
{
public:
    JoySubscriber()
        : Node("Joy_Subscriber")
    {
        subscription_1 = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy1", rclcpp::QoS(10), std::bind(&JoySubscriber::topic_callback1, this, _1));
        subscription_2 = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy2", rclcpp::QoS(10), std::bind(&JoySubscriber::topic_callback2, this, _1));
        timer_ = this->create_wall_timer(3000ms, std::bind(&JoySubscriber::timer_callback, this));
        
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
            RCLCPP_INFO(this->get_logger(), "error");
        }
        Time = 1;
    }
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_1, subscription_2;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<JoySubscriber>());
  rclcpp::shutdown();
  return 0;
}