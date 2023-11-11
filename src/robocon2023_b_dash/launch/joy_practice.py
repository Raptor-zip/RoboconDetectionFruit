from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription(
        [
            Node(
                package="joy",
                executable="joy_node",
                # output="screen",
                parameters=[{"device_id": 0}],
                remappings=[("/joy", "robocon2023_b_dash/joy")],
                # namespace="robocon2023_b_dash",
            ),
            Node(
                package="joy",
                executable="joy_node",
                # output="screen",
                parameters=[{"device_id": 1}],
                remappings=[("/joy", "robocon2023_b_dash/joy2")],
                # namespace="robocon2023_b_dash",
            ),
        ]
    )
