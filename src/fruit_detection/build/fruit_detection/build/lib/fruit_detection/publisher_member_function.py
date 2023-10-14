# dousakakuninzumi 10/5

import rclpy
from rclpy.node import Node

# from std_msgs.msg import String
from std_msgs.msg import Int16MultiArray

state = 1
x = 2
y = 3
kind = 4


class MinimalPublisher(Node):
    def __init__(self):
        super().__init__('minimal_publisher')
        # self.publisher_ = self.create_publisher(String, 'topic', 10)
        self.publisher_ = self.create_publisher(Int16MultiArray, 'image_recognition', 10)
        timer_period = 0.0333  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        # msg = String()
        # msg.data = "[1.0, 0.23, 0.12, 3.0]"
        msg = Int16MultiArray()

        global state
        global x
        global y
        global kind
        x = x + 1
        print(state)
        print(x)
        print(y)
        print(kind)
        msg.data = [state, x, y, kind]
        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%s"' % msg.data)
        self.i += 1

    # def __init__(self):
    #     super().__init__('minimal_publisher')
    #     # self.publisher_ = self.create_publisher(String, 'topic', 10)
    #     self.publisher_ = self.create_publisher(Int16MultiArray, 'topic', 10)
    #     # timer_period = 0.0333  # seconds
    #     msg = Int16MultiArray()
    #     print(state)
    #     print(x)
    #     print(y)
    #     print(kind)
    #     msg.data = [state, x, y, kind]
    #     self.publisher_.publish(msg)
    #     self.get_logger().info('Publishing: "%s"' % msg.data)


def main(args=None):
    rclpy.init(args=args)

    minimal_publisher = MinimalPublisher()

    rclpy.spin(minimal_publisher)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    minimal_publisher.destroy_node()
    rclpy.shutdown()


def receive(state_tmp, x_tmp, y_tmp, kind_tmp):
    # print(state_tmp)
    # print(x_tmp)
    # print(y_tmp)
    # print(kind_tmp)
    global state
    global x
    global y
    global kind
    state = state_tmp
    x = x_tmp
    y = y_tmp
    kind = kind_tmp
    # print(state)
    # print(x)
    # print(y)
    # print(kind)


if __name__ == '__main__':
    main()
