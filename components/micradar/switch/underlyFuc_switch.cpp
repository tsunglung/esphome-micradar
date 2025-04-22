#include "underlyFuc_switch.h"

namespace esphome {
namespace micradar {

void UnderlyOpenFunctionSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_underlying_open_function(state);
}

}  // namespace micradar
}  // namespace esphome
