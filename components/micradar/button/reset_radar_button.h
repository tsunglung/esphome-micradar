#pragma once

#include "esphome/components/button/button.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class ResetRadarButton : public button::Button, public Parented<MICRADARComponent> {
 public:
  ResetRadarButton() = default;

 protected:
  void press_action() override;
};

}  // namespace micradar
}  // namespace esphome
