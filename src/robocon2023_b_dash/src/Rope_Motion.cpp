#include "rclcpp/rclcpp.hpp"
#include "Rope_Motion.hpp"

#define CRAWL1_ZERO 249.433594
#define CRAWL2_ZERO 147.568359
#define CRAWL3_ZERO 290.039062
#define CRAWL4_ZERO 245.039062

#define FAST_MODE 200
#define SLOW_MODE 203

RopeMotion_Node::RopeMotion_Node() : rclcpp::Node("Rope_Motion")
{
    this->ly = 0;
    this->ry = 0;

    this->circle = 0;
    this->triangle = 0;
    this->square = 0;
    this->cross = 0;

    this->belt_motion_flag = 0;
    this->crawl_motion_flag = 0;
    this->drive_motion_flag = 0;

    this->belt_msg[0].type[0] = 23;
    this->belt_msg[0].type[1] = 180;
    this->belt_msg[0].ID = 4;
    this->belt_msg[0].cmd = 0;
    this->belt_msg[0].M = 0;

    this->belt_msg[1].type[0] = 23;
    this->belt_msg[1].type[1] = 180;
    this->belt_msg[1].ID = 5;
    this->belt_msg[1].cmd = 0;
    this->belt_msg[1].M = 0;

    this->crawl_msg[0].type[0] = 23;
    this->crawl_msg[0].type[1] = 180;
    this->crawl_msg[0].ID = 0;
    this->crawl_msg[0].cmd = 200;
    this->crawl_msg[0].M = CRAWL1_ZERO;

    this->crawl_msg[1].type[0] = 23;
    this->crawl_msg[1].type[1] = 180;
    this->crawl_msg[1].ID = 1;
    this->crawl_msg[1].cmd = 200;
    this->crawl_msg[1].M = CRAWL2_ZERO;

    this->crawl_msg[2].type[0] = 23;
    this->crawl_msg[2].type[1] = 180;
    this->crawl_msg[2].ID = 2;
    this->crawl_msg[2].cmd = 200;
    this->crawl_msg[2].M = CRAWL3_ZERO;

    this->crawl_msg[3].type[0] = 23;
    this->crawl_msg[3].type[1] = 180;
    this->crawl_msg[3].ID = 3;
    this->crawl_msg[3].cmd = 200;
    this->crawl_msg[3].M = CRAWL4_ZERO;

    this->drive_msg[0].type[0] = 23;
    this->drive_msg[0].type[1] = 180;
    this->drive_msg[0].ID = 10;
    this->drive_msg[0].cmd = 0;
    this->drive_msg[0].M = 0;

    this->drive_msg[1].type[0] = 23;
    this->drive_msg[1].type[1] = 180;
    this->drive_msg[1].ID = 11;
    this->drive_msg[1].cmd = 0;
    this->drive_msg[1].M = 0;

    this->JoySubscription = this->create_subscription<sensor_msgs::msg::Joy>("joy", rclcpp::QoS(10), std::bind(&RopeMotion_Node::Joy_Callback, this, std::placeholders::_1));

    this->belt_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("RopeMotion_BeltOut", rclcpp::QoS(10));
    this->crawl_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("RopeMotion_CrawlOut", rclcpp::QoS(10));
    this->drive_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("RopeMotion_DriveOut", rclcpp::QoS(10));
    this->solenoid_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("RopeMotion_SolenoidOut", rclcpp::QoS(10));

    this->timer_ = this->create_wall_timer(1ms, std::bind(&RopeMotion_Node::timer_callback, this));

    this->crawl_motion_flag = 0;
    this->belt_motion_flag = 0;
    this->drive_motion_flag = 0;
    this->PublishMDmsg();
    // thisl
    // this->crawl_motion_flag = 0;
    // this->PublishMDmsg();
}

RopeMotion_Node::~RopeMotion_Node()
{
}

void RopeMotion_Node::Joy_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
{
    this->ly = (float)(joy_msg->axes[1]);
    this->ry = (float)(joy_msg->axes[4]);

    this->cross = joy_msg->buttons[0];
    this->circle = joy_msg->buttons[1];
    this->triangle = joy_msg->buttons[2];
    this->square = joy_msg->buttons[3];
    this->ps = joy_msg->buttons[10];
}

