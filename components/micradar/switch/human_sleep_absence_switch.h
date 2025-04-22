#pragma once

#include "esphome/components/switch/switch.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class HumanSleepAbsenceSwitch : public switch_::Switch, public Parented<MICRADARComponent> {
 public:
  HumanSleepAbsenceSwitch() = default;

 protected:
  void write_state(bool state) override;
};

}  // namespace micradar
}  // namespace esphome
