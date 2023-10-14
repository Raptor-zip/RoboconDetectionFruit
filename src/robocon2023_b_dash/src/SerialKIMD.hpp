// ROS,Time
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/byte_multi_array.hpp"
#include <chrono>
// Serial
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "RoboconMsg/ABU2023_GenericOutput_Header.h"

using namespace std::chrono_literals;

class SerialKIMD_Node : public rclcpp::Node
{
private:
<<<<<<< HEAD
    rclcpp::Subscription<std_msgs::msg::ByteMultiArray>::SharedPtr BeltSubscription, CrowlSubscription, DriveSubscription, UpperSubscription, XmoveSubscription,SolenoidSubscription;
=======
    rclcpp::Subscription<std_msgs::msg::ByteMultiArray>::SharedPtr BeltSubscription, CrowlSubscription, DriveSubscription, UpperSubscription, XmoveSubscription;
>>>>>>> origin/master
    ABU2023_GenericOutput_Header_Typedef send_msg[12], motos_msg_header, solenoid_msg;

    void BeltOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg); // 2 data
    void Crowl_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);   // 4 data
    void Drive_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);   // 2 data
    void Upper_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);   // 1 data
    void Xmove_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);   // 1 data
<<<<<<< HEAD
    void Solenoid_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg);   // 1 data
=======
>>>>>>> origin/master

    int serial_port;
    rclcpp::TimerBase::SharedPtr timer_;

public:
    SerialKIMD_Node();
    ~SerialKIMD_Node();
    void SerialOpen(void);
    void SerialWrite(uint8_t type2);
    void SerialClose(void);
    void timer_callback(void);
};
