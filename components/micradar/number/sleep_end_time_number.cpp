#include "sleep_end_time_number.h"

namespace esphome {
namespace micradar {

void SleepEndTimeNumber::control(float value) { this->parent_->set_sleep_end_time(value); }

}  // namespace micradar
}  // namespace esphome
