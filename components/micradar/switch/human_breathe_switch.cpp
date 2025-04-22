#include "human_breathe_switch.h"

namespace esphome {
namespace micradar {

void HumanBreatheSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_human_breathe(state);
}

}  // namespace micradar
}  // namespace esphome
