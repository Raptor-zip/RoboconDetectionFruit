// ROS,Time
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/byte_multi_array.hpp"
#include <chrono>

#include "RoboconMsg/ABU2023_GenericOutput_Header.h"

using namespace std::chrono_literals;

class Solenoid_Controller_Node : public rclcpp::Node
{
private:
    rclcpp::Subscription<std_msgs::msg::ByteMultiArray>::SharedPtr
        WallMotionSubscription,
        RopeMotionOutSubscription, DriveMotionOutSubscription;

    void WallMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);  // 1 data
    void RopeMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);  // 1 data
    void DriveMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg); // 1 data

    rclcpp::Publisher<std_msgs::msg::ByteMultiArray>::SharedPtr Solenoid_Controller_publisher;

    ABU2023_GenericOutput_Header_Typedef Solenoid_Controll_msg, WallMotion_msg, RopeMotion_msg;

    rclcpp::TimerBase::SharedPtr timer_;

    int motion_flag;

public:
    Solenoid_Controller_Node();
    ~Solenoid_Controller_Node();
    void timer_callback(void);
};