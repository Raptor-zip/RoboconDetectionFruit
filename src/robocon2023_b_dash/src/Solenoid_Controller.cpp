#include "rclcpp/rclcpp.hpp"
#include "Solenoid_Controller.hpp"

Solenoid_Controller_Node::Solenoid_Controller_Node() : rclcpp::Node("Solenoid_Controller")
{
    this->motion_flag = 0;

    this->Solenoid_Controll_msg.type[0] = 23;
    this->Solenoid_Controll_msg.type[1] = 151;
    this->Solenoid_Controll_msg.ID = 0;
    this->Solenoid_Controll_msg.cmd = 0;
    this->Solenoid_Controll_msg.M = 0;

    this->WallMotionSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("WallMotion_SolenoidOut", rclcpp::QoS(10), std::bind(&Solenoid_Controller_Node::WallMotionOut_Callback, this, std::placeholders::_1));
    this->RopeMotionOutSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("RopeMotion_SolenoidOut", rclcpp::QoS(10), std::bind(&Solenoid_Controller_Node::RopeMotionOut_Callback, this, std::placeholders::_1));

    this->Solenoid_Controller_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("SolenoidOut", rclcpp::QoS(10));
    this->timer_ = this->create_wall_timer(1ms, std::bind(&Solenoid_Controller_Node::timer_callback, this));
}

Solenoid_Controller_Node::~Solenoid_Controller_Node()
{
}

void Solenoid_Controller_Node::WallMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (md_msg->data[8] == 1)
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
        this->WallMotion_msg.bin[i] = md_msg->data[i];
    }
    // for (int i = 0; i < 4; i++)
    // {
    //     RCLCPP_INFO(this->get_logger(), " %dtype1,type2,id,cmd,m   %d,%d,%d,%d,%f  ", i,
    //                 this->WallMotion_msg[i].type[0], this->WallMotion_msg[i].type[1], this->WallMotion_msg[i].ID, this->WallMotion_msg[i].cmd, this->WallMotion_msg[i].M);
    // }
}

void Solenoid_Controller_Node::RopeMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (md_msg->data[8] == 1)
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
        this->RopeMotion_msg.bin[i] = md_msg->data[i];
    }
}

void Solenoid_Controller_Node::timer_callback(void)
{
    if (this->motion_flag == 3)
    {
        this->Solenoid_Controll_msg = this->WallMotion_msg;
    }
    else if (this->motion_flag == 2)
    {
        this->Solenoid_Controll_msg = this->RopeMotion_msg;
    }

    std_msgs::msg::ByteMultiArray pub_msg;
    pub_msg.data.resize(8);
    for (int i = 0; i < 8; i++)
    {
        pub_msg.data[i] = this->Solenoid_Controll_msg.bin[i];
    }
    this->Solenoid_Controller_publisher->publish(pub_msg);
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Solenoid_Controller_Node>());
    rclcpp::shutdown();
    return 0;
}