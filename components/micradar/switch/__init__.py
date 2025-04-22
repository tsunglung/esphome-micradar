import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_SWITCH,
    ENTITY_CATEGORY_CONFIG,
    ICON_HEART_PULSE,
    ICON_PULSE
)

from .. import CONF_MICRADAR_ID, MICRADARComponent, micradar_ns

HumanExistSwitch = micradar_ns.class_(
    "HumanExistSwitch", switch.Switch
)

HumanBreatheSwitch = micradar_ns.class_(
    "HumanBreatheSwitch", switch.Switch
)

HumanHeartMonitorSwitch = micradar_ns.class_(
    "HumanHeartMonitorSwitch", switch.Switch
)

HumanSleepMonitorSwitch = micradar_ns.class_(
    "HumanSleepMonitorSwitch", switch.Switch
)

HumanSleepAbsenceSwitch = micradar_ns.class_(
    "HumanSleepAbsenceSwitch", switch.Switch
)

UnderlyingOpenFuncSwitch = micradar_ns.class_(
    "UnderlyOpenFunctionSwitch", switch.Switch
)

CONF_HUMAN_EXIST = "human_exist"
CONF_HUMAN_BREATH = "human_breath"
CONF_HUMAN_HEART_MONITOR = "human_heart_monitor"
CONF_HUMAN_SLEEP_MONITOR = "human_sleep_monitor"
CONF_HUMAN_SLEEP_ABSENCE = "human_sleep_absence"
CONF_UNDERLYING_OPEN_FUNCTION = "underlying_open_function"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MICRADARComponent),
    cv.Optional(CONF_HUMAN_EXIST): switch.switch_schema(
        HumanExistSwitch,
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:human",
    ),
    cv.Optional(CONF_HUMAN_BREATH): switch.switch_schema(
        HumanBreatheSwitch,
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon=ICON_PULSE,
    ),
    cv.Optional(CONF_HUMAN_HEART_MONITOR): switch.switch_schema(
        HumanHeartMonitorSwitch,
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon=ICON_HEART_PULSE,
    ),
    cv.Optional(CONF_UNDERLYING_OPEN_FUNCTION): switch.switch_schema(
        UnderlyingOpenFuncSwitch,
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:electric-switch",
    ),
    cv.Optional(CONF_HUMAN_SLEEP_MONITOR): switch.switch_schema(
        HumanSleepMonitorSwitch,
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:sleep",
    ),
    cv.Optional(CONF_HUMAN_SLEEP_ABSENCE): switch.switch_schema(
        HumanSleepAbsenceSwitch,
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:sleep",
    )
}


async def to_code(config):
    micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])
    if human_exist_config := config.get(CONF_HUMAN_EXIST):
        s = await switch.new_switch(human_exist_config)
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_human_exist_switch(s))
    if human_breath_config := config.get(CONF_HUMAN_BREATH):
        s = await switch.new_switch(human_breath_config)
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_human_breathe_switch(s))
    if human_heart_monitor_config := config.get(CONF_HUMAN_HEART_MONITOR):
        s = await switch.new_switch(human_heart_monitor_config)
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_human_heart_monitor_switch(s))
    if human_sleep_monitor_config := config.get(CONF_HUMAN_SLEEP_MONITOR):
        s = await switch.new_switch(human_sleep_monitor_config)
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_human_sleep_monitor_switch(s))
    if human_sleep_absence_config := config.get(CONF_HUMAN_SLEEP_ABSENCE):
        s = await switch.new_switch(human_sleep_absence_config)
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_human_sleep_absence_switch(s))
    if underlying_open_function_config := config.get(CONF_UNDERLYING_OPEN_FUNCTION):
        s = await switch.new_switch(underlying_open_function_config)
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_underlying_open_function_switch(s))