#include "existence_boundary_select.h"

namespace esphome {
namespace micradar {

void ExistenceBoundarySelect::control(const std::string &value) {
  this->publish_state(value);
  auto index = this->index_of(value);
  if (index.has_value()) {
    this->parent_->set_existence_boundary(index.value());
  }
}

}  // namespace micradar
}  // namespace esphome
