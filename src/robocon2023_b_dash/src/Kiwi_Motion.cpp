#include "rclcpp/rclcpp.hpp"
#include "Kiwi_Motion.hpp"

#define CRAWL1_ZERO 72.1582031
#define CRAWL2_ZERO 154.511719
#define CRAWL3_ZERO 83.4960938
#define CRAWL4_ZERO 31.0253906

#define FAST_MODE 200
#define SLOW_MODE 203
#define RESET_MODE201 201
#define RESET_MODE202 202

Kiwi_Motion_Node::Kiwi_Motion_Node() : rclcpp::Node("Kiwi_Motion")
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

    this->solenoid_msg.type[0] = 23;
    this->solenoid_msg.type[1] = 151;

    this->JoySubscription = this->create_subscription<sensor_msgs::msg::Joy>("joy", rclcpp::QoS(10), std::bind(&Kiwi_Motion_Node::Joy_Callback, this, std::placeholders::_1));

    this->belt_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("KiwiMotion_BeltOut", rclcpp::QoS(10));
    this->crawl_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("KiwiMotion_CrawlOut", rclcpp::QoS(10));
    this->drive_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("KiwiMotion_DriveOut", rclcpp::QoS(10));
    this->solenoid_publisher = this->create_publisher<std_msgs::msg::ByteMultiArray>("KiwiMotion_SolenoidOut", rclcpp::QoS(10));

    this->used_publiser = this->create_publisher<std_msgs::msg::ByteMultiArray>("KiwiMotion_UseOut", rclcpp::QoS(10));
    this->used_subscription = this->create_subscription<std_msgs::msg::ByteMultiArray>("MotionMasterOut", rclcpp::QoS(10), std::bind(&Kiwi_Motion_Node::MotionMaster_Code_Callback, this, std::placeholders::_1));

    this->timer_ = this->create_wall_timer(1ms, std::bind(&Kiwi_Motion_Node::timer_callback, this));

    this->crawl_motion_flag = 0;
    this->belt_motion_flag = 0;
    this->drive_motion_flag = 0;
    this->solenoid_motion_flag = 0;
    this->PublishMDmsg();
}

Kiwi_Motion_Node::~Kiwi_Motion_Node()
{
}

void Kiwi_Motion_Node::Joy_Callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg)
{
    this->ly = (float)(joy_msg->axes[1]);
    this->ry = (float)(joy_msg->axes[4]);

    this->cross = joy_msg->buttons[0];
    this->circle = joy_msg->buttons[1];
    this->triangle = joy_msg->buttons[2];
    this->square = joy_msg->buttons[3];
    this->ps = joy_msg->buttons[10];
}

void Kiwi_Motion_Node::MotionMaster_Code_Callback(const std_msgs::msg::ByteMultiArray::SharedPtr use_msg)
{
    this->motion_master_code = use_msg->data[0];
}

void Kiwi_Motion_Node::PublishMDmsg(void)
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

    std_msgs::msg::ByteMultiArray used_pub_msg;
    used_pub_msg.data.resize(1);
    used_pub_msg.data[0] = this->motion_start_flag;
    this->used_publiser->publish(used_pub_msg);
}

void Kiwi_Motion_Node::ToggleClawlCmd(uint8_t crawl_num, uint8_t mode)
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

    else if (mode == SLOW_MODE)
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
    else if (mode == RESET_MODE201)
    {
        this->crawl_msg[crawl_num].cmd = 201;
    }
    else if (mode == RESET_MODE202)
    {
        this->crawl_msg[crawl_num].cmd = 202;
    }
}

void Kiwi_Motion_Node::InitPos_Clawl(uint8_t crawl_num, uint8_t mode)
{
    this->crawl_msg[crawl_num].type[0] = 23;
    this->crawl_msg[crawl_num].type[1] = 180;
    this->crawl_msg[crawl_num].ID = crawl_num;
    this->crawl_msg[crawl_num].cmd = mode;
    this->crawl_msg[crawl_num].M = this->GetClawlZEROPos(crawl_num);
}

void Kiwi_Motion_Node::SetClawlIncPos(uint8_t crawl_num, uint8_t mode, float value)
{
    this->crawl_msg[crawl_num].type[0] = 23;
    this->crawl_msg[crawl_num].type[1] = 180;
    this->crawl_msg[crawl_num].ID = crawl_num;
    this->ToggleClawlCmd(crawl_num, mode);
    this->crawl_msg[crawl_num].M = value;
}

void Kiwi_Motion_Node::SetClawlAbsPos(uint8_t crawl_num, uint8_t mode, float value)
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
float Kiwi_Motion_Node::GetClawlNowM(uint8_t crawl_num)
{
    return this->crawl_msg[crawl_num].M;
}

float Kiwi_Motion_Node::GetClawlZEROPos(uint8_t crawl_num)
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

void Kiwi_Motion_Node::DriveSpeedControl(uint8_t drive_num, float speed)
{
    this->drive_msg[drive_num].cmd = 4;
    this->drive_msg[drive_num].M = speed;
}

void Kiwi_Motion_Node::DriveDutyControl(uint8_t drive_num, float duty)
{
    this->drive_msg[drive_num].cmd = 0;
    this->drive_msg[drive_num].M = duty;
}

void Kiwi_Motion_Node::BeltSpeedControl(uint8_t belt_num, float speed)
{
    this->belt_msg[belt_num].cmd = 4;
    this->belt_msg[belt_num].M = speed;
}

void Kiwi_Motion_Node::BeltDutyControl(uint8_t belt_num, float duty)
{
    this->belt_msg[belt_num].cmd = 0;
    this->belt_msg[belt_num].M = duty;
}

