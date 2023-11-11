// ROS,Time
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include <chrono>
// Serial
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

using namespace std::chrono_literals;

// #pragma pack(1)
typedef union
{
    struct
    {
        uint8_t address[2];
        uint8_t channel;
        uint8_t send_data[16 + 2];
    };
    uint8_t bin[21];
} TransmitData_920Mhz;

class Receiver920_Node : public rclcpp::Node
{
private:
    rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr Joy1_publiser, Joy2_publiser;

    sensor_msgs::msg::Joy receive_joy_msg[2];

    // void CompressJoyData(void);
    void ExpandJoyData(void);

    uint8_t receive_compress_data[16];

    int serial_port;
    rclcpp::TimerBase::SharedPtr timer_;

    void DecodeCOBSdata(void);
    uint8_t receive_buf[sizeof(receive_compress_data) + 2];

 

public:
    Receiver920_Node();
    ~Receiver920_Node();
    void SerialOpen(void);
    int SerialRead(void);
    void SerialClose(void);
    void timer_callback(void);
};