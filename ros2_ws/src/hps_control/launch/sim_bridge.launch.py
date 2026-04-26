"""Launch sub_state_node alongside the ROS-TCP endpoint that Unity connects to."""

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='hps_control',
            executable='sub_state_node',
            name='sub_state_node',
        ),
        # Bridges ROS2 DDS traffic to a plain TCP socket that Unity can reach.
        # ROS_IP must match the machine Unity will connect to (127.0.0.1 for
        # local simulation; set to the host's LAN IP for a networked setup).
        Node(
            package='ros_tcp_endpoint',
            executable='default_server_endpoint',
            name='ros_tcp_endpoint',
            parameters=[{'ROS_IP': '127.0.0.1', 'ROS_TCP_PORT': 10000}],
        ),
    ])