void RopeMotion_Node::PublishMDmsg(void)
{

    std_msgs::msg::ByteMultiArray belt_pub_msg;
    belt_pub_msg.data.resize(8 * 2 + 1);
    for (int i = 0; i < 8; i++)
    {
        belt_pub_msg.data[i] = this->belt_msg[0].bin[i];
        belt_pub_msg.data[i + 8] = this->belt_msg[1].bin[i];
    }
    belt_pub_msg.data[16] = this->belt_motion_flag;
    this->belt_publisher->publish(belt_pub_msg);

    std_msgs::msg::ByteMultiArray crawl_pub_msg;
    crawl_pub_msg.data.resize(8 * 4 + 1);
    for (int i = 0; i < 8; i++)
    {
        crawl_pub_msg.data[i] = this->crawl_msg[0].bin[i];
        crawl_pub_msg.data[i + 8] = this->crawl_msg[1].bin[i];
        crawl_pub_msg.data[i + 16] = this->crawl_msg[2].bin[i];
        crawl_pub_msg.data[i + 24] = this->crawl_msg[3].bin[i];
    }

    crawl_pub_msg.data[32] = this->crawl_motion_flag;
    this->crawl_publisher->publish(crawl_pub_msg);

    std_msgs::msg::ByteMultiArray drive_pub_msg;
    drive_pub_msg.data.resize(8 * 2 + 1);
    for (int i = 0; i < 8; i++)
    {
        drive_pub_msg.data[i] = this->drive_msg[0].bin[i];
        drive_pub_msg.data[i + 8] = this->drive_msg[1].bin[i];
    }
    drive_pub_msg.data[16] = this->drive_motion_flag;
    this->drive_publisher->publish(drive_pub_msg);

    std_msgs::msg::ByteMultiArray solenoid_pub_msg;
    solenoid_pub_msg.data.resize(8 * 1 + 1);
    for (int i = 0; i < 8; i++)
    {
        solenoid_pub_msg.data[i] = this->solenoid_msg.bin[i];
    }
    solenoid_pub_msg.data[8] = this->solenoid_motion_flag;
    this->solenoid_publisher->publish(solenoid_pub_msg);
}

void RopeMotion_Node::ToggleClawlCmd(uint8_t crawl_num, uint8_t mode)
{
    if (mode == FAST_MODE)
    {
        // fast
        if (this->crawl_msg[crawl_num].cmd == 201)
        {
            this->crawl_msg[crawl_num].cmd = 202;
        }
        else
        {
            this->crawl_msg[crawl_num].cmd = 201;
        }
    }

    if (mode == SLOW_MODE)
    {
        // slow
        if (this->crawl_msg[crawl_num].cmd == 204)
        {
            this->crawl_msg[crawl_num].cmd = 205;
        }
        else
        {
            this->crawl_msg[crawl_num].cmd = 204;
        }
    }
}

void RopeMotion_Node::InitPos_Clawl(uint8_t crawl_num, uint8_t mode)
{
    this->crawl_msg[crawl_num].type[0] = 23;
    this->crawl_msg[crawl_num].type[1] = 180;
    this->crawl_msg[crawl_num].ID = crawl_num;
    this->crawl_msg[crawl_num].cmd = mode;
    this->crawl_msg[crawl_num].M = this->GetClawlZEROPos(crawl_num);
}

void RopeMotion_Node::SetClawlIncPos(uint8_t crawl_num, uint8_t mode, float value)
{
    this->crawl_msg[crawl_num].type[0] = 23;
    this->crawl_msg[crawl_num].type[1] = 180;
    this->crawl_msg[crawl_num].ID = crawl_num;
    this->ToggleClawlCmd(crawl_num, mode);
    this->crawl_msg[crawl_num].M = value;
}

void RopeMotion_Node::SetClawlAbsPos(uint8_t crawl_num, uint8_t mode, float value)
{
    if (mode == FAST_MODE)
    {
        this->crawl_msg[crawl_num].type[0] = 23;
        this->crawl_msg[crawl_num].type[1] = 180;
        this->crawl_msg[crawl_num].ID = crawl_num;
        this->crawl_msg[crawl_num].cmd = 5;
        this->crawl_msg[crawl_num].M = value + this->GetClawlZEROPos(crawl_num);
    }
    else if (mode == SLOW_MODE)
    {
        this->crawl_msg[crawl_num].type[0] = 23;
        this->crawl_msg[crawl_num].type[1] = 180;
        this->crawl_msg[crawl_num].ID = crawl_num;
        this->crawl_msg[crawl_num].cmd = 7;
        this->crawl_msg[crawl_num].M = value + this->GetClawlZEROPos(crawl_num);
    }
}

float RopeMotion_Node::GetClawlNowM(uint8_t crawl_num)
{
    return this->crawl_msg[crawl_num].M;
}

