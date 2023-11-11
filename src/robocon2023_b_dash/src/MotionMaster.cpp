#include "rclcpp/rclcpp.hpp"
#include "MotionMaster.hpp"

MotionMaster_Node::MotionMaster_Node() : rclcpp::Node("MotionMaster")
{
    this->motion_flag = 0;

    this->WallMotionSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("WallMotion_UseOut", rclcpp::QoS(10), std::bind(&MotionMaster_Node::WallMotionOut_Callback, this, std::placeholders::_1));
    this->RopeMotionOutSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("RopeMotion_UseOut", rclcpp::QoS(10), std::bind(&MotionMaster_Node::RopeMotionOut_Callback, this, std::placeholders::_1));
    this->DriveMotionOutSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("DriveMotion_UseOut", rclcpp::QoS(10), std::bind(&MotionMaster_Node::DriveMotionOut_Callback, this, std::placeholders::_1));
    this->DriveMotionOutSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("KiwiMotion_UseOut", rclcpp::QoS(10), std::bind(&MotionMaster_Node::KiwiMotionOut_Callback, this, std::placeholders::_1));

    this->MotionMaster_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("MotionMasterOut", rclcpp::QoS(10));
    this->timer_ = this->create_wall_timer(1ms, std::bind(&MotionMaster_Node::timer_callback, this));
}

MotionMaster_Node::~MotionMaster_Node()
{
}

void MotionMaster_Node::KiwiMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr use_msg)
{
    if (use_msg->data[0] == 1)
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
}

void MotionMaster_Node::WallMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr use_msg)
{
    if (use_msg->data[0] == 1)
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
}

void MotionMaster_Node::RopeMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr use_msg)
{
    if (use_msg->data[0] == 1)
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
}

void MotionMaster_Node::DriveMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr use_msg)
{
    if (use_msg->data[0] == 1)
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
}

void MotionMaster_Node::timer_callback(void)
{
    
    std_msgs::msg::ByteMultiArray pub_msg;
    pub_msg.data.resize(1);

    pub_msg.data[0]=motion_flag;
    // RCLCPP_INFO(this->get_logger(), "MotionMaster Code%d" ,this->motion_flag);
    this->MotionMaster_publisher->publish(pub_msg);
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotionMaster_Node>());
    rclcpp::shutdown();
    return 0;
}
