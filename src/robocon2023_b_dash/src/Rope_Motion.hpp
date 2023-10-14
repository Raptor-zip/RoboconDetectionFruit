// ROS,Time
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/byte_multi_array.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include <chrono>

#include "RoboconMsg/ABU2023_GenericOutput_Header.h"

using namespace std::chrono_literals;

class RopeMotion_Node : public rclcpp::Node
{
private:
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr JoySubscription;
    void Joy_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg);

    rclcpp::Publisher<std_msgs::msg::ByteMultiArray>::SharedPtr belt_publisher, crawl_publisher, drive_publisher, solenoid_publisher;

    ABU2023_GenericOutput_Header_Typedef belt_msg[2], crawl_msg[4], drive_msg[2], solenoid_msg;

    float ly, ry;
    uint8_t circle, triangle, square, cross, ps;
    uint8_t belt_motion_flag, crawl_motion_flag, drive_motion_flag, solenoid_motion_flag;
    uint8_t crawl_old_cmd[4];

    void ToggleClawlCmd(uint8_t crawl_num,uint8_t mode);
    void InitPos_Clawl(uint8_t crawl_num,uint8_t mode);
    void SetClawlIncPos(uint8_t crawl_num,uint8_t mode,float value);
    void SetClawlAbsPos(uint8_t crawl_num,uint8_t mode,float value);
    float GetClawlNowM(uint8_t crawl_num);
    float GetClawlZEROPos(uint8_t crawl_num);

    void ToggleSlolenoidCmd(void);
    bool CheckLessTime(double now_time, double *old_time, double dt);

    bool CircleOnCheck(double dt);
    bool TriangleOnChek(double dt);
    bool SquareOnChek(double dt);
    bool CrossOnChek(double dt);
    bool PsOnChek(double dt);

    rclcpp::TimerBase::SharedPtr timer_;

public:
    RopeMotion_Node();
    ~RopeMotion_Node();
    void timer_callback(void);
    void Rope_Task(void);
    void PublishMDmsg(void);
};