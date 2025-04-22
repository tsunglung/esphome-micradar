#include "human_exist_switch.h"

namespace esphome {
namespace micradar {

void HumanExistSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_human_exist(state);
}

}  // namespace micradar
}  // namespace esphome
