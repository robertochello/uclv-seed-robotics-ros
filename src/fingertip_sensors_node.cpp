#include <iostream>
#include <vector>
#include <memory>

#include "uclv_dynamixel_utils/colors.hpp"
#include "rclcpp/rclcpp.hpp"
#include "uclv_seed_robotics_ros_interfaces/msg/fts3_sensors.hpp"
#include "std_srvs/srv/trigger.hpp"
#include "serial/serial.h"

class FingertipSensors : public rclcpp::Node
{
public:
    rclcpp::Time time;

    // Parameters
    int millisecondsTimer_;          // Timer duration for publishing sensor data
    std::string serial_port_;        // Serial port for communication with the sensors
    int baudrate_;                   // Baud rate for serial communication
    uint32_t serial_timeout_ = 1000; // Timeout for serial communication
    bool timestamp = false;          // Flag to check if the sensor data contains a timestamp

    // Serial communication objects
    std::shared_ptr<serial::Serial> sensor_read_;
    std::string line;

    // Number of sensors and timestamp offset
    int num_sensors = 5;
    int var_timestamp = 0;

    // Vectors for storing sensor IDs and forces
    std::vector<uint16_t> ids;
    geometry_msgs::msg::Vector3 vec;

    // Topic name for publishing sensor state
    std::string sensor_state_topic_name_;
    std::string calibrate_service_name_;

    // ROS publisher, timer, and service
    rclcpp::Publisher<uclv_seed_robotics_ros_interfaces::msg::FTS3Sensors>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr calibrate_service_;

    // Constructor
    FingertipSensors()
        : Node("fingertip_sensors"),
          millisecondsTimer_(this->declare_parameter<int>("millisecondsTimer", 0)),
          serial_port_(this->declare_parameter<std::string>("serial_port", std::string())),
          baudrate_(this->declare_parameter<int>("baudrate", 0)),
          sensor_state_topic_name_(this->declare_parameter<std::string>("sensor_state_topic", std::string())),
          calibrate_service_name_(this->declare_parameter<std::string>("calibrate_service_name", std::string()))
    {

        check_parameters();

        // Set serial port to low latency
        setSerialPortLowLatency(serial_port_);
        sensor_read_ = std::make_shared<serial::Serial>(serial_port_, baudrate_, serial::Timeout::simpleTimeout(serial_timeout_));

        // Create publisher for sensor data
        publisher_ = this->create_publisher<uclv_seed_robotics_ros_interfaces::msg::FTS3Sensors>(sensor_state_topic_name_, 1);

        // Initialize sensor communication by flushing and sending necessary commands
        sensor_read_->flush();
        write_on_serial("pausedata\r\n");
        sensor_read_->flush();
        write_on_serial("calibrate\r\n");
        write_on_serial("enabletime\r\n");
        write_on_serial("resume\r\n");

        // Create a calibration service
        calibrate_service_ = this->create_service<std_srvs::srv::Trigger>(
            calibrate_service_name_, std::bind(&FingertipSensors::handle_calibration, this, std::placeholders::_1, std::placeholders::_2));

        // Create a timer to periodically publish sensor state
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(millisecondsTimer_),
            std::bind(&FingertipSensors::publish_state, this));
    }

