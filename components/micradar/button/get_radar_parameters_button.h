#pragma once

#include "esphome/components/button/button.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class GetRadarParametersButton : public button::Button, public Parented<MICRADARComponent> {
 public:
  GetRadarParametersButton() = default;

 protected:
  void press_action() override;
};

}  // namespace micradar
}  // namespace esphome
