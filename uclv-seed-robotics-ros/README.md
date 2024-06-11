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
To use this project, follow the steps below.
### Running the Hand Driver node
Open a terminal window and run the following command to start the hand driver node:
```bash
ros2 run uclv_seed_robotics_ros hand_driver
```
### Sending commands to motor's hand
In another terminal window, you can send commands to the `/cmd/motor_position` topic. Use the following command to publish the motor IDs and desired positions:
```bash
ros2 topic pub /cmd/motor_position uclv_seed_robotics_ros_interfaces/msg/MotorPositions "{ids: [36, 37], positions: [1000, 1000]}"
```
Make sure to replace [36, 37] with the motor IDs and [1000, 1000] with the desired positions.
You can find default motor ids [here](https://kb.seedrobotics.com/doku.php?id=rh8d:dynamixelidsandcommsettings).

### Running the Fingertip Sensors node
Open a new terminal window and run the following command to start the fingertip sensors node:
```bash
ros2 run uclv_seed_robotics_ros fingertip_sensors
```
### View Motor State
Once the node is running, open another terminal window and use the following command to see what is being published on the /motor_state topic:
```bash
ros2 topic echo /motor_state
```


## Authors

- Roberto Chello - [GitHub Profile](https://github.com/robertochello)
