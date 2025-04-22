#include "motion_trigger_time_number.h"

namespace esphome {
namespace micradar {

void MotionTriggerTimeNumber::control(float value) { this->parent_->set_motion_trigger_time(value); }

}  // namespace micradar
}  // namespace esphome