float RopeMotion_Node::GetClawlZEROPos(uint8_t crawl_num)
{
    if (crawl_num == 0)
        return CRAWL1_ZERO;
    if (crawl_num == 1)
        return CRAWL2_ZERO;
    if (crawl_num == 2)
        return CRAWL3_ZERO;
    if (crawl_num == 3)
        return CRAWL4_ZERO;

    return 0; // error
}

void RopeMotion_Node::ToggleSlolenoidCmd(void)
{
    if (this->solenoid_msg.cmd == 1)
    {
        this->solenoid_msg.cmd = 0;
    }
    else
    {
        this->solenoid_msg.cmd = 1;
    }
}

bool RopeMotion_Node::CheckLessTime(double now_time, double *old_time, double dt)
{

    if ((now_time - *old_time) > dt)
    {
        *old_time = now_time;
        return true;
    }
    else
    {
        return false;
    }
}

bool RopeMotion_Node::CircleOnCheck(double dt)
{
    double now_time = RopeMotion_Node::now().seconds();
    static double old_time = 0;
    static uint8_t on_flag = 0;

    if (this->circle == 1)
    {
        if (this->CheckLessTime(now_time, &old_time, dt) && (on_flag == 0))
        {
            on_flag = 1;
            return true;
        }
    }
    else
    {
        on_flag = 0;
    }

    return false;
}
bool RopeMotion_Node::TriangleOnChek(double dt)
{
    double now_time = RopeMotion_Node::now().seconds();
    static double old_time = 0;
    static uint8_t on_flag = 0;

    if (this->triangle == 1)
    {
        if (this->CheckLessTime(now_time, &old_time, dt) && (on_flag == 0))
        {
            on_flag = 1;
            return true;
        }
    }
    else
    {
        on_flag = 0;
    }

    return false;
}
bool RopeMotion_Node::SquareOnChek(double dt)
{
    double now_time = RopeMotion_Node::now().seconds();
    static double old_time = 0;
    static uint8_t on_flag = 0;

    if (this->square == 1)
    {
        if (this->CheckLessTime(now_time, &old_time, dt) && (on_flag == 0))
        {
            on_flag = 1;
            return true;
        }
    }
    else
    {
        on_flag = 0;
    }

    return false;
}
bool RopeMotion_Node::CrossOnChek(double dt)
{
    double now_time = RopeMotion_Node::now().seconds();
    static double old_time = 0;
    static uint8_t on_flag = 0;

    if (this->cross == 1)
    {
        if (this->CheckLessTime(now_time, &old_time, dt) && (on_flag == 0))
        {
            on_flag = 1;
            return true;
        }
    }
    else
    {
        on_flag = 0;
    }

    return false;
}

bool RopeMotion_Node::PsOnChek(double dt)
{
    double now_time = RopeMotion_Node::now().seconds();
    static double old_time = 0;
    static uint8_t on_flag = 0;

    if (this->ps == 1)
    {
        if (this->CheckLessTime(now_time, &old_time, dt) && (on_flag == 0))
        {
            on_flag = 1;
            return true;
        }
    }
    else
    {
        on_flag = 0;
    }

    return false;
}

