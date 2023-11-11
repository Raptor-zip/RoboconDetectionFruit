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

class Transmitter920_Node : public rclcpp::Node
{
private:
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr Joy1Subscription, Joy2Subscription;
    rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr Joy1_look_publiser, Joy2_look_publiser;

    void Joy1_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg);
    void Joy2_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg);

    sensor_msgs::msg::Joy local_joy_msg[2], receive_joy_msg[2];

    void CompressJoyData(void);
    void ExpandJoyData(void);

    uint8_t compress_data[16], receive_compress_data[16];

    int serial_port;
    rclcpp::TimerBase::SharedPtr timer_;

    void EncodeCOBSdata(void);
    void DecodeCOBSdata(void);
    uint8_t cobs_data[sizeof(compress_data) + 2],
        receive_buf[sizeof(compress_data) + 2];

    TransmitData_920Mhz transmit_data;

public:
    Transmitter920_Node();
    ~Transmitter920_Node();
    void SerialOpen(void);
    void SerialWrite(uint8_t *data, uint8_t len);
    void SerialClose(void);
    void timer_callback(void);
};