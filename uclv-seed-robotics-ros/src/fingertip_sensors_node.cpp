#include <iostream>
#include <vector>
#include <memory>

#include "uclv_dynamixel_utils/colors.hpp"

#include "rclcpp/rclcpp.hpp"
#include "uclv_seed_robotics_ros_interfaces/msg/fts3_sensors.hpp"

#include "serial/serial.h"


class FingertipSensors : public rclcpp::Node
{
public:
    rclcpp::Time time;

    int millisecondsTimer_;
    std::string serial_port_;
    int baudrate_;

    uint32_t serial_timeout_ = 1000;
    bool timestamp = false;

    std::shared_ptr<serial::Serial> sensor_read_;
    std::string line;

    int num_sensors = 5;
    int var_timestamp = 0;

    std::vector<uint16_t> ids;
    geometry_msgs::msg::Vector3 vec;

    rclcpp::Publisher<uclv_seed_robotics_ros_interfaces::msg::FTS3Sensors>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    void setSerialPortLowLatency(const std::string &serial_port)
    {
        std::cout << "Setting low latency for " << WARN_COLOR << serial_port << CRESET << std::endl;
        std::string command = "setserial " + serial_port + " low_latency";
        int result = system(command.c_str());
        std::cout << "Setted low latency for " << WARN_COLOR << serial_port << CRESET
                  << " result: " << SUCCESS_COLOR << result << CRESET << std::endl;
    }

    FingertipSensors()
        : Node("test_read_from_sensors")
    {

        millisecondsTimer_ = this->declare_parameter<int>("millisecondsTimer", 2);
        serial_port_ = this->declare_parameter<std::string>("serial_port", "/dev/ttyUSB1");
        baudrate_ = this->declare_parameter<int>("baudrate", 1000000);

        setSerialPortLowLatency(serial_port_);
        sensor_read_ = std::make_shared<serial::Serial>(serial_port_, baudrate_, serial::Timeout::simpleTimeout(serial_timeout_));

        publisher_ = this->create_publisher<uclv_seed_robotics_ros_interfaces::msg::FTS3Sensors>("sensor_state", 1);

        sensor_read_->flush();
        write_on_serial("pausedata\r\n");
        sensor_read_->flush();
        write_on_serial("calibrate\r\n");
        write_on_serial("enabletime\r\n");
        write_on_serial("resume\r\n");

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(millisecondsTimer_),
            std::bind(&FingertipSensors::publish_state, this));
    }

private:
    void write_on_serial(const std::string &cmd)
    {
        sensor_read_->write(cmd);
        sensor_read_->waitByteTimes(100 * cmd.size());
    }

    void publish_state()
    {

        auto message = uclv_seed_robotics_ros_interfaces::msg::FTS3Sensors();
        message.header.stamp = rclcpp::Clock{}.now();

        line = (sensor_read_->readline());
        if (line == "")
        {
            // ERROR
        }
        else
        {
            std::string token;
            std::vector<std::string> tokens;
            char delimiter = ',';
            std::stringstream ss(line);

            while (getline(ss, token, delimiter))
            {
                tokens.push_back(token);
            }
            tokens.pop_back();

            if (tokens.size() == 18)
            {
                timestamp = true;
                var_timestamp = 2;
            }

            message.ids.resize(num_sensors);
            ids.resize(num_sensors);

            if (tokens[0] == "@")
            {
                for (size_t i = 1; i < 6; i++)
                {
                    auto fx = tokens[var_timestamp - 2 + 3 * i];
                    auto fy = tokens[var_timestamp - 1 + 3 * i];
                    auto fz = tokens[var_timestamp + 0 + 3 * i];
                    ids[i] = i;
                    vec.x = std::stof(fx);
                    vec.y = std::stof(fy);
                    vec.z = std::stof(fz);
                    message.forces.push_back(vec);
                }
                message.ids = ids;
            }
        }

        publisher_->publish(message);
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    try
    {
        auto hand_driver_node = std::make_shared<FingertipSensors>();
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