#include <iostream>
#include <vector>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "uclv_seed_robotics_ros_interfaces/msg/motor_positions.hpp"

#include "uclv_dynamixel_utils/hand.hpp"

using namespace uclv::dynamixel_utils;

class HandDriver : public rclcpp::Node
{
public:
    std::shared_ptr<Hand> hand_;
    dynamixel::PortHandler *portHandler;
    dynamixel::PacketHandler *packetHandler;

    int millisecondsTimer_;
    std::string serial_port_;
    int baudrate_;
    float protocol_version_;
    std::vector<int64_t> motor_ids_;
    std::vector<int64_t> motor_thresholds_;

    rclcpp::Publisher<uclv_seed_robotics_ros_interfaces::msg::MotorPositions>::SharedPtr publisher_;
    rclcpp::Subscription<uclv_seed_robotics_ros_interfaces::msg::MotorPositions>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;

    HandDriver()
        : Node("hand_driver")
    {
        millisecondsTimer_ = this->declare_parameter<int>("millisecondsTimer", 2);
        serial_port_ = this->declare_parameter<std::string>("serial_port", "/dev/ttyUSB0");
        baudrate_ = this->declare_parameter<int>("baudrate", 1000000);
        protocol_version_ = this->declare_parameter<float>("protocol_version", 2.0);
        motor_ids_ = this->declare_parameter<std::vector<int64_t>>("motor_ids", std::vector<int64_t>());
        motor_thresholds_ = this->declare_parameter<std::vector<int64_t>>("motor_thresholds", std::vector<int64_t>());

        // motor_thresholds_ = {
        //     {31, {0, 4095}},
        //     {32, {0, 4095}},
        //     {33, {0, 4095}},
        //     {34, {0, 4095}},
        //     {35, {0, 4095}},
        //     {36, {0, 4095}},
        //     {37, {0, 4095}},
        //     {38, {0, 4095}}
        // };

        hand_ = std::make_shared<Hand>(serial_port_, baudrate_, protocol_version_);
        hand_->setSerialPortLowLatency(serial_port_);
        if (!hand_->initialize())
        {
            throw std::runtime_error("Error: Hand not initialized");
        }

        if (motor_ids_.size() == 0)
        {
            throw std::runtime_error("Error: Motor IDs parameter is empty");
        }
        for (const auto &id : motor_ids_)
        { // forse check su id da 31 a 38
            if (id <= 0 || id >= 255)
            {
                RCLCPP_ERROR(this->get_logger(), "Invalid motor ID: %ld. Must be between 1 and 254.", id);
                throw std::invalid_argument("Invalid motor ID");
            }
            if (id > 30 && id < 34)
            {
                hand_->addWristMotor(id);
            }
            if (id > 33 && id < 39)
            {
                hand_->addFingerMotor(id);
            }
        }

        publisher_ = this->create_publisher<uclv_seed_robotics_ros_interfaces::msg::MotorPositions>("motor_state", 1);

        subscription_ = this->create_subscription<uclv_seed_robotics_ros_interfaces::msg::MotorPositions>(
            "/cmd/motor_position", 1,
            std::bind(&HandDriver::topic_callback, this, std::placeholders::_1));

        // Timer
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(millisecondsTimer_),
            std::bind(&HandDriver::publish_state, this));
    }

private:
    void publish_state()
    {
        std::vector<uint8_t> motor_ids_uint8t_vec;
        motor_ids_uint8t_vec.reserve(motor_ids_.size());
        for (size_t i = 0; i < motor_ids_.size(); i++)
        {
            motor_ids_uint8t_vec.push_back(static_cast<uint8_t>(motor_ids_[i]));
        }

        std::vector<uint32_t> motor_pos;
        try
        {
            motor_pos = hand_->readMotorsPositions(motor_ids_uint8t_vec);
        }
        catch (...)
        {
            RCLCPP_INFO_STREAM_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "ERROR");
            return;
        }

        auto message = uclv_seed_robotics_ros_interfaces::msg::MotorPositions();
        message.positions.resize(motor_pos.size());
        message.ids = motor_ids_uint8t_vec;

        for (size_t i = 0; i < motor_pos.size(); i++)
        {
            message.positions[i] = static_cast<float>(motor_pos[i]);
        }

        publisher_->publish(message);
    }

    void topic_callback(const uclv_seed_robotics_ros_interfaces::msg::MotorPositions::SharedPtr pos)
    {
        for (size_t i = 0; i < pos->ids.size(); ++i)
        {
            auto id = pos->ids[i];
            auto position = pos->positions[i];

            if (position < motor_thresholds_[0] || position > motor_thresholds_[1])
            {
                RCLCPP_ERROR(this->get_logger(), "Position %f for motor ID %d out of range [%ld, %ld]", position, id, motor_thresholds_[0], motor_thresholds_[1]);
                return;
            }
        }

        try
        {
            hand_->moveMotors(pos->ids, pos->positions);
        }
        catch (...)
        {
            RCLCPP_INFO_STREAM_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "ERROR");
            return;
        }
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    try
    {
        auto hand_driver_node = std::make_shared<HandDriver>();
        rclcpp::spin(hand_driver_node);
    }
    catch (const std::exception &e)
    {
        RCLCPP_FATAL(rclcpp::get_logger("rclcpp"), "Exception caught: %s", e.what());
        rclcpp::shutdown();
        return 1;
    }

    rclcpp::shutdown();
    return 0;
}