private:
    void check_parameters()
    {
        auto check_string_parameter = [this](const std::string &param_name, const std::string &value)
        {
            if (value.empty())
            {
                RCLCPP_ERROR(this->get_logger(), "Parameter '%s' is missing or empty. Please provide a valid value.", param_name.c_str());
                rclcpp::shutdown();
                throw std::runtime_error("Invalid or missing parameter: '" + param_name + "'");
            }
        };

        auto check_double_parameter = [this](const std::string &param_name, const double &value)
        {
            if (value != 0.0)
            {
                RCLCPP_ERROR(this->get_logger(), "Parameter '%s' is missing or empty. Please provide a valid vector.", param_name.c_str());
                rclcpp::shutdown();
                throw std::runtime_error("Invalid or missing parameter: '" + param_name + "'");
            }
        };

        auto check_vector_int_parameter = [this](const std::string &param_name, const std::vector<int64_t> &value)
        {
            if (value.empty())
            {
                RCLCPP_ERROR(this->get_logger(), "Parameter '%s' is missing or empty. Please provide a valid vector.", param_name.c_str());
                rclcpp::shutdown();
                throw std::runtime_error("Invalid or missing parameter: '" + param_name + "'");
            }
        };

        auto check_vector_string_parameter = [this](const std::string &param_name, const std::vector<std::string> &value)
        {
            if (value.empty())
            {
                RCLCPP_ERROR(this->get_logger(), "Parameter '%s' is missing or empty. Please provide a valid vector.", param_name.c_str());
                rclcpp::shutdown();
                throw std::runtime_error("Invalid or missing parameter: '" + param_name + "'");
            }
        };

        auto check_int_parameter = [this](const std::string &param_name, const int &value)
        {
            if (value == 0)
            {
                RCLCPP_ERROR(this->get_logger(), "Parameter '%s' is missing or empty. Please provide a valid vector.", param_name.c_str());
                rclcpp::shutdown();
                throw std::runtime_error("Invalid or missing parameter: '" + param_name + "'");
            }
        };

        check_int_parameter("millisecondsTimer", millisecondsTimer_);
        check_string_parameter("serial_port", serial_port_);
        check_int_parameter("baudrate", baudrate_);
        check_string_parameter("sensor_state_topic", sensor_state_topic_name_);
        check_string_parameter("calibrate_service_name", calibrate_service_name_);

        RCLCPP_INFO(this->get_logger(), "All required parameters are set correctly.");
    }

    // Function to set the serial port to low latency mode
    void setSerialPortLowLatency(const std::string &serial_port)
    {
        std::cout << "Setting low latency for " << WARN_COLOR << serial_port << CRESET << std::endl;
        std::string command = "setserial " + serial_port + " low_latency";
        int result = system(command.c_str());
        std::cout << "Set low latency for " << WARN_COLOR << serial_port << CRESET
                  << " result: " << SUCCESS_COLOR << result << CRESET << std::endl;
    }

    // Function to send a command over the serial connection
    void write_on_serial(const std::string &cmd)
    {
        sensor_read_->write(cmd);
        sensor_read_->waitByteTimes(100 * cmd.size());
    }

    // Function to handle calibration requests
    void handle_calibration(
        const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
        std::shared_ptr<std_srvs::srv::Trigger::Response> response)
    {
        try
        {
            // Perform sensor calibration
            RCLCPP_INFO(this->get_logger(), "Calibrating sensors...");
            sensor_read_->flush();            // Clear communication buffers
            write_on_serial("calibrate\r\n"); // Send the calibration command
            sensor_read_->flush();
            response->success = true;
            response->message = "Calibration successful.";
            RCLCPP_INFO(this->get_logger(), "Calibration completed.");
        }
        catch (const std::exception &e)
        {
            response->success = false;
            response->message = std::string("Calibration failed: ") + e.what();
            RCLCPP_ERROR(this->get_logger(), "Calibration failed: %s", e.what());
        }
    }

    // Function to read and publish sensor data
    void publish_state()
    {
        // Create a new message
        auto message = uclv_seed_robotics_ros_interfaces::msg::FTS3Sensors();
        message.header.stamp = rclcpp::Clock{}.now(); // Get current time

        // Read data from the sensor
        line = sensor_read_->readline();
        if (line.empty())
        {
            RCLCPP_WARN(this->get_logger(), "No data received from the sensor.");
            return;
        }

        // Tokenize the received line using commas as delimiters
        std::string token;
        std::vector<std::string> tokens;
        char delimiter = ',';
        std::stringstream ss(line);

        while (getline(ss, token, delimiter))
        {
            tokens.push_back(token);
        }
        tokens.pop_back(); // Remove any empty token at the end

        // Check if there is a timestamp in the data
        if (tokens.size() == 18)
        {
            timestamp = true;
            var_timestamp = 2;
        }

        // Resize the vectors to accommodate sensor data
        message.ids.resize(num_sensors);
        ids.resize(num_sensors);

        // Parse and assign sensor forces
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
            message.ids = ids; // Assign sensor IDs to the message
        }

        // Publish the message
        publisher_->publish(message);
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    try
    {
        auto fingertip_sensors_node = std::make_shared<FingertipSensors>();
        rclcpp::spin(fingertip_sensors_node);
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
