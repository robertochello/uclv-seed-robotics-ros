from typing import List
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            output='screen',
            package='uclv_seed_robotics_ros',
            # namespace='',
            executable='hand_driver',
            name='hand_driver',
            parameters=[
                {"motor_ids": [31, 32, 33, 34, 35, 36, 37, 38]}, # left hand
                {"motor_thresholds": [100, 3995]}
            ]
        ),
        Node(
            output='screen',
            package="uclv_seed_robotics_ros",
            # namespace =''
            executable='fingertip_sensors', #####
            parameters=[
                {}
            ]
        ),
    ])
    