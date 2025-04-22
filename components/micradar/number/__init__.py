import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_SENSITIVITY,
    ENTITY_CATEGORY_CONFIG,
    UNIT_PULSES_PER_MINUTE,
    UNIT_MINUTE
)
from .. import CONF_MICRADAR_ID, MICRADARComponent, micradar_ns

SensitivityNumber = micradar_ns.class_("SensitivityNumber", number.Number)
CustomModeNumber = micradar_ns.class_("CustomModeNumber", number.Number)
ExistenceThresholdNumber = micradar_ns.class_("ExistenceThresholdNumber", number.Number)
MotionThresholdNumber = micradar_ns.class_("MotionThresholdNumber", number.Number)
MotionTriggerTimeNumber = micradar_ns.class_("MotionTriggerTimeNumber", number.Number)
MotionToRestTimeNumber = micradar_ns.class_("MotionToRestTimeNumber", number.Number)
CustomUnmanTimeNumber = micradar_ns.class_("CustomUnmanTimeNumber", number.Number)
BreatheSlowLevelNumber = micradar_ns.class_("BreatheSlowLevelNumber", number.Number)
SleepAbsenceTimeNumber = micradar_ns.class_("SleepAbsenceTimeNumber", number.Number)
SleepEndTimeNumber = micradar_ns.class_("SleepEndTimeNumber", number.Number)

CONF_CUSTOM_MODE = "custom_mode"
CONF_EXISTENCE_THRESHOLD = "existence_threshold"
CONF_MOTION_THRESHOLD = "motion_threshold"
CONF_MOTION_TRIGGER = "motion_trigger"
CONF_MOTION_TO_REST = "motion_to_rest"
CONF_CUSTOM_UNMAN_TIME = "custom_unman_time"
CONF_BREATHE_SLOW_LEVEL = "breathe_slow_level"
CONF_SLEEP_ABSENCE_TIME = "sleep_absence_time"
CONF_SLEEP_END_TIME = "sleep_end_time"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MICRADARComponent),
        cv.Optional(CONF_SENSITIVITY): number.number_schema(
            SensitivityNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:archive-check-outline",
        ),
        cv.Optional(CONF_CUSTOM_MODE): number.number_schema(
            CustomModeNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:cog",
        ),
        cv.Optional(CONF_EXISTENCE_THRESHOLD): number.number_schema(
            ExistenceThresholdNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_MOTION_THRESHOLD): number.number_schema(
            MotionThresholdNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_MOTION_TRIGGER): number.number_schema(
            MotionTriggerTimeNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:camera-timer",
            unit_of_measurement="ms",
        ),
        cv.Optional(CONF_MOTION_TO_REST): number.number_schema(
            MotionToRestTimeNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:camera-timer",
            unit_of_measurement="ms",
        ),
        cv.Optional(CONF_CUSTOM_UNMAN_TIME): number.number_schema(
            CustomUnmanTimeNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:camera-timer",
            unit_of_measurement="s",
        ),
        cv.Optional(CONF_BREATHE_SLOW_LEVEL): number.number_schema(
            BreatheSlowLevelNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:camera-timer",
            unit_of_measurement=UNIT_PULSES_PER_MINUTE,
        ),
        cv.Optional(CONF_SLEEP_ABSENCE_TIME): number.number_schema(
            SleepAbsenceTimeNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:camera-timer",
            unit_of_measurement=UNIT_MINUTE,
        ),
        cv.Optional(CONF_SLEEP_END_TIME): number.number_schema(
            SleepEndTimeNumber,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon="mdi:camera-timer",
            unit_of_measurement=UNIT_MINUTE,
        ),
    }
)


async def to_code(config):
    micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])
    if sensitivity_config := config.get(CONF_SENSITIVITY):
        n = await number.new_number(
            sensitivity_config,
            min_value=0,
            max_value=3,
            step=1,
        )
        await cg.register_parented(n, micradar_component)
        cg.add(micradar_component.set_sensitivity_number(n))
    if custom_mode_config := config.get(CONF_CUSTOM_MODE):
        n = await number.new_number(
            custom_mode_config,
            min_value=0,
            max_value=4,
            step=1,
        )
        await cg.register_parented(n, micradar_component)
        cg.add(micradar_component.set_custom_mode_number(n))
    if existence_threshold_config := config.get(CONF_EXISTENCE_THRESHOLD):
        n = await number.new_number(
            existence_threshold_config,
            min_value=0,
            max_value=250,
            step=1,
        )
        await cg.register_parented(n, micradar_component)
        cg.add(micradar_component.set_existence_threshold_number(n))
    if motion_threshold_config := config.get(CONF_MOTION_THRESHOLD):
        n = await number.new_number(
            motion_threshold_config,
            min_value=0,
            max_value=250,
            step=1,
        )
        await cg.register_parented(n, micradar_component)
        cg.add(micradar_component.set_motion_threshold_number(n))
    if motion_trigger_config := config.get(CONF_MOTION_TRIGGER):
        n = await number.new_number(
            motion_trigger_config,
            min_value=0,
            max_value=150,
            step=1,
        )
        await cg.register_parented(n, micradar_component)
        cg.add(micradar_component.set_motion_trigger_number(n))
    if motion_to_rest_config := config.get(CONF_MOTION_TO_REST):
        n = await number.new_number(
            motion_to_rest_config,
            min_value=0,
            max_value=3000,
            step=1,
        )
        await cg.register_parented(n, micradar_component)
        cg.add(micradar_component.set_motion_to_rest_number(n))
    if custom_unman_time_config := config.get(CONF_CUSTOM_UNMAN_TIME):
        n = await number.new_number(
            custom_unman_time_config,
            min_value=0,
            max_value=3600,
            step=1,
        )
        await cg.register_parented(n, micradar_component)
        cg.add(micradar_component.set_custom_unman_time_number(n))
    if breathe_slow_level_config := config.get(CONF_BREATHE_SLOW_LEVEL):
        n = await number.new_number(
            breathe_slow_level_config,
            min_value=10,
            max_value=20,
            step=1,
        )
        await cg.register_parented(n, micradar_component)
        cg.add(micradar_component.breathe_slow_level_number(n))
    if sleep_absence_time_config := config.get(CONF_SLEEP_ABSENCE_TIME):
        n = await number.new_number(
            sleep_absence_time_config,
            min_value=30,
            max_value=180,
            step=1,
        )
        await cg.register_parented(n, micradar_component)
        cg.add(micradar_component.sleep_absence_time_number(n))
    if sleep_end_time_config := config.get(CONF_SLEEP_END_TIME):
        n = await number.new_number(
            sleep_end_time_config,
            min_value=5,
            max_value=120,
            step=1,
        )
        await cg.register_parented(n, micradar_component)
        cg.add(micradar_component.sleep_end_time_number(n))

