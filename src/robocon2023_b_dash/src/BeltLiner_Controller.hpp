// ROS,Time
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/byte_multi_array.hpp"
#include <chrono>

#include "RoboconMsg/ABU2023_GenericOutput_Header.h"

using namespace std::chrono_literals;

class BeltLiner_Controller_Node : public rclcpp::Node
{
private:
    rclcpp::Subscription<std_msgs::msg::ByteMultiArray>::SharedPtr
        WallMotionSubscription,
        RopeMotionOutSubscription,
        DriveMotionOutSubscription,
        KiwiMotionOutSubscription;

    void WallMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);  // 2 data
    void RopeMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);  // 2 data
    void DriveMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg); // 2 data
    void KiwiotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);   // 2 data

    rclcpp::Publisher<std_msgs::msg::ByteMultiArray>::SharedPtr Belt_Controller_publisher;

    ABU2023_GenericOutput_Header_Typedef Belt_Controller_msg[2], WallMotion_msg[2], RopeMotion_msg[2], DriveMotion_msg[2], KiwiMotion_msg[2];

    rclcpp::TimerBase::SharedPtr timer_;

    int motion_flag;

public:
    BeltLiner_Controller_Node();
    ~BeltLiner_Controller_Node();
    void timer_callback(void);
};