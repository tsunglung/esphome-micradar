#include "human_heart_wave_switch.h"

namespace esphome {
namespace micradar {

void HumanHeartWaveSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_human_heart_wave(state);
}

}  // namespace micradar
}  // namespace esphome
