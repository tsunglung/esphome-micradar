#include "sensitivity_number.h"

namespace esphome {
namespace micradar {

void SensitivityNumber::control(float value) { this->parent_->set_sensitivity(value); }

}  // namespace micradar
}  // namespace esphome
