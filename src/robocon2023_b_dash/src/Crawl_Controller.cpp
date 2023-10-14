#include "rclcpp/rclcpp.hpp"
#include "Crawl_Controller.hpp"

Crawl_Controller_Node::Crawl_Controller_Node() : rclcpp::Node("Crawl_Controller")
{
    this->motion_flag = 0;

    this->Crawl_Controll_msg[0].type[0] = 23;
    this->Crawl_Controll_msg[0].type[1] = 180;
    this->Crawl_Controll_msg[0].ID = 0;
    this->Crawl_Controll_msg[0].cmd = 0;
    this->Crawl_Controll_msg[0].M = 0;

    this->Crawl_Controll_msg[1].type[0] = 23;
    this->Crawl_Controll_msg[1].type[1] = 180;
    this->Crawl_Controll_msg[1].ID = 1;
    this->Crawl_Controll_msg[1].cmd = 0;
    this->Crawl_Controll_msg[1].M = 0;

    this->Crawl_Controll_msg[2].type[0] = 23;
    this->Crawl_Controll_msg[2].type[1] = 180;
    this->Crawl_Controll_msg[2].ID = 2;
    this->Crawl_Controll_msg[2].cmd = 0;
    this->Crawl_Controll_msg[2].M = 0;

    this->Crawl_Controll_msg[3].type[0] = 23;
    this->Crawl_Controll_msg[3].type[1] = 180;
    this->Crawl_Controll_msg[3].ID = 3;
    this->Crawl_Controll_msg[3].cmd = 0;
    this->Crawl_Controll_msg[3].M = 0;

    this->WallMotionSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("WallMotion_CrawlOut", rclcpp::QoS(10), std::bind(&Crawl_Controller_Node::WallMotionOut_Callback, this, std::placeholders::_1));
    this->RopeMotionOutSubscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("RopeMotion_CrawlOut", rclcpp::QoS(10), std::bind(&Crawl_Controller_Node::RopeMotionOut_Callback, this, std::placeholders::_1));

    this->Crawl_Controller_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("CrawlOut", rclcpp::QoS(10));
    this->timer_ = this->create_wall_timer(1ms, std::bind(&Crawl_Controller_Node::timer_callback, this));
}

Crawl_Controller_Node::~Crawl_Controller_Node()
{
}

void Crawl_Controller_Node::WallMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (md_msg->data[32] == 1)
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
        this->WallMotion_msg[2].bin[i] = md_msg->data[i + 16];
        this->WallMotion_msg[3].bin[i] = md_msg->data[i + 24];
    }
    // for (int i = 0; i < 4; i++)
    // {
    //     RCLCPP_INFO(this->get_logger(), " %dtype1,type2,id,cmd,m   %d,%d,%d,%d,%f  ", i,
    //                 this->WallMotion_msg[i].type[0], this->WallMotion_msg[i].type[1], this->WallMotion_msg[i].ID, this->WallMotion_msg[i].cmd, this->WallMotion_msg[i].M);
    // }
}

void Crawl_Controller_Node::RopeMotionOut_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr md_msg)
{
    if (md_msg->data[32] == 1)
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
        this->RopeMotion_msg[2].bin[i] = md_msg->data[i + 16];
        this->RopeMotion_msg[3].bin[i] = md_msg->data[i + 24];
    }
}

void Crawl_Controller_Node::timer_callback(void)
{
    if (this->motion_flag == 3)
    {
        this->Crawl_Controll_msg[0] = this->WallMotion_msg[0];
        this->Crawl_Controll_msg[1] = this->WallMotion_msg[1];
        this->Crawl_Controll_msg[2] = this->WallMotion_msg[2];
        this->Crawl_Controll_msg[3] = this->WallMotion_msg[3];
    }
    else if (this->motion_flag == 2)
    {
        this->Crawl_Controll_msg[0] = this->RopeMotion_msg[0];
        this->Crawl_Controll_msg[1] = this->RopeMotion_msg[1];
        this->Crawl_Controll_msg[2] = this->RopeMotion_msg[2];
        this->Crawl_Controll_msg[3] = this->RopeMotion_msg[3];
    }

    // for (int i = 0; i < 4; i++)
    // {
    //     RCLCPP_INFO(this->get_logger(), " %dtype1,type2,id,cmd,m   %d,%d,%d,%d,%f  ", i,
    //                 this->WallMotion_msg[i].type[0], this->WallMotion_msg[i].type[1], this->WallMotion_msg[i].ID, this->WallMotion_msg[i].cmd, this->WallMotion_msg[i].M);
    // }
    // RCLCPP_INFO(this->get_logger(), "motion falg %d", this->motion_flag);
    // for (int i = 0; i < 4; i++)
    // {
    //     RCLCPP_INFO(this->get_logger(), " %dtype1,type2,id,cmd,m   %d,%d,%d,%d,%f  ", i,
    //                 this->Crawl_Controll_msg[i].type[0], this->Crawl_Controll_msg[i].type[1], this->Crawl_Controll_msg[i].ID, this->Crawl_Controll_msg[i].cmd, this->Crawl_Controll_msg[i].M);
    // }

    // else if (this->motion_flag == 0)
    // {
    //     this->Crawl_Controll_msg[0].cmd = 0;
    //     this->Crawl_Controll_msg[0].M = 0;
    //     this->Crawl_Controll_msg[1].cmd = 0;
    //     this->Crawl_Controll_msg[1].M = 0;
    //     this->Crawl_Controll_msg[2].cmd = 0;
    //     this->Crawl_Controll_msg[2].M = 0;
    //     this->Crawl_Controll_msg[3].cmd = 0;
    //     this->Crawl_Controll_msg[3].M = 0;
    // }

    std_msgs::msg::ByteMultiArray pub_msg;
    pub_msg.data.resize(32);
    for (int i = 0; i < 8; i++)
    {
        pub_msg.data[i] = this->Crawl_Controll_msg[0].bin[i];
        pub_msg.data[i + 8] = this->Crawl_Controll_msg[1].bin[i];
        pub_msg.data[i + 16] = this->Crawl_Controll_msg[2].bin[i];
        pub_msg.data[i + 24] = this->Crawl_Controll_msg[3].bin[i];
    }
    this->Crawl_Controller_publisher->publish(pub_msg);
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Crawl_Controller_Node>());
    rclcpp::shutdown();
    return 0;
}