from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription(
        [
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="Receiver920",
            ),
        ]
    )
