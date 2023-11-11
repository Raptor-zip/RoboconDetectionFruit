#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstdint>
#include <string.h>
#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include <memory>
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "robocon2023_b_dash_transceiver/DualShock.h"
#include <stdlib.h>

#define SERIAL_PORT "/dev/ttyUSB0"

typedef struct  {
    // uint8_t adress[2];
    // uint8_t channel;
    uint8_t cobs[sizeof(DualShock_TypeDef) * 2 + 2];
}Data_TypeDef;

typedef union {
    Data_TypeDef data;
    uint8_t bin[18];
} Robocon2023_USB920_snd_TypeDef;

Robocon2023_USB920_snd_TypeDef usb;

typedef union {
    DualShock_TypeDef data[2];
    uint8_t bin[16];
}DS_data_TypeDef;

DS_data_TypeDef DS_data;

uint8_t cobs[sizeof(DualShock_TypeDef) * 2 + 2];

int fd;  
using std::placeholders::_1;

using namespace std::chrono_literals;

class JoySubscriber : public rclcpp::Node
{
public:
    JoySubscriber()
        : Node("Joy_Subscriber")
    {
        subscription_1 = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy1", rclcpp::QoS(10), std::bind(&JoySubscriber::topic_callback1, this, _1));
        subscription_2 = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy2", rclcpp::QoS(10), std::bind(&JoySubscriber::topic_callback2, this, _1));
        timer_ = this->create_wall_timer(100ms, std::bind(&JoySubscriber::timer_callback, this));
    }
private:
    void topic_callback1(const sensor_msgs::msg::Joy::SharedPtr recv_msg)
    {
        // RCLCPP_INFO(this->get_logger(), "successfully");
        sensor_msgs::msg::Joy *joy_msg = recv_msg.get();
        for(int i = 0; i < 6; i++) {
            if(joy_msg->buttons[i] == 1) {
                DS_data.data[0].JoyStickData.button_status |= (1 << i);
            }else {
                DS_data.data[0].JoyStickData.button_status &= ~(1 << i);
            }
        }
        for(int i = 8; i < 13; i++) {
            if(joy_msg->buttons[i] == 1) {
                DS_data.data[0].JoyStickData.button_status |= (1 << i - 2);
            }else {
                DS_data.data[0].JoyStickData.button_status &= ~(1 << i - 2);
            }
        }
        for(int i = 0; i < 2; i++) {
            if(joy_msg->axes[6 + i] == 1) {
                DS_data.data[0].JoyStickData.button_status |= (1 << 11 + i);
                DS_data.data[0].JoyStickData.button_status &= ~(1 << 13 + i);
            }else if(joy_msg->axes[6 + i] == -1) {
                DS_data.data[0].JoyStickData.button_status |= (1 << 13 + i);
                DS_data.data[0].JoyStickData.button_status &= ~(1 << 11 + i);
            }else {
                DS_data.data[0].JoyStickData.button_status &= ~(1 << 11 + i);
                DS_data.data[0].JoyStickData.button_status &= ~(1 << 13 + i);
            }
        }
        DS_data.data[0].JoyStickData.button_status &= ~(1 << 15);
        DS_data.bin[2] = (-joy_msg->axes[2] + 1.0) * 127.5;
        DS_data.bin[3] = (-joy_msg->axes[5] + 1.0) * 127.5;
        DS_data.bin[4] = (-joy_msg->axes[0] + 1.0) * 127.5;
        DS_data.bin[5] = (joy_msg->axes[1] + 1.0) * 127.5;
        DS_data.bin[6] = (-joy_msg->axes[3] + 1.0) * 127.5;
        DS_data.bin[7] = (joy_msg->axes[4] + 1.0) * 127.5;
        
        uint8_t count = 0;
        int mark = 0;
        for(int i = 0; i < sizeof(DS_data.bin) + 1; i++) {
            if(DS_data.bin[i] != 0x00) {
                cobs[i + 1] = DS_data.bin[i];
                count++;
            }else {
                count++;
                cobs[mark] = count;
                mark = i + 1;
                count = 0;
                cobs[mark] = count;
            }
        }
        cobs[mark] = count;
        cobs[sizeof(DS_data.bin) + 1] = 0;
        memcpy(usb.data.cobs, cobs, sizeof(cobs));
        for(int i = 0; i < sizeof(cobs); i++) {
            printf("%d ", cobs[i]);
        }
        printf("\n");
    }
    void topic_callback2(const sensor_msgs::msg::Joy::SharedPtr recv_msg)
    {
        // RCLCPP_INFO(this->get_logger(), "successfully");
        sensor_msgs::msg::Joy *joy_msg = recv_msg.get();
        for(int i = 0; i < 6; i++) {
            if(joy_msg->buttons[i] == 1) {
                DS_data.data[1].JoyStickData.button_status |= (1 << i);
            }else {
                DS_data.data[1].JoyStickData.button_status &= ~(1 << i);
            }
        }
        for(int i = 8; i < 13; i++) {
            if(joy_msg->buttons[i] == 1) {
                DS_data.data[1].JoyStickData.button_status |= (1 << i - 2);
            }else {
                DS_data.data[1].JoyStickData.button_status &= ~(1 << i - 2);
            }
        }
        for(int i = 0; i < 2; i++) {
            if(joy_msg->axes[6 + i] == 1) {
                DS_data.data[1].JoyStickData.button_status |= (1 << 11 + i);
                DS_data.data[1].JoyStickData.button_status &= ~(1 << 13 + i);
            }else if(joy_msg->axes[6 + i] == -1) {
                DS_data.data[1].JoyStickData.button_status |= (1 << 13 + i);
                DS_data.data[1].JoyStickData.button_status &= ~(1 << 11 + i);
            }else {
                DS_data.data[1].JoyStickData.button_status &= ~(1 << 11 + i);
                DS_data.data[1].JoyStickData.button_status &= ~(1 << 13 + i);
            }
        }
        DS_data.data[1].JoyStickData.button_status &= ~(1 << 15);
        DS_data.bin[10] = (-joy_msg->axes[2] + 1.0) * 127.5;
        DS_data.bin[11] = (-joy_msg->axes[5] + 1.0) * 127.5;
        DS_data.bin[12] = (-joy_msg->axes[0] + 1.0) * 127.5;
        DS_data.bin[13] = (joy_msg->axes[1] + 1.0) * 127.5;
        DS_data.bin[14] = (-joy_msg->axes[3] + 1.0) * 127.5;
        DS_data.bin[15] = (joy_msg->axes[4] + 1.0) * 127.5;
        
        uint8_t count = 0;
        int mark = 0;
        for(int i = 0; i < sizeof(DS_data.bin) + 1; i++) {
            if(DS_data.bin[i] != 0x00) {
                cobs[i + 1] = DS_data.bin[i];
                count++;
            }else {
                count++;
                cobs[mark] = count;
                mark = i + 1;
                count = 0;
                cobs[mark] = count;
            }
        }
        cobs[mark] = count;
        cobs[sizeof(DS_data.bin) + 1] = 0;
        memcpy(usb.data.cobs, cobs, sizeof(cobs));
        for(int i = 0; i < sizeof(cobs); i++) {
            printf("%d ", cobs[i]);
        }
        printf("\n");
    }
    void timer_callback() {
        write(fd, usb.bin, sizeof(usb.bin));
        tcflush(fd, TCOFLUSH);
    }
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_1, subscription_2;
    rclcpp::TimerBase::SharedPtr timer_;
};
    
int main(int argc, char ** argv)
{              
    struct termios tio;                 // シリアル通信設定
    int baudRate = B115200;
    int i;

    // usb.data.adress[0] = 0xCC;
    // usb.data.adress[1] = 0x66;
    // usb.data.channel = 0x0F;

    fd = open(SERIAL_PORT, O_RDWR);     // デバイスをオープンする
    if (fd < 0) {
        printf("open error\n");
        return -1;
    }

    tio.c_cflag += CREAD;               // 受信有効
    tio.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
    tio.c_cflag += CS8;                 // データビット:8bit
    tio.c_cflag += 0;                   // ストップビット:1bit
    tio.c_cflag += 0;                   // パリティ:None

    cfsetispeed( &tio, baudRate );
    cfsetospeed( &tio, baudRate );

    cfmakeraw(&tio);                    // RAWモード

    tcsetattr( fd, TCSANOW, &tio );     // デバイスに設定を行う

    ioctl(fd, TCSETS, &tio);            // ポートの設定を有効にする

    rclcpp::init(argc, argv);  
    rclcpp::spin(std::make_shared<JoySubscriber>());
    rclcpp::shutdown();
    close(fd);                              // デバイスのクローズ
    return 0;
}