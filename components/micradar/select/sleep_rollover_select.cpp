#include "sleep_rollover_select.h"

namespace esphome {
namespace micradar {

void SleepRollOverSelect::control(const std::string &value) {
  this->publish_state(value);
  auto index = this->index_of(value);
  if (index.has_value()) {
    this->parent_->set_sleep_rollover(index.value());
  }
}

}  // namespace micradar
}  // namespace esphome
