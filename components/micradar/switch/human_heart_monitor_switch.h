#pragma once

#include "esphome/components/switch/switch.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class HumanHeartMonitorSwitch : public switch_::Switch, public Parented<MICRADARComponent> {
 public:
  HumanHeartMonitorSwitch() = default;

 protected:
  void write_state(bool state) override;
};

}  // namespace micradar
}  // namespace esphome
