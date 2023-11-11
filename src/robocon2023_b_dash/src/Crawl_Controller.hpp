// ROS,Time
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/byte_multi_array.hpp"
#include <chrono>

#include "RoboconMsg/ABU2023_GenericOutput_Header.h"

using namespace std::chrono_literals;

class Crawl_Controller_Node : public rclcpp::Node
{
private:
    rclcpp::Subscription<std_msgs::msg::ByteMultiArray>::SharedPtr
        WallMotionSubscription,
        RopeMotionOutSubscription, DriveMotionOutSubscription, KiwiMotionOutSubscription;

    void WallMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);  // 4 data
    void RopeMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);  // 4 data
    void DriveMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg); // 4 data
    void KiwiotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);   // 4 data

    rclcpp::Publisher<std_msgs::msg::ByteMultiArray>::SharedPtr Crawl_Controller_publisher;

    ABU2023_GenericOutput_Header_Typedef Crawl_Controll_msg[4], WallMotion_msg[4], RopeMotion_msg[4], KiwiMotion_msg[4];

    rclcpp::TimerBase::SharedPtr timer_;

    int motion_flag;

public:
    Crawl_Controller_Node();
    ~Crawl_Controller_Node();
    void timer_callback(void);
};