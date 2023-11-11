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
                remappings=[("/joy", "robocon2023_b_dash/raw_joy1")],
                # namespace="robocon2023_b_dash",
            ),
            Node(
                package="joy",
                executable="joy_node",
                # output="screen",
                parameters=[{"device_id": 1}],
                remappings=[("/joy", "robocon2023_b_dash/raw_joy2")],
                # namespace="robocon2023_b_dash",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="Transmitter920",
            ),
        ]
    )
