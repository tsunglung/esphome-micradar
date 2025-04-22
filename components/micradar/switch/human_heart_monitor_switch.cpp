#include "human_heart_monitor_switch.h"

namespace esphome {
namespace micradar {

void HumanHeartMonitorSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_human_heart_monitor(state);
}

}  // namespace micradar
}  // namespace esphome
