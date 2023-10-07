#include "rclcpp/rclcpp.hpp"
#include "Xmove_Controller.hpp"

Xmove_Controller_Node::Xmove_Controller_Node() : rclcpp::Node("Xmove_Controller")
{

    this->l1 = 0;
    this->r1 = 0;

    this->xmove_msg.type[0] = 23;
    this->xmove_msg.type[1] = 180;
    this->xmove_msg.ID = 6;
    this->xmove_msg.cmd = 0;
    this->xmove_msg.M = 0;

    this->JoySubscription = this->create_subscription<sensor_msgs::msg::Joy>("joy", rclcpp::QoS(10), std::bind(&Xmove_Controller_Node::Joy_Callback, this, std::placeholders::_1));
    this->ImageRecognition_Subscription = this->create_subscription<std_msgs::msg::Int16MultiArray>(
        "image_recognition", rclcpp::QoS(10),
        std::bind(&Xmove_Controller_Node::ImageRecognition_Callback, this, std::placeholders::_1));

    this->Xmove_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("XmoveOut", rclcpp::QoS(10));
    this->timer_ = this->create_wall_timer(1ms, std::bind(&Xmove_Controller_Node::timer_callback, this));
}

Xmove_Controller_Node::~Xmove_Controller_Node()
{
}

void Xmove_Controller_Node::Joy_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
{
    // this->l1 = (float)(joy_msg->buttons[4]); // L1
    // this->r1 = (float)(joy_msg->buttons[5]); // R1
}

void Xmove_Controller_Node::ImageRecognition_Callback(const std_msgs::msg::Int16MultiArray::SharedPtr recognition_msg)
{
    int gain = 6;
    if(data[3] == 4){
        // blueberry
        gain = 8;
    }else if(data[3] == 5){
        // grape
        gain = 6;
    }else if(data[3] == 6){
        // mix
        gain = 4;
    }
    if (recognition_msg->data[1] > 0)
    {
        this->l1 = 0;
        this->r1 = (float)recognition_msg->data[1] / gain;
    }
    else
    {
        this->l1 = (float)recognition_msg->data[1] / gain * -1;
        this->r1 = 0;
    }
}

void Xmove_Controller_Node::timer_callback(void)
{
    this->xmove_msg.M = this->l1 * -10.0;
    this->xmove_msg.M += this->r1 * 10.0;
    std_msgs::msg::ByteMultiArray pub_msg;
    pub_msg.data.resize(8);
    for (int i = 0; i < 8; i++)
    {
        pub_msg.data[i] = this->xmove_msg.bin[i];
    }
    this->Xmove_publisher->publish(pub_msg);
}

int main(int argc, char *argv[])
{

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Xmove_Controller_Node>());
    rclcpp::shutdown();
    return 0;
}