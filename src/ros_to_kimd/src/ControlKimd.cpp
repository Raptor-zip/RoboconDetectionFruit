#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joy.hpp"

#include "ABU2023_GenericOutput_Header.h"

#define MOTOR_NUM 12
ABU2023_GenericOutput_Header_Typedef send_md_msg[1], md_msg[MOTOR_NUM];
uint8_t send_data[64];

using std::placeholders::_1;

void signal_handler(int signum);

int serial_port;

#define CRAWL1_ZERO 249.433594
#define CRAWL2_ZERO 147.568359
#define CRAWL3_ZERO 290.039062
#define CRAWL4_ZERO 245.039062

class KiMDJoySubscriber : public rclcpp::Node
{
public:
    KiMDJoySubscriber()
        : Node("KiMDJoy_Subscriber")
    {
        subscription_ = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy", rclcpp::QoS(10), std::bind(&KiMDJoySubscriber::topic_callback, this, _1));
    }

private:
    void topic_callback(const sensor_msgs::msg::Joy::SharedPtr recv_msg)
    {
        // RCLCPP_INFO(this->get_logger(), "I got joy data!!");
        sensor_msgs::msg::Joy *joy_msg = recv_msg.get();

        // md_msg[0].M = joy_msg->axes[1] * 100.0;
        // md_msg[1].M = joy_msg->axes[1] * 100.0;
        // md_msg[2].M = joy_msg->axes[1] * 100.0;
        // md_msg[3].M = joy_msg->axes[1] * 100.0;
        // md_msg[6].cmd = 5;
        // md_msg[6].M = joy_msg->axes[1] * 400.0 / 731.551265 * 360.0;
        // rclcpp::Clock system_clock(rcl_clock_type_t RCL_SYSTEM_TIME);
        rclcpp::Time now = KiMDJoySubscriber::now();
        RCLCPP_INFO(this->get_logger(), "Publishing: '%f'", now.seconds());

        // md_msg[0].cmd = 5;
        // md_msg[1].cmd = 5;
        // md_msg[2].cmd = 5;
        // md_msg[3].cmd = 5;

        // if (joy_msg->buttons[0] == 1)
        // {
        //     md_msg[0].M++;
        // }
        // if (joy_msg->buttons[1] == 1)
        // {
        //     md_msg[1].M++;
        // }
        // if (joy_msg->buttons[2] == 1)
        // {
        //     md_msg[2].M++;
        // }
        // if (joy_msg->buttons[3] == 1)
        // {
        //     md_msg[3].M++;
        // }

        // if (joy_msg->axes[6] == 1)
        // {
        //     md_msg[0].M--;
        // }
        // if (joy_msg->axes[6] == -1)
        // {
        //     md_msg[1].M--;
        // }
        // if (joy_msg->axes[7] == 1)
        // {
        //     md_msg[2].M--;
        // }
        // if (joy_msg->axes[7] == -1)
        // {
        //     md_msg[3].M--;
        // }

        if (joy_msg->buttons[0] == 1)
        {
            md_msg[0].cmd = 5;
            md_msg[1].cmd = 5;
            md_msg[2].cmd = 5;
            md_msg[3].cmd = 5;
            md_msg[0].M = CRAWL1_ZERO;
            md_msg[1].M = CRAWL2_ZERO;
            md_msg[2].M = CRAWL3_ZERO;
            md_msg[3].M = CRAWL4_ZERO;

            md_msg[4].cmd = 0;
            md_msg[5].cmd = 0;
            md_msg[4].M = 0;
            md_msg[5].M = 0;
        }
        else if (joy_msg->buttons[1] == 1)
        {
            md_msg[0].cmd = 5;
            md_msg[1].cmd = 5;
            md_msg[2].cmd = 5;
            md_msg[3].cmd = 5;
            md_msg[0].M = CRAWL1_ZERO - 90;
            md_msg[1].M = CRAWL2_ZERO + 90;
            md_msg[2].M = CRAWL3_ZERO - 90;
            md_msg[3].M = CRAWL4_ZERO + 90;

            md_msg[4].cmd = 4;
            md_msg[5].cmd = 4;
            md_msg[4].M = 120;
            md_msg[5].M = -120;
        }

        // else if (joy_msg->buttons[2] == 1)
        // {
        //     md_msg[0].cmd = 7;
        //     md_msg[0].M = CRAWL1_ZERO - 270;
        // }
        // else if (joy_msg->buttons[3] == 1)
        // {
        //     md_msg[3].cmd = 7;
        //     md_msg[3].M = CRAWL1_ZERO + 270;
        // }
        else if (joy_msg->buttons[2] == 1)
        {
            md_msg[0].cmd = 5;
            md_msg[0].M = CRAWL1_ZERO - 180;
            md_msg[3].cmd = 5;
            md_msg[3].M = CRAWL4_ZERO + 180;
        }
        else if (joy_msg->buttons[3] == 1)
        {
            md_msg[0].cmd = 5;
            md_msg[0].M = CRAWL1_ZERO - 270;
            md_msg[3].cmd = 5;
            md_msg[3].M = CRAWL4_ZERO + 270;
        }

        md_msg[7].cmd = 5;
        if (md_msg[7].M > 25)
            md_msg[7].M -= 0.5 * joy_msg->buttons[9]; // option
        if (md_msg[7].M < 300)
            md_msg[7].M += 0.5 * joy_msg->buttons[8]; // share

        static int flag = 0;
        if (joy_msg->axes[6] == 1)
        {
            md_msg[7].M = 176; //<- blue
            flag = 0;
        }
        else if (joy_msg->axes[6] == -1)
        {
            md_msg[7].M = 135; //-> greqp
            flag = 0;
        }
        else if (joy_msg->axes[7] == 1)
        {
            md_msg[7].M = 74; // up mix
            flag = 0;
        }
        else if (joy_msg->axes[7] == -1)
        {
            if (flag == 0)
            {
                md_msg[7].M += -20; // down  up
                flag = 1;
            }
        }

        md_msg[6].M = joy_msg->buttons[4] * -30.0; // L1
        md_msg[6].M += joy_msg->buttons[5] * 30.0; // R1

        md_msg[10].M = joy_msg->axes[1] * 100.0;
        md_msg[11].M = joy_msg->axes[4] * -100.0;

        RCLCPP_INFO(this->get_logger(), "data:md_msg[7].M  %f  ", md_msg[7].M);

        // for (int i = 0; i < 8; i++)
        // {
        //     memcpy(&send_data[i * 8], send_md_msg[i].bin, sizeof(ABU2023_GenericOutput_Header_Typedef));
        // }
        send_md_msg[0].cmd = 7;
        memcpy(&send_data[0 * 8], send_md_msg[0].bin, sizeof(ABU2023_GenericOutput_Header_Typedef));
        for (int i = 0; i < 7; i++)
        {
            memcpy(&send_data[(i + 1) * 8], md_msg[i].bin, sizeof(ABU2023_GenericOutput_Header_Typedef));
        }
        write(serial_port, send_data, sizeof(send_data));

        // std::this_thread::sleep_for(std::chrono::milliseconds(5));

        send_md_msg[0].cmd = 12 - 7;
        memcpy(&send_data[0 * 8], send_md_msg[0].bin, sizeof(ABU2023_GenericOutput_Header_Typedef));
        for (int i = 7; i < MOTOR_NUM; i++)
        {
            memcpy(&send_data[(i - 6) * 8], md_msg[i].bin, sizeof(ABU2023_GenericOutput_Header_Typedef));
        }
        write(serial_port, send_data, sizeof(send_data));
    }
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
};

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);
    serial_port = open("/dev/ttyACM0", O_RDWR);
    if (serial_port < 0)
    {
        printf("open error\n");
        return -1;
    }
    struct termios tio;    // シリアル通信設定
    tio.c_cflag += CREAD;  // 受信有効
    tio.c_cflag += CLOCAL; // ローカルライン（モデム制御なし）
    tio.c_cflag += CS8;    // データビット:8bit
    tio.c_cflag += 0;      // ストップビット:1bit
    tio.c_cflag += 0;      // パリティ:None
    cfsetispeed(&tio, B921600);
    cfsetospeed(&tio, B921600);

    cfmakeraw(&tio); // RAWモード

    tcsetattr(serial_port, TCSANOW, &tio); // デバイスに設定を行う

    ioctl(serial_port, TCSETS, &tio); // ポートの設定を有効にする

    // init send_md_msg
    send_md_msg[0].type[0] = 23;
    send_md_msg[0].type[1] = 150;
    send_md_msg[0].ID = 0;
    send_md_msg[0].cmd = 7; // broadcast
    memcpy(&send_data[0 * 8], send_md_msg[0].bin, sizeof(ABU2023_GenericOutput_Header_Typedef));

    md_msg[0].cmd = 5;
    md_msg[1].cmd = 5;
    md_msg[2].cmd = 5;
    md_msg[3].cmd = 5;
    md_msg[0].M = CRAWL1_ZERO;
    md_msg[1].M = CRAWL2_ZERO;
    md_msg[2].M = CRAWL3_ZERO;
    md_msg[3].M = CRAWL4_ZERO;
    // for (int i = 1; i < 8; i++)
    // {
    //     send_md_msg[i].type[0] = 23;
    //     send_md_msg[i].type[1] = 180;
    //     send_md_msg[i].ID = i-1;
    //     send_md_msg[i].cmd = 0;
    //     send_md_msg[i].M = 0;
    // }
    for (int i = 0; i < MOTOR_NUM; i++)
    {
        md_msg[i].type[0] = 23;
        md_msg[i].type[1] = 180;
        md_msg[i].ID = i;
        md_msg[i].cmd = 0;
        md_msg[i].M = 0;
    }
    md_msg[7].M = 250;
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<KiMDJoySubscriber>());

    return 0;
}

void signal_handler(int signum)
{
    printf("Signal_handler Called\n");
    rclcpp::shutdown();
    close(serial_port);
}