#include "rclcpp/rclcpp.hpp"
#include "Transmitter920.hpp"

Transmitter920_Node::Transmitter920_Node() : rclcpp::Node("Transmitter920")
{
    this->Joy1Subscription = this->create_subscription<sensor_msgs::msg::Joy>("raw_joy1", rclcpp::QoS(10), std::bind(&Transmitter920_Node::Joy1_Callback, this, std::placeholders::_1));
    this->Joy2Subscription = this->create_subscription<sensor_msgs::msg::Joy>("raw_joy2", rclcpp::QoS(10), std::bind(&Transmitter920_Node::Joy2_Callback, this, std::placeholders::_1));
    this->Joy1_look_publiser = this->create_publisher<sensor_msgs::msg::Joy>("joy1_look", rclcpp::QoS(10));
    this->Joy2_look_publiser = this->create_publisher<sensor_msgs::msg::Joy>("joy2_look", rclcpp::QoS(10));

    for (int i = 0; i < 16; i++)
    {
        this->compress_data[i] = 0;
    }

    this->local_joy_msg[0].axes.resize(8, 0.0f);
    this->local_joy_msg[0].buttons.resize(13, 0);
    this->local_joy_msg[1].axes.resize(8, 0.0f);
    this->local_joy_msg[1].buttons.resize(13, 0);

    this->receive_joy_msg[0].axes.resize(8, 0.0f);
    this->receive_joy_msg[0].buttons.resize(13, 0);
    this->receive_joy_msg[1].axes.resize(8, 0.0f);
    this->receive_joy_msg[1].buttons.resize(13, 0);

    this->timer_ = this->create_wall_timer(125ms, std::bind(&Transmitter920_Node::timer_callback, this));

    this->SerialOpen();
}

Transmitter920_Node::~Transmitter920_Node()
{
    this->SerialClose();
}

void Transmitter920_Node::Joy1_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
{
    this->local_joy_msg[0] = *joy_msg;
}

void Transmitter920_Node::Joy2_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
{
    this->local_joy_msg[1] = *joy_msg;
}

void Transmitter920_Node::CompressJoyData(void)
{
    for (int i = 0; i < 2; i++)
    {
        this->compress_data[0 + i * 8] = this->local_joy_msg[i].axes[0] * 127.0 + 127.0; // lx
        this->compress_data[1 + i * 8] = this->local_joy_msg[i].axes[1] * 127.0 + 127.0; // ly
        this->compress_data[2 + i * 8] = this->local_joy_msg[i].axes[2] * 127.0 + 127.0; // l3

        this->compress_data[3 + i * 8] = this->local_joy_msg[i].axes[3] * 127.0 + 127.0; // rx
        this->compress_data[4 + i * 8] = this->local_joy_msg[i].axes[4] * 127.0 + 127.0; // ry
        this->compress_data[5 + i * 8] = this->local_joy_msg[i].axes[5] * 127.0 + 127.0; // r3

        this->compress_data[6 + i * 8] = 0;

        for (int j = 0; j < 8; j++)
        {
            if (this->local_joy_msg[i].buttons[j] == 1)
                this->compress_data[6 + i * 8] |= (0b1 << j);
            else
                this->compress_data[6 + i * 8] &= ~(0b1 << j);
        }

        this->compress_data[7 + i * 8] = 0;
        for (int j = 0; j < 3; j++)
        {
            if (this->local_joy_msg[i].buttons[j + 8] == 1)
                this->compress_data[7 + i * 8] |= (0b1 << j);
            else
                this->compress_data[7 + i * 8] &= ~(0b1 << j);
        }

        if (this->local_joy_msg[i].axes[7] < 0) // down
            this->compress_data[7 + i * 8] |= (0b1 << 4);
        else
            this->compress_data[7 + i * 8] &= ~(0b1 << 4);

        if (this->local_joy_msg[i].axes[6] < 0) // right
            this->compress_data[7 + i * 8] |= (0b1 << 5);
        else
            this->compress_data[7 + i * 8] &= ~(0b1 << 5);

        if (this->local_joy_msg[i].axes[7] > 0) // up
            this->compress_data[7 + i * 8] |= (0b1 << 6);
        else
            this->compress_data[7 + i * 8] &= ~(0b1 << 6);

        if (this->local_joy_msg[i].axes[6] > 0) // left
            this->compress_data[7 + i * 8] |= (0b1 << 7);
        else
            this->compress_data[7 + i * 8] &= ~(0b1 << 7);
    }
}

void Transmitter920_Node::ExpandJoyData(void)
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

void Transmitter920_Node::EncodeCOBSdata(void)
{
    uint8_t count = 0; // now 0x00 index num
    uint8_t mark = 0;  // last 0x00 index num
    for (int i = 0; i < sizeof(this->compress_data) + 1; i++)
        if (this->compress_data[i] != 0x00)
        {
            this->cobs_data[i + 1] = this->compress_data[i];
            count++;
        }
        else
        {
            count++;
            this->cobs_data[mark] = count;
            mark = i + 1;
            count = 0;
            this->cobs_data[mark] = count;
        }

    this->cobs_data[mark] = count;
    this->cobs_data[sizeof(this->compress_data) + 1] = 0x00;
}

void Transmitter920_Node::DecodeCOBSdata(void)
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

        i += next_mark;
        next_mark = temp_cobs[i];
        temp_cobs[i] = 0x00;
    }

    for (int i = 0; i < sizeof(this->receive_compress_data); i++)
    {
        this->receive_compress_data[i] = temp_cobs[i + 1];
    }
}

