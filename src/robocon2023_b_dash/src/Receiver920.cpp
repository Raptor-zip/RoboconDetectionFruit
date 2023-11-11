#include "rclcpp/rclcpp.hpp"
#include "Receiver920.hpp"

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyUSB0"

Receiver920_Node::Receiver920_Node() : rclcpp::Node("Receiver920")
{

    this->Joy1_publiser = this->create_publisher<sensor_msgs::msg::Joy>("joy", rclcpp::QoS(10));
    this->Joy2_publiser = this->create_publisher<sensor_msgs::msg::Joy>("joy2", rclcpp::QoS(10));

    for (int i = 0; i < 16; i++)
    {
        this->receive_compress_data[i] = 0;
    }

    this->receive_joy_msg[0].axes.resize(8, 0.0f);
    this->receive_joy_msg[0].buttons.resize(13, 0);
    this->receive_joy_msg[1].axes.resize(8, 0.0f);
    this->receive_joy_msg[1].buttons.resize(13, 0);

    this->timer_ = this->create_wall_timer(1ms, std::bind(&Receiver920_Node::timer_callback, this));

    this->SerialOpen();
}

Receiver920_Node::~Receiver920_Node()
{
    this->SerialClose();
}

void Receiver920_Node::ExpandJoyData(void)
{
    for (int i = 0; i < 2; i++)
    {

        this->receive_joy_msg[i].axes[0] = (this->receive_compress_data[0 + i * 8] - 127.0) / 127.0; // lx
        this->receive_joy_msg[i].axes[1] = (this->receive_compress_data[1 + i * 8] - 127.0) / 127.0; // ly
        this->receive_joy_msg[i].axes[2] = (this->receive_compress_data[2 + i * 8] - 127.0) / 127.0; // l3
        this->receive_joy_msg[i].axes[3] = (this->receive_compress_data[3 + i * 8] - 127.0) / 127.0; // rx
        this->receive_joy_msg[i].axes[4] = (this->receive_compress_data[4 + i * 8] - 127.0) / 127.0; // ry
        this->receive_joy_msg[i].axes[5] = (this->receive_compress_data[5 + i * 8] - 127.0) / 127.0; // r3

        for (int j = 0; j < 8; j++)
        {
            this->receive_joy_msg[i].buttons[j] = (this->receive_compress_data[6 + i * 8] >> j) & 0b01;
        }

        for (int j = 0; j < 3; j++)
        {

            this->receive_joy_msg[i].buttons[j + 8] = (this->receive_compress_data[7 + i * 8] >> j) & 0b01;
        }

        this->receive_joy_msg[i].axes[6] =
            ((this->receive_compress_data[7 + i * 8] >> 5) & 0b01) * -1 + ((this->receive_compress_data[7 + i * 8] >> 7) & 0b01) * 1;
        this->receive_joy_msg[i].axes[7] =
            ((this->receive_compress_data[7 + i * 8] >> 4) & 0b01) * -1 + ((this->receive_compress_data[7 + i * 8] >> 6) & 0b01) * 1;
    }
}

void Receiver920_Node::DecodeCOBSdata(void)
{
    uint8_t temp_cobs[sizeof(this->receive_buf)];
    for (int i = 0; i < sizeof(temp_cobs); i++)
    {
        temp_cobs[i] = this->receive_buf[i];
    }

    int i = 0;
    int next_mark = temp_cobs[i];
    while (i < sizeof(this->receive_buf) - 1)
    {
        if ((i < 16) && (temp_cobs[i] == 0x00) && (temp_cobs[i + 1] == 0x00))
        {
            RCLCPP_INFO(this->get_logger(), "cobs error\n%d ", i);

            RCLCPP_INFO(this->get_logger(), "cob %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                        temp_cobs[0], temp_cobs[1],
                        temp_cobs[2], temp_cobs[3],
                        temp_cobs[4], temp_cobs[5],
                        temp_cobs[6], temp_cobs[7],
                        temp_cobs[8], temp_cobs[9],
                        temp_cobs[10], temp_cobs[11],
                        temp_cobs[12], temp_cobs[13],
                        temp_cobs[14], temp_cobs[15],
                        temp_cobs[16], temp_cobs[17]);
            break;
        }
        i += next_mark;
        next_mark = temp_cobs[i];
        temp_cobs[i] = 0x00;
    }

    for (int i = 0; i < sizeof(this->receive_compress_data); i++)
    {
        this->receive_compress_data[i] = temp_cobs[i + 1];
    }
}

void Receiver920_Node::SerialOpen(void)
{
    this->serial_port = open(MODEMDEVICE, O_RDWR | O_NOCTTY);
    if (this->serial_port < 0)
    {
        RCLCPP_INFO(this->get_logger(), "open error\n");
        rclcpp::shutdown();
    }
    RCLCPP_INFO(this->get_logger(), "Serial port Opened\n");

    struct termios tio; // シリアル通信設定
    bzero(&tio, sizeof(tio));
    tio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    tio.c_iflag = IGNPAR;
    tio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    tio.c_lflag = 0;

    tio.c_cc[VTIME] = 0; /* キャラクタ間タイマは未使用 */
    tio.c_cc[VMIN] = 0;  /* 18文字受け取るまでブロックする */

    tcflush(this->serial_port, TCIFLUSH);
    tcsetattr(this->serial_port, TCSANOW, &tio);
}

