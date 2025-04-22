#include "breathe_slow_level_number.h"

namespace esphome {
namespace micradar {

void BreatheSlowLevelNumber::control(float value) { this->parent_->set_breathe_slow_level(value); }

}  // namespace micradar
}  // namespace esphome
