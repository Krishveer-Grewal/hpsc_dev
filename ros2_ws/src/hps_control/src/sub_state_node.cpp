// gather and broadcast raw submarine telemetry at 20 Hz.

#include <chrono>      // time
#include <functional>  // std::bind; treat class functions like vars

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/string.hpp>

using namespace std::chrono_literals;

class SubStateNode : public rclcpp::Node
{
public:
  SubStateNode()
  : Node("sub_state_node"),
    depth_(0.0), heading_(0.0), pitch_(0.0), roll_(0.0), forward_speed_(0.0),
    rpm_(0.0), power_output_(0.0), current_thrust_(50.0),
    o2_level_(100.0), leak_detected_(false)
  {
    // Navigation.
    pub_depth_         = create_publisher<std_msgs::msg::Float64>("state/depth",         10);
    pub_heading_       = create_publisher<std_msgs::msg::Float64>("state/heading",       10);
    pub_pitch_         = create_publisher<std_msgs::msg::Float64>("state/pitch",         10);
    pub_roll_          = create_publisher<std_msgs::msg::Float64>("state/roll",          10);
    pub_forward_speed_ = create_publisher<std_msgs::msg::Float64>("state/forward_speed", 10);

    // Power and propulsion.
    pub_rpm_          = create_publisher<std_msgs::msg::Float64>("state/rpm",          10);
    pub_power_output_ = create_publisher<std_msgs::msg::Float64>("state/power_output", 10);

    // System health.
    pub_o2_level_      = create_publisher<std_msgs::msg::Float64>("state/o2_level", 10);
    pub_leak_detected_ = create_publisher<std_msgs::msg::Bool>   ("state/leak",     10);

    // Comms.
    pub_comms_status_ = create_publisher<std_msgs::msg::String>("state/comms_status", 10);

    // One timer drives all publishers so every topic shares the same 20 Hz heartbeat.
    timer_ = create_wall_timer(
      50ms,
      std::bind(&SubStateNode::timer_callback, this));
  }

private:
  void timer_callback()
  {
    simulate();
    publish_all();
  }

  // Updates internal state variables each tick before publishing.
  void simulate()
  {
    // 0.005 %/tick * 20 ticks/s = 0.1 %/s, depletes in ~1000 s.
    o2_level_ = std::max(0.0, o2_level_ - 0.005);

    // Linear drag model: speed = thrust / drag. Placeholder until real thruster
    // feedback is wired in.
    const double kDragCoefficient = 2.5;
    forward_speed_ = current_thrust_ / kDragCoefficient;

    // Threshold chosen to match hardware sensor alert spec.
    leak_detected_ = (o2_level_ < 20.0);
  }

  void publish_all()
  {
    // Inline helper avoids repeating the stamp-and-publish pattern ten times.
    auto pub_f64 = [](auto & pub, double value) {
      auto msg = std_msgs::msg::Float64();
      msg.data = value;
      pub->publish(msg);
    };

    pub_f64(pub_depth_,         depth_);
    pub_f64(pub_heading_,       heading_);
    pub_f64(pub_pitch_,         pitch_);
    pub_f64(pub_roll_,          roll_);
    pub_f64(pub_forward_speed_, forward_speed_);
    pub_f64(pub_rpm_,           rpm_);
    pub_f64(pub_power_output_,  power_output_);
    pub_f64(pub_o2_level_,      o2_level_);

    auto leak_msg = std_msgs::msg::Bool();
    leak_msg.data = leak_detected_;
    pub_leak_detected_->publish(leak_msg);

    auto comms_msg = std_msgs::msg::String();
    comms_msg.data = leak_detected_ ? "ALERT: leak detected" : "nominal";
    pub_comms_status_->publish(comms_msg);

    RCLCPP_INFO(this->get_logger(),
      "depth=%.2fm  hdg=%.1f°  spd=%.2fm/s  O2=%.1f%%  leak=%s",
      depth_, heading_, forward_speed_, o2_level_,
      leak_detected_ ? "YES" : "no");
  }

  // Publishers.
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_depth_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_heading_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_pitch_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_roll_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_forward_speed_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_rpm_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_power_output_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_o2_level_;
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr    pub_leak_detected_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr  pub_comms_status_;

  rclcpp::TimerBase::SharedPtr timer_;

  // State variables.
  double depth_, heading_, pitch_, roll_, forward_speed_;
  double rpm_, power_output_, current_thrust_;
  double o2_level_;
  bool   leak_detected_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SubStateNode>());
  rclcpp::shutdown();
  return 0;
}