int Receiver920_Node::SerialRead(void)
{
    return read(this->serial_port, this->receive_buf, sizeof(this->receive_buf));
}

void Receiver920_Node::SerialClose(void)
{
    RCLCPP_INFO(this->get_logger(), "Serial Closed\n");
    close(this->serial_port);
}

void Receiver920_Node::timer_callback(void)
{
    double now_time = Receiver920_Node::now().seconds();

    int res = this->SerialRead();

    if (res == 18)
    {
        RCLCPP_INFO(this->get_logger(), "return value %d", res);
        // RCLCPP_INFO(this->get_logger(), "rec %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
        //             this->receive_compress_data[0], this->receive_compress_data[1],
        //             this->receive_compress_data[2], this->receive_compress_data[3],
        //             this->receive_compress_data[4], this->receive_compress_data[5],
        //             this->receive_compress_data[6], this->receive_compress_data[7],
        //             this->receive_compress_data[8], this->receive_compress_data[9],
        //             this->receive_compress_data[10], this->receive_compress_data[11],
        //             this->receive_compress_data[12], this->receive_compress_data[13],
        //             this->receive_compress_data[14], this->receive_compress_data[15]);
        RCLCPP_INFO(this->get_logger(), "cob %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                    this->receive_buf[0], this->receive_buf[1],
                    this->receive_buf[2], this->receive_buf[3],
                    this->receive_buf[4], this->receive_buf[5],
                    this->receive_buf[6], this->receive_buf[7],
                    this->receive_buf[8], this->receive_buf[9],
                    this->receive_buf[10], this->receive_buf[11],
                    this->receive_buf[12], this->receive_buf[13],
                    this->receive_buf[14], this->receive_buf[15],
                    this->receive_buf[16], this->receive_buf[17]);
        this->DecodeCOBSdata();

        this->ExpandJoyData();
        double now_time2 = Receiver920_Node::now().seconds();
        RCLCPP_INFO(this->get_logger(), "time %lf", now_time2 - now_time);
    }

    this->Joy1_publiser->publish(this->receive_joy_msg[0]);
    this->Joy2_publiser->publish(this->receive_joy_msg[1]);
    // // 920Mhz
    // this->transmit_data.address[0] = 0xCC;
    // this->transmit_data.address[1] = 0x66;

    // this->transmit_data.channel = 0x0C;

    // for (int i = 0; i < sizeof(this->compress_data); i++)
    // {
    //     this->transmit_data.send_data[i] = this->cobs_data[i];
    // }

    // this->SerialWrite(this->transmit_data.bin, sizeof(this->transmit_data.bin));

    // RCLCPP_INFO(this->get_logger(), "raw %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
    //             this->compress_data[0], this->compress_data[1],
    //             this->compress_data[2], this->compress_data[3],
    //             this->compress_data[4], this->compress_data[5],
    //             this->compress_data[6], this->compress_data[7],
    //             this->compress_data[8], this->compress_data[9],
    //             this->compress_data[10], this->compress_data[11],
    //             this->compress_data[12], this->compress_data[13],
    //             this->compress_data[14], this->compress_data[15]);
    // RCLCPP_INFO(this->get_logger(), "cob %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
    //             this->cobs_data[0], this->cobs_data[1],
    //             this->cobs_data[2], this->cobs_data[3],
    //             this->cobs_data[4], this->cobs_data[5],
    //             this->cobs_data[6], this->cobs_data[7],
    //             this->cobs_data[8], this->cobs_data[9],
    //             this->cobs_data[10], this->cobs_data[11],
    //             this->cobs_data[12], this->cobs_data[13],
    //             this->cobs_data[14], this->cobs_data[15],
    //             this->cobs_data[16], this->cobs_data[17]);

    //     RCLCPP_INFO(this->get_logger(), "com %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
    //                 this->receive_compress_data[0], this->receive_compress_data[1],
    //                 this->receive_compress_data[2], this->receive_compress_data[3],
    //                 this->receive_compress_data[4], this->receive_compress_data[5],
    //                 this->receive_compress_data[6], this->receive_compress_data[7],
    //                 this->receive_compress_data[8], this->receive_compress_data[9],
    //                 this->receive_compress_data[10], this->receive_compress_data[11],
    //                 this->receive_compress_data[12], this->receive_compress_data[13],
    //                 this->receive_compress_data[14], this->receive_compress_data[15]);
}

int main(int argc, char *argv[])
{

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Receiver920_Node>());
    rclcpp::shutdown();
    return 0;
}
