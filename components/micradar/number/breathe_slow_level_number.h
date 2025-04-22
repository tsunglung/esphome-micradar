#pragma once

#include "esphome/components/number/number.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class BreatheSlowLevelNumber : public number::Number, public Parented<MICRADARComponent> {
 public:
  BreatheSlowLevelNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace micradar
}  // namespace esphome
