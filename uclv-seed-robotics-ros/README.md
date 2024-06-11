<!-- omit in toc -->
# UCLV Seed Robotic ROS


This project contains two nodes: a node to control the RH8D robotic hand from Seed Robotics and another node to obtain information from the FTS3 touch sensors, also from Seed Robotics.
Developed at the University of Campania Luigi Vanvitelli.

<!-- omit in toc -->
## Summary


- [Installation](#installation)
- [Dependencies](#dependencies)
- [Additional Dependencies](#additional-dependencies)
- [License](#license)
- [Authors](#authors)

## Installation

1. Clone this repository into your ROS 2 workspace:
   ```bash
   cd /path/to/your/ros2/workspace/src
   git clone https://github.com/robertochello/uclv-seed-robotics-ros.git
   ```
2. Build the package using `colcon`:
    ```bash
    cd /path/to/your/ros2/workspace
    colcon build --packages-select uclv_seed_robotics_ros
    ```
## Dependencies

- [ROS 2](https://index.ros.org/doc/ros2/) - Robot Operating System 2
- `rclcpp` - ROS 2 C++ Client Library


## Additional Dependencies

This project requires additional dependencies from other repositories. Clone the following repositories into your ROS 2 workspace:
- `uclv_seed_robotics_ros_interfaces` - Custom package with ROS 2 message
- `uclv_dynamixel_utils` - External library for robotic hand control

1. Interfaces:
   ```bash
    cd /path/to/your/ros2/workspace/src
    git clone https://github.com/robertochello/uclv-seed-robotics-ros-interfaces.git
    ```
2. Library:
    ```bash
    cd /path/to/your/ros2/workspace/src
    git clone https://github.com/robertochello/uclv-dynamixel-utils.git
    ```

## Usage


## Authors

- Roberto Chello - [GitHub Profile](https://github.com/robertochello)
