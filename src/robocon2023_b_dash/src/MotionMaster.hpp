// ROS,Time
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/byte_multi_array.hpp"
#include <chrono>

#include "RoboconMsg/ABU2023_GenericOutput_Header.h"

using namespace std::chrono_literals;

// typedef struct
// {
//     ABU2023_GenericOutput_Header_Typedef crawl_msg[4], belt_msg[2], drive_msg[2], solenoid_msg;
// }AllMotion_MDmsg;

class MotionMaster_Node : public rclcpp::Node
{
private:
    rclcpp::Subscription<std_msgs::msg::ByteMultiArray>::SharedPtr
        WallMotionSubscription,
        RopeMotionOutSubscription,
        DriveMotionOutSubscription,
        KiwiMotionOutSubscription;

    void WallMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr use_msg);  // 1 byte
    void RopeMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr use_msg);  // 1 byte
    void DriveMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr use_msg); // 1 byte
    void KiwiMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr use_msg);  // 1 byte

    rclcpp::Publisher<std_msgs::msg::ByteMultiArray>::SharedPtr MotionMaster_publisher;

    rclcpp::TimerBase::SharedPtr timer_;

    int motion_flag;

public:
    MotionMaster_Node();
    ~MotionMaster_Node();
    void timer_callback(void);
};