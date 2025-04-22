import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_MICRADAR_ID, MICRADARComponent

CONF_EQUIPMENT_STATE = "equipment_state"
CONF_PRODUCT_MODEL = "product_model"
CONF_PRODUCT_ID = "product_id"
CONF_HARDWARE_MODEL = "hardware_model"
CONF_HARDWARE_VERSION = "hardware_version"

CONF_KEEP_AWAY = "keep_away"
CONF_MOTION_STATUS = "motion_status"
CONF_BREATHE_INFO = "breathe_info"
CONF_SLEEP_STATUS = "sleep_status"
CONF_SLEEP_QUALITY_LEVEL = "sleep_quality_level"
CONF_SLEEP_TOTAL_STATUS = "sleep_total_status"

CONF_CUSTOM_MODE_END = "custom_mode_end"


# The entity category for read only diagnostic values, for example RSSI, uptime or MAC Address
CONFIG_SCHEMA = {
    cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MICRADARComponent),
    cv.Optional(CONF_EQUIPMENT_STATE): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:connection"
    ),
    cv.Optional(CONF_PRODUCT_MODEL): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:information-outline"
    ),
    cv.Optional(CONF_PRODUCT_ID): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:information-outline"
    ),
    cv.Optional(CONF_HARDWARE_MODEL): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:information-outline"
    ),
    cv.Optional(CONF_HARDWARE_VERSION): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:information-outline"
    ),
    cv.Optional(CONF_KEEP_AWAY): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:walk"
    ),
    cv.Optional(CONF_MOTION_STATUS): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:human-greeting"
    ),
    cv.Optional(CONF_BREATHE_INFO): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:weather-windy"
    ),
    cv.Optional(CONF_SLEEP_STATUS): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:bed",
    ),
    cv.Optional(CONF_SLEEP_QUALITY_LEVEL): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:bed",
    ),
    cv.Optional(CONF_SLEEP_TOTAL_STATUS): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:bed-king",
    ),
    cv.Optional(CONF_CUSTOM_MODE_END): text_sensor.text_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC, icon="mdi:account-check"
    ),
}


async def to_code(config):
    micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])
    if equipment_state_config := config.get(CONF_EQUIPMENT_STATE):
        sens = await text_sensor.new_text_sensor(equipment_state_config)
        cg.add(micradar_component.set_equipment_state_text_sensor(sens))
    if productmodel_config := config.get(CONF_PRODUCT_MODEL):
        sens = await text_sensor.new_text_sensor(productmodel_config)
        cg.add(micradar_component.set_product_model_text_sensor(sens))
    if productid_config := config.get(CONF_PRODUCT_ID):
        sens = await text_sensor.new_text_sensor(productid_config)
        cg.add(micradar_component.set_product_id_text_sensor(sens))
    if hardwaremodel_config := config.get(CONF_HARDWARE_MODEL):
        sens = await text_sensor.new_text_sensor(hardwaremodel_config)
        cg.add(micradar_component.set_hardware_model_text_sensor(sens))
    if firwareversion_config := config.get(CONF_HARDWARE_VERSION):
        sens = await text_sensor.new_text_sensor(firwareversion_config)
        cg.add(micradar_component.set_firware_version_text_sensor(sens))
    if keepaway_config := config.get(CONF_KEEP_AWAY):
        sens = await text_sensor.new_text_sensor(keepaway_config)
        cg.add(micradar_component.set_keep_away_text_sensor(sens))
    if motionstatus_config := config.get(CONF_MOTION_STATUS):
        sens = await text_sensor.new_text_sensor(motionstatus_config)
        cg.add(micradar_component.set_motion_status_text_sensor(sens))
    if breathe_info_config := config.get(CONF_BREATHE_INFO):
        sens = await text_sensor.new_text_sensor(breathe_info_config)
        cg.add(micradar_component.set_breathe_info_text_sensor(sens))
    if sleep_status_config := config.get(CONF_SLEEP_STATUS):
        sens = await text_sensor.new_text_sensor(sleep_status_config)
        cg.add(micradar_component.set_sleep_status_text_sensor(sens))
    if sleep_quality_level_config := config.get(CONF_SLEEP_QUALITY_LEVEL):
        sens = await text_sensor.new_text_sensor(sleep_quality_level_config)
        cg.add(micradar_component.set_sleep_quality_level_text_sensor(sens))
    if sleep_total_status_config := config.get(CONF_SLEEP_TOTAL_STATUS):
        sens = await text_sensor.new_text_sensor(sleep_total_status_config)
        cg.add(micradar_component.set_sleep_total_status_text_sensor(sens))
    if custommodeend_config := config.get(CONF_CUSTOM_MODE_END):
        sens = await text_sensor.new_text_sensor(custommodeend_config)
        cg.add(micradar_component.set_custom_mode_end_text_sensor(sens))
