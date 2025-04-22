import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_HAS_TARGET, DEVICE_CLASS_OCCUPANCY

from . import CONF_MICRADAR_ID, MICRADARComponent

DEPENDENCIES = ["micradar"]

CONF_RANGE_BREAKBOUNDS = "range_breakbounds"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MICRADARComponent),
    cv.Optional(CONF_HAS_TARGET): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_OCCUPANCY, icon="mdi:motion-sensor"
    ),
}


async def to_code(config):
    micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])

    if has_target_config := config.get(CONF_HAS_TARGET):
        sens = await binary_sensor.new_binary_sensor(has_target_config)
        cg.add(micradar_component.set_has_target_binary_sensor(sens))

    if range_breakbounds_config := config.get(CONF_RANGE_BREAKBOUNDS):
        sens = await binary_sensor.new_binary_sensor(range_breakbounds_config)
        cg.add(micradar_component.set_range_breakbounds_binary_sensor(sens))
