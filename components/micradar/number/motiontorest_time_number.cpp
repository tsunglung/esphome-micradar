#include "motiontorest_time_number.h"

namespace esphome {
namespace micradar {

void MotionToRestTimeNumber::control(float value) { this->parent_->set_motion_to_rest_time(value); }

}  // namespace micradar
}  // namespace esphome
