cmake_minimum_required(VERSION 3.8)
project(uclv_seed_robotics_ros)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(uclv_dynamixel_utils REQUIRED)
find_package(uclv_seed_robotics_ros_interfaces REQUIRED)
find_package(serial REQUIRED)
find_package(std_srvs REQUIRED)



add_executable(hand_driver src/hand_driver_node.cpp)
ament_target_dependencies(
  hand_driver
  "rclcpp"
  "uclv_seed_robotics_ros_interfaces"
  "uclv_dynamixel_utils"
)

add_executable(fingertip_sensors src/fingertip_sensors_node.cpp)
ament_target_dependencies(
  fingertip_sensors
  "rclcpp"
  "uclv_seed_robotics_ros_interfaces"
  "uclv_dynamixel_utils"
  "serial"
  "std_srvs"
)

install(TARGETS 
      hand_driver
      fingertip_sensors
      DESTINATION 
        lib/${PROJECT_NAME})

  install(DIRECTORY
      launch
      DESTINATION
        share/${PROJECT_NAME}/
  )


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
