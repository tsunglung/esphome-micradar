#include "human_sleep_absence_switch.h"

namespace esphome {
namespace micradar {

void HumanSleepAbsenceSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_human_sleep_absence(state);
}

}  // namespace micradar
}  // namespace esphome
