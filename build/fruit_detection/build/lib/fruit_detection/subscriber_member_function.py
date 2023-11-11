# dousakakuninzumi 10/9

import rclpy
from rclpy.node import Node
from std_msgs.msg import Int16MultiArray
from sensor_msgs.msg import Joy


class MinimalSubscriber(Node):
    def __init__(self):
        super().__init__('minimal_subscriber')
        # self.subscription = self.create_subscription(
        #     Int16MultiArray,
        #     'image_recognition',
        #     self.listener_callback,
        #     10)
        self.subscription = self.create_subscription(
            Joy,
            '/joy',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        self.get_logger().info('Sub: "%s"' % msg.button)
        # self.get_logger().info('   x   : "%s"' % msg.data[1])
        # self.get_logger().info('   y   : "%s"' % msg.data[2])
        # self.get_logger().info(' kind  : "%s"' % msg.data[3])
        # self.get_logger().info('"%s":' % msg.data[3] % msg.data[1])
        # self.get_logger().info(' conf  : "%s"' % msg.data[4])
        # state:0起動成功 1フルーツ検出 2カメラ未接続
        # x:-50から50までの少数0なら中央。正の値なら右方向に移動、負の値なら、左方向に移動
        # y:0から100までの少数。画面下なら0、画面の上なら1 使う予定はない
        # kind:0キウイ,1みかん,2もも,3ぶどう,4ブルーベリー,5ミックスフルーツ 使う予定はないが、昇降用


def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = MinimalSubscriber()

    rclpy.spin(minimal_subscriber)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
