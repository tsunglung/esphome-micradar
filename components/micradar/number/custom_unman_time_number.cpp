#include "custom_unman_time_number.h"

namespace esphome {
namespace micradar {

void CustomUnmanTimeNumber::control(float value) { this->parent_->set_custom_unman_time(value); }

}  // namespace micradar
}  // namespace esphome