void RopeMotion_Node::Rope_Task()
{
    static int motion_count = -1;
    if (this->CircleOnCheck(0.01))
    {
        motion_count++;
        if (motion_count > 12)
            motion_count = 0;

        switch (motion_count)
        {
        case 0://init
            this->crawl_motion_flag = 1;
            this->InitPos_Clawl(0, SLOW_MODE);
            this->InitPos_Clawl(1, SLOW_MODE);
            this->InitPos_Clawl(2, SLOW_MODE);
            this->InitPos_Clawl(3, SLOW_MODE);
            break;
        case 1://front stundup
            this->crawl_motion_flag = 1;

            this->SetClawlIncPos(0, SLOW_MODE, -106);
            this->SetClawlIncPos(3, SLOW_MODE, +106);
            break;
        case 2://back stundup too
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(0, SLOW_MODE, +16);
            this->SetClawlIncPos(1, SLOW_MODE, +90);
            this->SetClawlIncPos(2, SLOW_MODE, -90);
            this->SetClawlIncPos(3, SLOW_MODE, -16);
            break;
        case 3://crawl left front
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(0, FAST_MODE, -180);
            break;
        case 4://crawl right front
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(3, FAST_MODE, 180);
            break;
        case 5://down back
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(0, SLOW_MODE, -16);
            this->SetClawlIncPos(1, SLOW_MODE, -90);
            this->SetClawlIncPos(2, SLOW_MODE, +90);
            this->SetClawlIncPos(3, SLOW_MODE, +16);
            break;
        case 6://back staundup too
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(0, SLOW_MODE, +16);
            this->SetClawlIncPos(1, SLOW_MODE, +90);
            this->SetClawlIncPos(2, SLOW_MODE, -90);
            this->SetClawlIncPos(3, SLOW_MODE, -16);
            break;
        case 7://down front
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(0, SLOW_MODE, +90);
            this->SetClawlIncPos(1, SLOW_MODE, -16);
            this->SetClawlIncPos(2, SLOW_MODE, +16);
            this->SetClawlIncPos(3, SLOW_MODE, -90);
            break;
        case 8://front stundup too
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(0, SLOW_MODE, -90);
            this->SetClawlIncPos(1, SLOW_MODE, +16);
            this->SetClawlIncPos(2, SLOW_MODE, -16);
            this->SetClawlIncPos(3, SLOW_MODE, +90);
            break;
        case 9://crawl left back
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(1, FAST_MODE, -180);
            break;
        case 10://crawl right back
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(2, FAST_MODE, +180);
            break;
        case 11://down front
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(0, SLOW_MODE, +90);
            this->SetClawlIncPos(1, SLOW_MODE, -16);
            this->SetClawlIncPos(2, SLOW_MODE, +16);
            this->SetClawlIncPos(3, SLOW_MODE, -90);
            break;
        case 12://reset init pos
            this->crawl_motion_flag = 1;
            this->SetClawlIncPos(1, SLOW_MODE, -74);
            this->SetClawlIncPos(2, SLOW_MODE, +74);
            break;
        default:
            this->crawl_motion_flag = 0;
            break;
        }
    }

    // RCLCPP_INFO(this->get_logger(), "motion coutn%d", motion_count);
    //     this->crawl_motion_flag = 1;
    //     this->crawl_msg[0].type[0] = 23;
    //     this->crawl_msg[0].type[1] = 180;
    //     this->crawl_msg[0].ID = 0;
    //     this->ToggleClawlCmd(0);
    //     this->crawl_msg[0].M = -10;
    // }
    // if (this->TriangleOnChek(0.01))
    // {
    //     this->crawl_motion_flag = 1;
    //     this->crawl_msg[1].type[0] = 23;
    //     this->crawl_msg[1].type[1] = 180;
    //     this->crawl_msg[1].ID = 1;
    //     this->ToggleClawlCmd(1);
    //     this->crawl_msg[1].M = -10;
    // }
    // if (this->SquareOnChek(0.01))
    // {
    //     this->crawl_motion_flag = 1;
    //     this->crawl_msg[2].type[0] = 23;
    //     this->crawl_msg[2].type[1] = 180;
    //     this->crawl_msg[2].ID = 2;
    //     this->ToggleClawlCmd(2);
    //     this->crawl_msg[2].M = 10;
    // }
    // if (this->CrossOnChek(0.01))
    // {
    //     this->crawl_motion_flag = 1;
    //     this->crawl_msg[3].type[0] = 23;
    //     this->crawl_msg[3].type[1] = 180;
    //     this->crawl_msg[3].ID = 3;
    //     this->ToggleClawlCmd(3);
    //     this->crawl_msg[3].M = 10;
    // }
    if (this->PsOnChek(0.01))
    {
        this->solenoid_motion_flag = 1;
        this->solenoid_msg.type[0] = 23;
        this->solenoid_msg.type[1] = 151;
        this->ToggleSlolenoidCmd();
    }

    this->drive_motion_flag = 1;
    this->drive_msg[0].M = this->ly * -100.0;
    this->drive_msg[1].M = this->ry * 100.0;

    this->belt_motion_flag = 1;
    this->belt_msg[0].M = this->ly * 100.0;
    this->belt_msg[1].M = this->ry * -100.0;
}

void RopeMotion_Node::timer_callback(void)
{
    this->Rope_Task();

    this->PublishMDmsg();
}

int main(int argc, char *argv[])
{

    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<RopeMotion_Node>());

    // auto node = std::make_shared<WallUpMotion_Node>();
    // rclcpp::spin_some(node);

    // for (int i = 0; i < 100; i++)
    // {
    //     rclcpp::sleep_for(10ms);
    //     node->PublishMDmsg();
    // }
    // while (rclcpp::ok())
    // {
    //     rclcpp::spin_some(node);
    //     node->Wallup_Task();
    //     rclcpp::sleep_for(200ms);
    // }

    rclcpp::shutdown();
    return 0;
}
