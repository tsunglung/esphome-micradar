#include "existence_threshold_number.h"

namespace esphome {
namespace micradar {

void ExistenceThresholdNumber::control(float value) { this->parent_->set_existence_threshold(value); }

}  // namespace micradar
}  // namespace esphome
