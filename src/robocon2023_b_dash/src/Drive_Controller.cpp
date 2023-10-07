#include "rclcpp/rclcpp.hpp"
#include "Drive_Controller.hpp"

Drive_Controller_Node::Drive_Controller_Node() : rclcpp::Node("Drive_Controller")
{
    int motion_flag = 0;

    this->Drive_Controller_msg[0].type[0] = 23;
    this->Drive_Controller_msg[0].type[1] = 180;
    this->Drive_Controller_msg[0].ID = 10;
    this->Drive_Controller_msg[0].cmd = 0;
    this->Drive_Controller_msg[0].M = 0;

    this->Drive_Controller_msg[1].type[0] = 23;
    this->Drive_Controller_msg[1].type[1] = 180;
    this->Drive_Controller_msg[1].ID = 11;
    this->Drive_Controller_msg[1].cmd = 0;
    this->Drive_Controller_msg[1].M = 0;

    this->WallMotionSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("WallMotionOut", rclcpp::QoS(10), std::bind(&Drive_Controller_Node::WallMotionOut_Callback, this, std::placeholders::_1));
    this->RopeMotionOutSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("RopeMotionOut", rclcpp::QoS(10), std::bind(&Drive_Controller_Node::RopeMotionOut_Callback, this, std::placeholders::_1));
    this->DriveMotionOutSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("DriveMotionOut", rclcpp::QoS(10), std::bind(&Drive_Controller_Node::DriveMotionOut_Callback, this, std::placeholders::_1));

    this->Drive_Controller_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("DriveOut", rclcpp::QoS(10));
    this->timer_ = this->create_wall_timer(1ms, std::bind(&Drive_Controller_Node::timer_callback, this));
}

Drive_Controller_Node::~Drive_Controller_Node()
{
}

void Drive_Controller_Node::WallMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (this->motion_flag <= 3)
    {
        if (md_msg->data[16] == 1)
        {
            this->motion_flag = 3;
        }
        else
        {
            this->motion_flag = 0;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        this->WallMotion_msg[0].bin[i] = md_msg->data[i];
        this->WallMotion_msg[1].bin[i] = md_msg->data[i + 8];
    }
}

void Drive_Controller_Node::RopeMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (this->motion_flag <= 2)
    {
        if (md_msg->data[16] == 1)
        {
            this->motion_flag = 2;
        }
        else
        {
            this->motion_flag = 0;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        this->RopeMotion_msg[0].bin[i] = md_msg->data[i];
        this->RopeMotion_msg[1].bin[i] = md_msg->data[i + 8];
    }
}

void Drive_Controller_Node::DriveMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (this->motion_flag <= 1)
    {
        if (md_msg->data[16] == 1)
        {
            this->motion_flag = 1;
        }
        else
        {
            this->motion_flag = 0;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        this->DriveMotion_msg[0].bin[i] = md_msg->data[i];
        this->DriveMotion_msg[1].bin[i] = md_msg->data[i + 8];
    }
}

void Drive_Controller_Node::timer_callback(void)
{
    if (this->motion_flag == 3)
    {
        this->Drive_Controller_msg[0] = this->WallMotion_msg[0];
        this->Drive_Controller_msg[1] = this->WallMotion_msg[1];
    }
    else if (this->motion_flag == 2)
    {
        this->Drive_Controller_msg[0] = this->RopeMotion_msg[0];
        this->Drive_Controller_msg[1] = this->RopeMotion_msg[1];
    }
    else if (this->motion_flag == 1)
    {
        this->Drive_Controller_msg[0] = this->DriveMotion_msg[0];
        this->Drive_Controller_msg[1] = this->DriveMotion_msg[1];
    }
    else if (this->motion_flag == 0)
    {
        this->Drive_Controller_msg[0].cmd = 0;
        this->Drive_Controller_msg[0].M = 0;
        this->Drive_Controller_msg[1].cmd = 0;
        this->Drive_Controller_msg[1].M = 0;
    }

    std_msgs::msg::ByteMultiArray pub_msg;
    pub_msg.data.resize(16);
    for (int i = 0; i < 8; i++)
    {
        pub_msg.data[i] = this->Drive_Controller_msg[0].bin[i];
        pub_msg.data[i + 8] = this->Drive_Controller_msg[1].bin[i];
    }
    this->Drive_Controller_publisher->publish(pub_msg);
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Drive_Controller_Node>());
    rclcpp::shutdown();
    return 0;
}