"""Launch sub_state_node and the ROS-TCP endpoint for Unity simulation."""

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='hps_control',
            executable='sub_state_node',
            name='sub_state_node',
        ),
        # 0.0.0.0 binds to every network interface on this machine, so Unity
        # on Windows can reach the endpoint through the WSL2 virtual adapter
        # without needing to know its ever-changing IP address.

        #!NEEDTO clean up port bindings later on
        Node(
            package='ros_tcp_endpoint',
            executable='default_server_endpoint',
            name='ros_tcp_endpoint',
            parameters=[{'ROS_IP': '0.0.0.0', 'ROS_TCP_PORT': 10000}],
        ),
    ])
