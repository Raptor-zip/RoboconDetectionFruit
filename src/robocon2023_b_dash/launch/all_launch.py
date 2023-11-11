# from launch import LaunchDescription
# from launch_ros.actions import Node
import launch_ros.actions
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration


def generate_launch_description():
    return LaunchDescription(
        [
            # Node(
            #     package="joy",
            #     namespace="robocon2023_b_dash",
            #     executable="joy_node",
            # ),
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
                # prefix= "xterm -e",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="Upper_Controller",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="WallUpMotion",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="Crawl_Controller",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="Crawl_Controller",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="Solenoid_Controller",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="BeltLiner_Controller",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="RopeMotion",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="MotionMaster",
            ),
            Node(
                package="robocon2023_b_dash",
                namespace="robocon2023_b_dash",
                executable="KiwiMotion",
            ),
            Node(
                package="fruit_detection",
                namespace="robocon2023_b_dash",
                executable="pubdetect",
                output = "screen", # print wo hyouzi
                prefix= "xterm -e",
            ),
        ]
    )