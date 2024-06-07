<!-- omit in toc -->
# UCLV Robot Hand Controller ROS 2


This repository contains driver, in ROS2 and C++, for RH8D robotic hand by Seed Robotics. Developed at the Università degli Studi della Campania Luigi Vanvitelli.

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
   git clone https://github.com/robertochello/uclv-robot-hand-controller-ros2.git
   ```
2. Build the package using `colcon`:
    ```bash
    cd /path/to/your/ros2/workspace
    colcon build --packages-select uclv_robot_hand_controller
    ```
## Dependencies

- [ROS 2](https://index.ros.org/doc/ros2/) - Robot Operating System 2
- `rclcpp` - ROS 2 C++ Client Library


## Additional Dependencies

This project requires additional dependencies from other repositories. Clone the following repositories into your ROS 2 workspace:
- `custom_msg` - Custom ROS 2 Message package
- `my_library` - External library for robotic hand control

1. Custom Message Definitions:
   ```bash
    cd /path/to/your/ros2/workspace/src
    git clone https://github.com/robertochello/custom_msg.git
    ```
2. Custom Library:
    ```bash
    cd /path/to/your/ros2/workspace/src
    git clone https://github.com/robertochello/my_library.git
    ```

## Usage

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Authors

- Roberto Chello - [GitHub Profile](https://github.com/robertochello)
