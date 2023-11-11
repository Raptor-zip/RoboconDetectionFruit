// ROS,Time
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/byte_multi_array.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include <chrono>

#include "RoboconMsg/ABU2023_GenericOutput_Header.h"

using namespace std::chrono_literals;

class DriveMotion_Node : public rclcpp::Node
{
private:
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr JoySubscription;
    void Joy_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg);

    rclcpp::Publisher<std_msgs::msg::ByteMultiArray>::SharedPtr DriveMotion_publisher;

    ABU2023_GenericOutput_Header_Typedef DriveMotion_msg[2];

    float ly, ry;
    rclcpp::TimerBase::SharedPtr timer_;

public:
    DriveMotion_Node();
    ~DriveMotion_Node();
    void timer_callback(void);
};