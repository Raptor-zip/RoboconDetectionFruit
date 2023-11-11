#include "rclcpp/rclcpp.hpp"
#include "BeltLiner_Controller.hpp"

BeltLiner_Controller_Node::BeltLiner_Controller_Node() : rclcpp::Node("BeltLiner_Controller")
{
    this->motion_flag = 0;

    this->Belt_Controller_msg[0].type[0] = 23;
    this->Belt_Controller_msg[0].type[1] = 180;
    this->Belt_Controller_msg[0].ID = 4;
    this->Belt_Controller_msg[0].cmd = 0;
    this->Belt_Controller_msg[0].M = 0;

    this->Belt_Controller_msg[1].type[0] = 23;
    this->Belt_Controller_msg[1].type[1] = 180;
    this->Belt_Controller_msg[1].ID = 5;
    this->Belt_Controller_msg[1].cmd = 0;
    this->Belt_Controller_msg[1].M = 0;

    this->WallMotionSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("WallMotion_BeltOut", rclcpp::QoS(10), std::bind(&BeltLiner_Controller_Node::WallMotionOut_Callback, this, std::placeholders::_1));
    this->RopeMotionOutSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("RopeMotion_BeltOut", rclcpp::QoS(10), std::bind(&BeltLiner_Controller_Node::RopeMotionOut_Callback, this, std::placeholders::_1));
    this->DriveMotionOutSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("DriveMotion_BeltOut", rclcpp::QoS(10), std::bind(&BeltLiner_Controller_Node::DriveMotionOut_Callback, this, std::placeholders::_1));
    this->KiwiMotionOutSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("KiwiMotion_BeltOut", rclcpp::QoS(10), std::bind(&BeltLiner_Controller_Node::KiwiotionOut_Callback, this, std::placeholders::_1));

    this->Belt_Controller_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("BeltOut", rclcpp::QoS(10));
    this->timer_ = this->create_wall_timer(1ms, std::bind(&BeltLiner_Controller_Node::timer_callback, this));
}

BeltLiner_Controller_Node::~BeltLiner_Controller_Node()
{
}

void BeltLiner_Controller_Node::KiwiotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (md_msg->data[16] == 1)
    {
        if (this->motion_flag <= 4)
        {
            this->motion_flag = 4;
        }
    }
    else if (this->motion_flag == 4)
    {
        this->motion_flag = 0;
    }

    for (int i = 0; i < 8; i++)
    {
        this->KiwiMotion_msg[0].bin[i] = md_msg->data[i];
        this->KiwiMotion_msg[1].bin[i] = md_msg->data[i + 8];
    }
}

void BeltLiner_Controller_Node::WallMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (md_msg->data[16] == 1)
    {
        if (this->motion_flag <= 3)
        {
            this->motion_flag = 3;
        }
    }
    else if (this->motion_flag == 3)
    {
        this->motion_flag = 0;
    }

    for (int i = 0; i < 8; i++)
    {
        this->WallMotion_msg[0].bin[i] = md_msg->data[i];
        this->WallMotion_msg[1].bin[i] = md_msg->data[i + 8];
    }
}

void BeltLiner_Controller_Node::RopeMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (md_msg->data[16] == 1)
    {
        if (this->motion_flag <= 2)
        {
            this->motion_flag = 2;
        }
    }
    else if (this->motion_flag == 2)
    {
        this->motion_flag = 0;
    }

    for (int i = 0; i < 8; i++)
    {
        this->RopeMotion_msg[0].bin[i] = md_msg->data[i];
        this->RopeMotion_msg[1].bin[i] = md_msg->data[i + 8];
    }
}

void BeltLiner_Controller_Node::DriveMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (md_msg->data[16] == 1)
    {
        if (this->motion_flag <= 1)
        {
            this->motion_flag = 1;
        }
    }
    else if (this->motion_flag == 1)
    {
        this->motion_flag = 0;
    }

    for (int i = 0; i < 8; i++)
    {
        this->DriveMotion_msg[0].bin[i] = md_msg->data[i];
        this->DriveMotion_msg[1].bin[i] = md_msg->data[i + 8];
    }
}

void BeltLiner_Controller_Node::timer_callback(void)
{
    if (this->motion_flag == 4)
    {
        this->Belt_Controller_msg[0] = this->KiwiMotion_msg[0];
        this->Belt_Controller_msg[1] = this->KiwiMotion_msg[1];
    }
    else if (this->motion_flag == 3)
    {
        this->Belt_Controller_msg[0] = this->WallMotion_msg[0];
        this->Belt_Controller_msg[1] = this->WallMotion_msg[1];
    }
    else if (this->motion_flag == 2)
    {
        this->Belt_Controller_msg[0] = this->RopeMotion_msg[0];
        this->Belt_Controller_msg[1] = this->RopeMotion_msg[1];
    }
    else if (this->motion_flag == 1)
    {
        this->Belt_Controller_msg[0] = this->DriveMotion_msg[0];
        this->Belt_Controller_msg[1] = this->DriveMotion_msg[1];
    }
    else if (this->motion_flag == 0)
    {
        this->Belt_Controller_msg[0].cmd = 0;
        this->Belt_Controller_msg[0].M = 0;
        this->Belt_Controller_msg[1].cmd = 0;
        this->Belt_Controller_msg[1].M = 0;
    }

    std_msgs::msg::ByteMultiArray pub_msg;
    pub_msg.data.resize(16);
    for (int i = 0; i < 8; i++)
    {
        pub_msg.data[i] = this->Belt_Controller_msg[0].bin[i];
        pub_msg.data[i + 8] = this->Belt_Controller_msg[1].bin[i];
    }
    this->Belt_Controller_publisher->publish(pub_msg);
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<BeltLiner_Controller_Node>());
    rclcpp::shutdown();
    return 0;
}
