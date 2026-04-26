# Resources

https://docs.ros2.org/foxy/api/rclcpp/index.html

---

## Build

```bash
cd ~/hpsc_dev/ros2_ws
colcon build --packages-select hps_control
source install/setup.bash
```

Re-run `source install/setup.bash` after every build.

---

## Run

```bash
ros2 run hps_control sub_state_node
```

---

## Test / Inspect

```bash
ros2 topic list                    # all active topics
ros2 topic echo /state/depth       # live data on a topic
ros2 topic hz /state/depth         # verify publish rate
ros2 node info /sub_state_node     # publishers, subscribers, services
```

---

## One-time shell setup (add to ~/.bashrc)

```bash
source /opt/ros/jazzy/setup.bash
source ~/hpsc_dev/ros2_ws/install/setup.bash
```
