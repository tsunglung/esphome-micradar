#include "sleep_absence_time_number.h"

namespace esphome {
namespace micradar {

void SleepAbsenceTimeNumber::control(float value) { this->parent_->set_sleep_absence_time(value); }

}  // namespace micradar
}  // namespace esphome
