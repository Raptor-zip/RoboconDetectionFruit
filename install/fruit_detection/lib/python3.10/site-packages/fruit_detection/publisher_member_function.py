# dousakakuninzumi 10/9

import rclpy
from rclpy.node import Node

# from std_msgs.msg import String
from std_msgs.msg import Int16MultiArray

state = 9
x = 2
y = 3
kind = 4
conf = 5

class MinimalPublisher(Node):
    def __init__(self):
        super().__init__('fruit_detect_node_test')
        # self.publisher_ = self.create_publisher(String, 'topic', 10)
        self.publisher_ = self.create_publisher(Int16MultiArray, 'image_recognition', 10)
        timer_period = 0.05  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        msg = Int16MultiArray()

        global state
        global x
        global y
        global kind
        global conf
        x = x + 1
        msg.data = [state, x, y, kind, conf]
        self.publisher_.publish(msg)
        self.get_logger().info('Pub test:"%s"' % msg.data)
        self.i += 1

def main(args=None):
    rclpy.init(args=args)

    minimal_publisher = MinimalPublisher()

    rclpy.spin(minimal_publisher)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    minimal_publisher.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
