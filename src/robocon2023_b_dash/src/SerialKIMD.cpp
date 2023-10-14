#include "rclcpp/rclcpp.hpp"
#include "SerialKIMD.hpp"

SerialKIMD_Node::SerialKIMD_Node() : rclcpp::Node("SerialKIMD_Node")
{
    memset(this->send_msg, 0, sizeof(this->send_msg));
    for (int i = 0; i < 12; i++)
    {
        this->send_msg[i].ID = i;
    }

    this->motos_msg_header.type[0] = 23;
    this->motos_msg_header.type[1] = 150;
    this->motos_msg_header.ID = 0;
    this->motos_msg_header.cmd = 7;

    this->solenoid_msg.type[0] = 23;
    this->solenoid_msg.type[1] = 151;
    this->solenoid_msg.ID = 0;
    this->solenoid_msg.cmd = 0;

    this->BeltSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("BeltOut", rclcpp::QoS(10), std::bind(&SerialKIMD_Node::BeltOut_Callback, this, std::placeholders::_1));
    this->CrowlSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("CrawlOut", rclcpp::QoS(10), std::bind(&SerialKIMD_Node::Crowl_Callback, this, std::placeholders::_1));
    this->DriveSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("DriveOut", rclcpp::QoS(10), std::bind(&SerialKIMD_Node::Drive_Callback, this, std::placeholders::_1));
    this->UpperSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("UpperOut", rclcpp::QoS(10), std::bind(&SerialKIMD_Node::Upper_Callback, this, std::placeholders::_1));
    this->XmoveSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("XmoveOut", rclcpp::QoS(10), std::bind(&SerialKIMD_Node::Xmove_Callback, this, std::placeholders::_1));
    this->SolenoidSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("SolenoidOut", rclcpp::QoS(10), std::bind(&SerialKIMD_Node::Solenoid_Callback, this, std::placeholders::_1));

    this->timer_ = this->create_wall_timer(10ms, std::bind(&SerialKIMD_Node::timer_callback, this));
    this->SerialOpen();
}

SerialKIMD_Node::~SerialKIMD_Node()
{

    this->SerialClose();
}

void SerialKIMD_Node::BeltOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    for (int i = 0; i < 8; i++)
    {
        this->send_msg[4].bin[i] = md_msg->data[i];
        this->send_msg[5].bin[i] = md_msg->data[i + 8];
    }
}

void SerialKIMD_Node::Crowl_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    for (int i = 0; i < 8; i++)
    {
        this->send_msg[0].bin[i] = md_msg->data[i];
        this->send_msg[1].bin[i] = md_msg->data[i + 8];
        this->send_msg[2].bin[i] = md_msg->data[i + 16];
        this->send_msg[3].bin[i] = md_msg->data[i + 24];
    }
    // for (int i = 0; i < 4; i++)
    // {
    //     RCLCPP_INFO(this->get_logger(), " %dtype1,type2,id,cmd,m   %d,%d,%d,%d,%f  ", i,
    //                 this->send_msg[i].type[0], this->send_msg[i].type[1], this->send_msg[i].ID, this->send_msg[i].cmd, this->send_msg[i].M);
    // }
}

void SerialKIMD_Node::Drive_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    for (int i = 0; i < 8; i++)
    {
        this->send_msg[10].bin[i] = md_msg->data[i];
        this->send_msg[11].bin[i] = md_msg->data[i + 8];
    }
}

void SerialKIMD_Node::Upper_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    for (int i = 0; i < 8; i++)
    {
        this->send_msg[7].bin[i] = md_msg->data[i];
    }
}

void SerialKIMD_Node::Xmove_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    for (int i = 0; i < 8; i++)
    {
        this->send_msg[6].bin[i] = md_msg->data[i];
    }
}

void SerialKIMD_Node::Solenoid_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    for (int i = 0; i < 8; i++)
    {
        this->solenoid_msg.bin[i] = md_msg->data[i];
    }
}

void SerialKIMD_Node::SerialOpen(void)
{
    this->serial_port = open("/dev/ttyACM0", O_RDWR);
    if (serial_port < 0)
    {
        printf("open error\n");
        rclcpp::shutdown();
    }
    printf("Serial port Opened\n");
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
}

void SerialKIMD_Node::SerialWrite(uint8_t type2)
{
    uint8_t send_data[64] = {0};
    // this->header.type[1] = type2;
    if (type2 == 150) // Motor
    {
        this->motos_msg_header.cmd = 7;
        memcpy(send_data, this->motos_msg_header.bin, sizeof(ABU2023_GenericOutput_Header_Typedef));

        for (int i = 0; i < 7; i++)
        {
            memcpy(&send_data[(i + 1) * 8], this->send_msg[i].bin, sizeof(ABU2023_GenericOutput_Header_Typedef));
        }
        write(this->serial_port, send_data, sizeof(send_data));

        // RCLCPP_INFO(this->get_logger(), " type1,type2,id,cmd,m   %d,%d,%d,%d,%f  ",
        //             this->send_msg[0].type[0], this->send_msg[0].type[1], this->send_msg[0].ID, this->send_msg[0].cmd, this->send_msg[0].M);
        // RCLCPP_INFO(this->get_logger(), "%d,%d,%d,%d,%d,%d,%d,%d", send_data[8], send_data[9], send_data[10], send_data[11], send_data[12], send_data[13], send_data[14], send_data[15]);

        this->motos_msg_header.cmd = 5;
        memcpy(send_data, this->motos_msg_header.bin, sizeof(ABU2023_GenericOutput_Header_Typedef));
        for (int i = 7; i < 12; i++)
        {
            memcpy(&send_data[(i - 6) * 8], this->send_msg[i].bin, sizeof(ABU2023_GenericOutput_Header_Typedef));
        }
        write(this->serial_port, send_data, sizeof(send_data));
    }
    else if (type2 == 151) // Solenoid
    {
        write(this->serial_port, this->solenoid_msg.bin, sizeof(this->solenoid_msg.bin));
    }
}

void SerialKIMD_Node::SerialClose(void)
{
    printf("Serial Closed\n");
    close(this->serial_port);
}

void SerialKIMD_Node::timer_callback(void)
{
    this->SerialWrite(150);
    this->SerialWrite(151);
}

int main(int argc, char *argv[])
{

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SerialKIMD_Node>());
    rclcpp::shutdown();
    return 0;
}