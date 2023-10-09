from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription(
        [
            Node(
                package="joy",
                namespace="robocon2023_b_dash",
                executable="joy_node",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="SerialKIMD",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="Xmove_Controller",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="DriveMotion",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="Drive_Controller",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="Upper_Controller",
            ),
            Node(
                package="fruit_detection",
                namespace="robocon2023_b_dash",
                executable="pubdetect",
                # prefix= "xterm -e",
                output = "screen", # print wo hyouzi
            ),
        ]
    )
