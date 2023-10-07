#include "rclcpp/rclcpp.hpp"
#include "Upper_Controller.hpp"

Upper_Controller_Node::Upper_Controller_Node() : rclcpp::Node("Upper_Controller")
{
    this->option = 0;
    this->share = 0;

    this->button_state = 0;
    this->up_flag = 0;

    this->upper_msg.type[0] = 23;
    this->upper_msg.type[1] = 180;
    this->upper_msg.ID = 7;
    this->upper_msg.cmd = 5;
    this->upper_msg.M = 250;

    this->JoySubscription = this->create_subscription<sensor_msgs::msg::Joy>("joy", rclcpp::QoS(10), std::bind(&Upper_Controller_Node::Joy_Callback, this, std::placeholders::_1));

    this->Upper_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("UpperOut", rclcpp::QoS(10));
    this->timer_ = this->create_wall_timer(1ms, std::bind(&Upper_Controller_Node::timer_callback, this));
}

Upper_Controller_Node::~Upper_Controller_Node()
{
}

void Upper_Controller_Node::Joy_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
{
    this->option = joy_msg->buttons[9]; // option
    this->share = joy_msg->buttons[8];  // share

    if (joy_msg->axes[6] == 1) // left
    {
        this->button_state |= 0b0001 << 0;
    }
    else
    {
        this->button_state &= ~(0b0001 << 0);
    }

    if (joy_msg->axes[6] == -1) // right
    {
        this->button_state |= 0b0001 << 1;
    }
    else
    {
        this->button_state &= ~(0b0001 << 1);
    }

    if (joy_msg->axes[7] == 1) // up
    {
        this->button_state |= 0b0001 << 2;
    }
    else
    {
        this->button_state &= ~(0b0001 << 2);
    }

    if (joy_msg->axes[7] == -1) // down
    {
        this->button_state |= 0b0001 << 3;
    }
    else
    {
        this->button_state &= ~(0b0001 << 3);
    }
}

void Upper_Controller_Node::timer_callback(void)
{
    if (this->upper_msg.M > 64)
    {
        this->upper_msg.M -= 0.05 * this->option;
    }
    if (this->upper_msg.M < 300)
    {
        this->upper_msg.M += 0.05 * this->share;
    }

    if ((this->button_state >> 0) & 0b001 == 1) // left
    {
        this->upper_msg.M = 194; // blueberry
        this->up_flag = 0;
    }

    else if ((this->button_state >> 1) & 0b0001 == 1) // right
    {
        this->upper_msg.M = 118; // grape
        this->up_flag = 0;
    }
    else if ((this->button_state >> 2) & 0b0001 == 1) // up
    {
        this->upper_msg.M = 95; // mix
        this->up_flag = 0;
    }
    else if ((this->button_state >> 3) & 0b0001 == 1) // down
    {
        if (this->up_flag == 0)
        {
            this->upper_msg.M -= 20;
            this->up_flag = 1;
        }
    }

    RCLCPP_INFO(this->get_logger(), "Upper value %f  ", this->upper_msg.M);
    // RCLCPP_INFO(this->get_logger(), "button value %d  ", this->button_state >> 3 & 0b001);
    std_msgs::msg::ByteMultiArray pub_msg;
    pub_msg.data.resize(8);
    for (int i = 0; i < 8; i++)
    {
        pub_msg.data[i] = this->upper_msg.bin[i];
    }
    this->Upper_publisher->publish(pub_msg);
}

int main(int argc, char *argv[])
{

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Upper_Controller_Node>());
    rclcpp::shutdown();
    return 0;
}