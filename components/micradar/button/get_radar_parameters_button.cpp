#include "get_radar_parameters_button.h"

namespace esphome {
namespace micradar {

void GetRadarParametersButton::press_action() { this->parent_->get_radar_parameters(); }

}  // namespace micradar
}  // namespace esphome
