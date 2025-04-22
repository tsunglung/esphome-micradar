#pragma once

#include "esphome/components/switch/switch.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class HumanHeartWaveSwitch : public switch_::Switch, public Parented<MICRADARComponent> {
 public:
  HumanHeartWaveSwitch() = default;

 protected:
  void write_state(bool state) override;
};

}  // namespace micradar
}  // namespace esphome
