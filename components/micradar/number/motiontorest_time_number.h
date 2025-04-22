#pragma once

#include "esphome/components/number/number.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class MotionToRestTimeNumber : public number::Number, public Parented<MICRADARComponent> {
 public:
  MotionToRestTimeNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace micradar
}  // namespace esphome