void Kiwi_Motion_Node::BetlUse(uint8_t belt_num)
{
    this->belt_used[belt_num] = 1;
}

void Kiwi_Motion_Node::BeltFree(uint8_t belt_num)
{
    this->belt_used[belt_num] = 0;
}

uint8_t Kiwi_Motion_Node::BeltGetUsed(uint8_t belt_num)
{
    return this->belt_used[belt_num];
}

void Kiwi_Motion_Node::ToggleSlolenoidCmd(void)
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

bool Kiwi_Motion_Node::CheckLessTime(double now_time, double *old_time, double dt)
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

void Kiwi_Motion_Node::ResetOldTIme(double now_time, double *old_time)
{
    *old_time = now_time;
}

bool Kiwi_Motion_Node::CircleOnCheck(double dt)
{
    double now_time = Kiwi_Motion_Node::now().seconds();
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
bool Kiwi_Motion_Node::TriangleOnChek(double dt)
{
    double now_time = Kiwi_Motion_Node::now().seconds();
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
bool Kiwi_Motion_Node::SquareOnChek(double dt)
{
    double now_time = Kiwi_Motion_Node::now().seconds();
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
bool Kiwi_Motion_Node::CrossOnChek(double dt)
{
    double now_time = Kiwi_Motion_Node::now().seconds();
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

bool Kiwi_Motion_Node::PsOnChek(double dt)
{
    double now_time = Kiwi_Motion_Node::now().seconds();
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

void Kiwi_Motion_Node::Rope_Task()
{
    if ((this->motion_master_code == 0) || (this->motion_master_code == 4))
    {
        static int motion_count = 0;
        static int motion_do_count = 0;
        // static int this->motion_start_flag = 0;
        // RCLCPP_INFO(this->get_logger(), "count,do,start     %d,%d,%d", motion_count, motion_do_count, this->motion_start_flag);

        // time schedule
        if (this->motion_start_flag == 1)
        {
            double now_time = Kiwi_Motion_Node::now().seconds();
            static double old_time = 0;
            switch (motion_count)
            {
            case 0:
                break;
            case 1: // back stundup
                if (this->CircleOnCheck(0.01))
                {
                    motion_do_count = 0;
                    motion_count++;
                    this->ResetOldTIme(now_time, &old_time);
                }
                break;
            case 2: // back  down
                if (this->CheckLessTime(now_time, &old_time, 0.5))
                {
                    motion_do_count = 0;
                    motion_count++;
                }
                break;
            case 3: // reset cmd
                if (this->CheckLessTime(now_time, &old_time, 0.1))
                {
                    motion_do_count = 0;
                    motion_count++;
                }
                break;
            case 4: // reset cmd
                if (this->CheckLessTime(now_time, &old_time, 0.1))
                {
                    motion_do_count = 0;
                    motion_count = 0;
                    this->motion_start_flag = 0;
                }
                break;
            }
        }
        else if (this->CircleOnCheck(0.01))
        {

            motion_count = 1;
            this->motion_start_flag = 1;

            this->crawl_motion_flag = 1;
            this->drive_motion_flag = 1;
            this->belt_motion_flag = 1;
            this->solenoid_motion_flag = 1;
        }
        else
        {
            this->crawl_motion_flag = 0;
            this->drive_motion_flag = 0;
            this->belt_motion_flag = 0;
            this->solenoid_motion_flag = 0;
        }

        // motion
        if (motion_do_count == 0)
        {
            switch (motion_count)
            {
            case 1: // back stundup

                this->SetClawlIncPos(1, SLOW_MODE, +106);
                this->SetClawlIncPos(2, SLOW_MODE, -106);

                this->solenoid_msg.cmd = 1;

                motion_do_count = 1;
                break;
            case 2: // back down

                this->SetClawlIncPos(1, SLOW_MODE, -106);
                this->SetClawlIncPos(2, SLOW_MODE, +106);

                this->solenoid_msg.cmd = 0;

                motion_do_count = 1;
                break;
            case 3: // reset cmd
                this->SetClawlIncPos(0, RESET_MODE201, 0);
                this->SetClawlIncPos(1, RESET_MODE201, 0);
                this->SetClawlIncPos(2, RESET_MODE201, 0);
                this->SetClawlIncPos(3, RESET_MODE201, 0);
                motion_do_count = 1;
                break;
            case 4: // reset cmd
                this->SetClawlIncPos(0, RESET_MODE202, 0);
                this->SetClawlIncPos(1, RESET_MODE202, 0);
                this->SetClawlIncPos(2, RESET_MODE202, 0);
                this->SetClawlIncPos(3, RESET_MODE202, 0);
                motion_do_count = 1;
                break;
            }
        }

        this->drive_msg[0].M = this->ly * -100.0;
        this->drive_msg[1].M = this->ry * 100.0;


        this->belt_msg[0].M = this->ly * 100.0;
        this->belt_msg[1].M = this->ry * -100.0;
        // }
        // RCLCPP_INFO(this->get_logger(), "crawl,drive,belt,solenoid     %d,%d,%d,%d",
        //             this->crawl_motion_flag,
        //             this->drive_motion_flag,
        //             this->belt_motion_flag,
        //             this->solenoid_motion_flag);
    }
}

void Kiwi_Motion_Node::timer_callback(void)
{
    this->Rope_Task();

    this->PublishMDmsg();
}

int main(int argc, char *argv[])
{

    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<Kiwi_Motion_Node>());

    rclcpp::shutdown();
    return 0;
}