void Transmitter920_Node::SerialOpen(void)
{
    this->serial_port = open("/dev/ttyUSB0", O_RDWR);
    if (this->serial_port < 0)
    {
        RCLCPP_INFO(this->get_logger(), "open error\n");
        rclcpp::shutdown();
    }
    RCLCPP_INFO(this->get_logger(), "Serial port Opened\n");
    struct termios tio;    // シリアル通信設定
    tio.c_cflag += CREAD;  // 受信有効
    tio.c_cflag += CLOCAL; // ローカルライン（モデム制御なし）
    tio.c_cflag += CS8;    // データビット:8bit
    tio.c_cflag += 0;      // ストップビット:1bit
    tio.c_cflag += 0;      // パリティ:None
    cfsetispeed(&tio, B115200);
    cfsetospeed(&tio, B115200);

    cfmakeraw(&tio); // RAWモード

    tcsetattr(this->serial_port, TCSANOW, &tio); // デバイスに設定を行う

    ioctl(this->serial_port, TCSETS, &tio); // ポートの設定を有効にする
}

void Transmitter920_Node::SerialWrite(uint8_t *data, uint8_t len)
{
    write(this->serial_port, data, len);
}

void Transmitter920_Node::SerialClose(void)
{
    RCLCPP_INFO(this->get_logger(), "Serial Closed\n");
    close(this->serial_port);
}

void Transmitter920_Node::timer_callback(void)
{

    double now_time = Transmitter920_Node::now().seconds();

    this->CompressJoyData();

    this->EncodeCOBSdata();
    for (int i = 0; i < sizeof(this->compress_data) + 2; i++)
    {
        this->receive_buf[i] = this->cobs_data[i];
    }
    this->DecodeCOBSdata();

    this->ExpandJoyData();
    this->Joy1_look_publiser->publish(this->receive_joy_msg[0]);
    this->Joy2_look_publiser->publish(this->receive_joy_msg[1]);

    // 920Mhz
    this->transmit_data.address[0] = 0xCC;
    this->transmit_data.address[1] = 0x66;

    this->transmit_data.channel = 0x0C;

    for (int i = 0; i < sizeof(this->cobs_data); i++)
    {
        this->transmit_data.send_data[i] = this->cobs_data[i];
    }

    this->SerialWrite(this->transmit_data.bin, sizeof(this->transmit_data.bin));

    // RCLCPP_INFO(this->get_logger(), "raw %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
    //             this->compress_data[0], this->compress_data[1],
    //             this->compress_data[2], this->compress_data[3],
    //             this->compress_data[4], this->compress_data[5],
    //             this->compress_data[6], this->compress_data[7],
    //             this->compress_data[8], this->compress_data[9],
    //             this->compress_data[10], this->compress_data[11],
    //             this->compress_data[12], this->compress_data[13],
    //             this->compress_data[14], this->compress_data[15]);
    RCLCPP_INFO(this->get_logger(), "cob %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                this->cobs_data[0], this->cobs_data[1],
                this->cobs_data[2], this->cobs_data[3],
                this->cobs_data[4], this->cobs_data[5],
                this->cobs_data[6], this->cobs_data[7],
                this->cobs_data[8], this->cobs_data[9],
                this->cobs_data[10], this->cobs_data[11],
                this->cobs_data[12], this->cobs_data[13],
                this->cobs_data[14], this->cobs_data[15],
                this->cobs_data[16], this->cobs_data[17]);

    //     RCLCPP_INFO(this->get_logger(), "com %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
    //                 this->receive_compress_data[0], this->receive_compress_data[1],
    //                 this->receive_compress_data[2], this->receive_compress_data[3],
    //                 this->receive_compress_data[4], this->receive_compress_data[5],
    //                 this->receive_compress_data[6], this->receive_compress_data[7],
    //                 this->receive_compress_data[8], this->receive_compress_data[9],
    //                 this->receive_compress_data[10], this->receive_compress_data[11],
    //                 this->receive_compress_data[12], this->receive_compress_data[13],
    //                 this->receive_compress_data[14], this->receive_compress_data[15]);

    // RCLCPP_INFO(this->get_logger(), "raw %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d ",
    //             this->transmit_data.bin[0], this->transmit_data.bin[1],
    //             this->transmit_data.bin[2], this->transmit_data.bin[3],
    //             this->transmit_data.bin[4], this->transmit_data.bin[5],
    //             this->transmit_data.bin[6], this->transmit_data.bin[7],
    //             this->transmit_data.bin[8], this->transmit_data.bin[9],
    //             this->transmit_data.bin[10], this->transmit_data.bin[11],
    //             this->transmit_data.bin[12], this->transmit_data.bin[13],
    //             this->transmit_data.bin[14], this->transmit_data.bin[15],
    //             this->transmit_data.bin[16], this->transmit_data.bin[17],
    //             this->transmit_data.bin[18], this->transmit_data.bin[19], this->transmit_data.bin[20]);

    double now_time2 = Transmitter920_Node::now().seconds();
    RCLCPP_INFO(this->get_logger(), "time %lf", now_time2 - now_time);
}

int main(int argc, char *argv[])
{

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Transmitter920_Node>());
    rclcpp::shutdown();
    return 0;
}
