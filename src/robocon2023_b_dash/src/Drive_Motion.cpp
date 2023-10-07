#include "rclcpp/rclcpp.hpp"
#include "Drive_Motion.hpp"

DriveMotion_Node::DriveMotion_Node() : rclcpp::Node("Drive_Motion")
{
    this->ly = 0;
    this->ry = 0;

    this->DriveMotion_msg[0].type[0] = 23;
    this->DriveMotion_msg[0].type[1] = 180;
    this->DriveMotion_msg[0].ID = 10;
    this->DriveMotion_msg[0].cmd = 0;
    this->DriveMotion_msg[0].M = 0;

    this->DriveMotion_msg[1].type[0] = 23;
    this->DriveMotion_msg[1].type[1] = 180;
    this->DriveMotion_msg[1].ID = 11;
    this->DriveMotion_msg[1].cmd = 0;
    this->DriveMotion_msg[1].M = 0;

    this->JoySubscription = this->create_subscription<sensor_msgs::msg::Joy>("joy", rclcpp::QoS(10), std::bind(&DriveMotion_Node::Joy_Callback, this, std::placeholders::_1));
    this->DriveMotion_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("DriveMotionOut", rclcpp::QoS(10));
    this->timer_ = this->create_wall_timer(10ms, std::bind(&DriveMotion_Node::timer_callback, this));
}

DriveMotion_Node::~DriveMotion_Node()
{
}

void DriveMotion_Node::Joy_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
{
    this->ly = (float)(joy_msg->axes[1]);
    this->ry = (float)(joy_msg->axes[4]);
}

void DriveMotion_Node::timer_callback(void)
{
    this->DriveMotion_msg[0].M = this->ly * 100.0;
    this->DriveMotion_msg[1].M = this->ry * -100.0;
    std_msgs::msg::ByteMultiArray pub_msg;
    pub_msg.data.resize(16 + 1);
    for (int i = 0; i < 8; i++)
    {
        pub_msg.data[i] = this->DriveMotion_msg[0].bin[i];
        pub_msg.data[i + 8] = this->DriveMotion_msg[1].bin[i];
    }
    pub_msg.data[16] = 1; // always use
    this->DriveMotion_publisher->publish(pub_msg);
}

int main(int argc, char *argv[])
{

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DriveMotion_Node>());
    rclcpp::shutdown();
    return 0;
}