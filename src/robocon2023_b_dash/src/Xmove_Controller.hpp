// ROS,Time
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/byte_multi_array.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"
#include <chrono>

#include "RoboconMsg/ABU2023_GenericOutput_Header.h"

using namespace std::chrono_literals;

class Xmove_Controller_Node : public rclcpp::Node
{
private:
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr JoySubscription;
    void Joy_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg);

    rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr ImageRecognition_Subscription;
    void ImageRecognition_Callback(const std_msgs::msg::Int16MultiArray ::SharedPtr recognition_msg);

    rclcpp::Publisher<std_msgs::msg::ByteMultiArray>::SharedPtr Xmove_publisher;

    ABU2023_GenericOutput_Header_Typedef xmove_msg;

    float l1, r1;
    rclcpp::TimerBase::SharedPtr timer_;

public:
    Xmove_Controller_Node();
    ~Xmove_Controller_Node();
    void timer_callback(void);
};
