import launch_ros.actions
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    return LaunchDescription([
        launch_ros.actions.Node(
            package='joy',
            executable='joy_node',
            output='screen',
            parameters=[{
                'device_id': 0
            }],
            remappings = [('/joy', 'joy1')]
        ),
        launch_ros.actions.Node(
            package='joy',
            executable='joy_node',
            output='screen',
            parameters=[{
                'device_id': 1
            }],
            remappings = [('/joy', 'joy2')]
        ),
        Node(
            package='robocon2023_b_dash_transceiver',
            executable='usb920_node',
            output='screen'
        ),
        Node(
            package='robocon2023_b_dash_transceiver',
            executable='program_checker_node',
            output='screen'
        )
    ])
