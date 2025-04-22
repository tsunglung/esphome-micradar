#include "human_sleep_monitor_switch.h"

namespace esphome {
namespace micradar {

void HumanSleepMonitorSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_human_sleep_monitor(state);
}

}  // namespace micradar
}  // namespace esphome
