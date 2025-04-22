#include "reset_radar_button.h"

namespace esphome {
namespace micradar {

void ResetRadarButton::press_action() { this->parent_->factory_reset(); }

}  // namespace micradar
}  // namespace esphome
