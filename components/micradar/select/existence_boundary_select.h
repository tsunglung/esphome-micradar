#pragma once

#include "esphome/components/select/select.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class ExistenceBoundarySelect : public select::Select, public Parented<MICRADARComponent> {
 public:
  ExistenceBoundarySelect() = default;

 protected:
  void control(const std::string &value) override;
};

}  // namespace micradar
}  // namespace esphome
