#include "motion_threshold_number.h"

namespace esphome {
namespace micradar {

void MotionThresholdNumber::control(float value) { this->parent_->set_motion_threshold(value); }

}  // namespace micradar
}  // namespace esphome
