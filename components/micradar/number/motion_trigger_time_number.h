#pragma once

#include "esphome/components/number/number.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class MotionTriggerTimeNumber : public number::Number, public Parented<MICRADARComponent> {
 public:
  MotionTriggerTimeNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace micradar
}  // namespace esphome
