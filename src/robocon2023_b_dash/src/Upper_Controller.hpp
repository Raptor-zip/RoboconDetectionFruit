// ROS,Time
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/byte_multi_array.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"
#include <chrono>

#include "RoboconMsg/ABU2023_GenericOutput_Header.h"

using namespace std::chrono_literals;

class Upper_Controller_Node : public rclcpp::Node
{
private:
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr JoySubscription;
    void Joy_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg);

    rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr ImageRecognition_Subscription;
    void ImageRecognition_Callback(const std_msgs::msg::Int16MultiArray ::SharedPtr recognition_msg);

    rclcpp::Publisher<std_msgs::msg::ByteMultiArray>::SharedPtr Upper_publisher;

    ABU2023_GenericOutput_Header_Typedef upper_msg;

    float option, share, x_button;
    uint8_t button_state,up_flag;
    rclcpp::TimerBase::SharedPtr timer_;

public:
    Upper_Controller_Node();
    ~Upper_Controller_Node();
    void timer_callback(void);
};