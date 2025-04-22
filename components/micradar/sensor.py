import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_HEIGHT,
    CONF_DISTANCE,
    CONF_RESOLUTION,
    CONF_SPEED,
    CONF_DISTANCE,
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_SPEED,
    ICON_COUNTER,
    ICON_HEART_PULSE,
    ICON_PULSE,
    ICON_SIGNAL,
    STATE_CLASS_MEASUREMENT,
    UNIT_BEATS_PER_MINUTE,
    UNIT_CENTIMETER,
    UNIT_METER,
    UNIT_MINUTE,
    UNIT_PERCENT
)

from . import CONF_MICRADAR_ID, MICRADARComponent

DEPENDENCIES = ["micradar"]

CONF_CUSTOM_PRESENCE_OF_DETECTION = "custom_presence_of_detection"
CONF_MOVEMENT_SIGNS = "movement_signs"
CONF_CUSTOM_MOTION_DISTANCE = "custom_motion_distance"
CONF_CUSTOM_SPATIAL_STATIC_VALUE = "custom_spatial_static_value"
CONF_CUSTOM_SPATIAL_MOTION_VALUE = "custom_spatial_motion_value"
CONF_CUSTOM_MOTION_SPEED = "custom_motion_speed"
CONF_CUSTOM_MODE_NUM = "custom_mode_num"
CONF_BREATHE_RATE = "breathe_rate"
CONF_HEARTBEAT_RATE = "heartbeat_rate"
CONF_NUM_TARGETS = "num_targets"
CONF_BED_STATUS = "bed_status"
CONF_WAKE_UP = "wake_up"
CONF_LIGHT_SLEEP = "light_sleep"
CONF_DEEP_SLEEP = "deep_sleep"
CONF_SLEEP_QUALITY = "sleep_quality"
CONF_X = "x"
CONF_Y = "y"
CONF_Z = "z"

ICON_ALPHA_X_BOX_OUTLINE = "mdi:alpha-x-box-outline"
ICON_ALPHA_Y_BOX_OUTLINE = "mdi:alpha-y-box-outline"
ICON_ALPHA_Z_BOX_OUTLINE = "mdi:alpha-z-box-outline"
ICON_RELATION_ZERO_OR_ONE_TO_ZERO_OR_ONE = "mdi:relation-zero-or-one-to-zero-or-one"
ICON_MAP_MARKER_DISTANCE = "mdi:map-marker-distance"
ICON_SPEEDOMETER_SLOW = "mdi:speedometer-slow"

MAX_TARGETS = 3

UNIT_CENTIMETER_PER_SECOND = "cm/s"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MICRADARComponent),

        cv.Optional(CONF_CUSTOM_PRESENCE_OF_DETECTION): sensor.sensor_schema(
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_METER,
            accuracy_decimals=2,  # Specify the number of decimal places
            icon="mdi:signal-distance-variant",
        ),
        cv.Optional(CONF_MOVEMENT_SIGNS): sensor.sensor_schema(
            icon="mdi:human-greeting-variant",
        ),
        cv.Optional(CONF_CUSTOM_MOTION_DISTANCE): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER,
            accuracy_decimals=2,
            icon="mdi:signal-distance-variant",
        ),
        cv.Optional(CONF_CUSTOM_SPATIAL_STATIC_VALUE): sensor.sensor_schema(
            device_class=DEVICE_CLASS_ENERGY,
            icon="mdi:counter",
        ),
        cv.Optional(CONF_CUSTOM_SPATIAL_MOTION_VALUE): sensor.sensor_schema(
            device_class=DEVICE_CLASS_ENERGY,
            icon="mdi:counter",
        ),
        cv.Optional(CONF_CUSTOM_MOTION_SPEED): sensor.sensor_schema(
            unit_of_measurement="m/s",
            device_class=DEVICE_CLASS_SPEED,
            accuracy_decimals=2,
            icon="mdi:run-fast",
        ),
        cv.Optional(CONF_CUSTOM_MODE_NUM): sensor.sensor_schema(
            icon="mdi:counter",
        ),
        cv.Optional(CONF_BREATHE_RATE): sensor.sensor_schema(
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            icon=ICON_PULSE,
        ),
        cv.Optional(CONF_HEARTBEAT_RATE): sensor.sensor_schema(
            accuracy_decimals=0,
            icon=ICON_HEART_PULSE,
            state_class=STATE_CLASS_MEASUREMENT,
            unit_of_measurement=UNIT_BEATS_PER_MINUTE,
        ),
        cv.Optional(CONF_DISTANCE): sensor.sensor_schema(
            device_class=DEVICE_CLASS_DISTANCE,
            state_class=STATE_CLASS_MEASUREMENT,
            unit_of_measurement=UNIT_CENTIMETER,
            accuracy_decimals=2,
            icon=ICON_SIGNAL,
        ),
        cv.Optional(CONF_NUM_TARGETS): sensor.sensor_schema(
            icon=ICON_COUNTER,
        ),
        cv.Optional(CONF_BED_STATUS): sensor.sensor_schema(
            icon="mdi:bed-empty",
        ),
        cv.Optional(CONF_WAKE_UP): sensor.sensor_schema(
            icon="mdi:bed-outline",
            unit_of_measurement=UNIT_MINUTE,
        ),
        cv.Optional(CONF_LIGHT_SLEEP): sensor.sensor_schema(
            icon="mdi:bed-clock",
            unit_of_measurement=UNIT_MINUTE,
        ),
        cv.Optional(CONF_DEEP_SLEEP): sensor.sensor_schema(
            icon="mdi:bed-clock",
            unit_of_measurement=UNIT_MINUTE,
        ),
        cv.Optional(CONF_SLEEP_QUALITY): sensor.sensor_schema(
            icon="mdi:bed",
            unit_of_measurement=UNIT_PERCENT,
        ),
        cv.Optional(CONF_X): sensor.sensor_schema(
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_CENTIMETER,
            icon=ICON_ALPHA_X_BOX_OUTLINE,
        ),
        cv.Optional(CONF_Y): sensor.sensor_schema(
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_CENTIMETER,
            icon=ICON_ALPHA_Y_BOX_OUTLINE,
        ),
        cv.Optional(CONF_Z): sensor.sensor_schema(
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_CENTIMETER,
            icon=ICON_ALPHA_Z_BOX_OUTLINE,
        ),
    }
)

CONFIG_SCHEMA = CONFIG_SCHEMA.extend(
    {
        cv.Optional(f"target_{n + 1}"): cv.Schema(
            {
                cv.Optional(CONF_X): sensor.sensor_schema(
                    device_class=DEVICE_CLASS_DISTANCE,
                    unit_of_measurement=UNIT_CENTIMETER,
                    icon=ICON_ALPHA_X_BOX_OUTLINE,
                ),
                cv.Optional(CONF_Y): sensor.sensor_schema(
                    device_class=DEVICE_CLASS_DISTANCE,
                    unit_of_measurement=UNIT_CENTIMETER,
                    icon=ICON_ALPHA_Y_BOX_OUTLINE,
                ),
                cv.Optional(CONF_SPEED): sensor.sensor_schema(
                    device_class=DEVICE_CLASS_SPEED,
                    unit_of_measurement=UNIT_CENTIMETER_PER_SECOND,
                    icon=ICON_SPEEDOMETER_SLOW,
                ),
                cv.Optional(CONF_HEIGHT): sensor.sensor_schema(
                    unit_of_measurement=UNIT_CENTIMETER,
                    icon=ICON_ALPHA_Z_BOX_OUTLINE,
                ),
                cv.Optional(CONF_DISTANCE): sensor.sensor_schema(
                    device_class=DEVICE_CLASS_DISTANCE,
                    unit_of_measurement=UNIT_CENTIMETER,
                    icon=ICON_MAP_MARKER_DISTANCE,
                ),
                cv.Optional(CONF_RESOLUTION): sensor.sensor_schema(
                    device_class=DEVICE_CLASS_DISTANCE,
                    unit_of_measurement=UNIT_CENTIMETER,
                    icon=ICON_RELATION_ZERO_OR_ONE_TO_ZERO_OR_ONE,
                ),
            }
        )
        for n in range(MAX_TARGETS)
    }
)

async def to_code(config):
    micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])
    if custompresenceofdetection_config := config.get(
        CONF_CUSTOM_PRESENCE_OF_DETECTION
    ):
        sens = await sensor.new_sensor(custompresenceofdetection_config)
        cg.add(micradar_component.set_custom_presence_of_detection_sensor(sens))
    if movementsigns_config := config.get(CONF_MOVEMENT_SIGNS):
        sens = await sensor.new_sensor(movementsigns_config)
        cg.add(micradar_component.set_movement_signs_sensor(sens))
    if custommotiondistance_config := config.get(CONF_CUSTOM_MOTION_DISTANCE):
        sens = await sensor.new_sensor(custommotiondistance_config)
        cg.add(micradar_component.set_custom_motion_distance_sensor(sens))
    if customspatialstaticvalue_config := config.get(CONF_CUSTOM_SPATIAL_STATIC_VALUE):
        sens = await sensor.new_sensor(customspatialstaticvalue_config)
        cg.add(micradar_component.set_custom_spatial_static_value_sensor(sens))
    if customspatialmotionvalue_config := config.get(CONF_CUSTOM_SPATIAL_MOTION_VALUE):
        sens = await sensor.new_sensor(customspatialmotionvalue_config)
        cg.add(micradar_component.set_custom_spatial_motion_value_sensor(sens))
    if custommotionspeed_config := config.get(CONF_CUSTOM_MOTION_SPEED):
        sens = await sensor.new_sensor(custommotionspeed_config)
        cg.add(micradar_component.set_custom_motion_speed_sensor(sens))
    if custommodenum_config := config.get(CONF_CUSTOM_MODE_NUM):
        sens = await sensor.new_sensor(custommodenum_config)
        cg.add(micradar_component.set_custom_mode_num_sensor(sens))
    if breathe_rate_config := config.get(CONF_BREATHE_RATE):
        sens = await sensor.new_sensor(breathe_rate_config)
        cg.add(micradar_component.set_breathe_rate_sensor(sens))
    if heartbeat_rate_config := config.get(CONF_HEARTBEAT_RATE):
        sens = await sensor.new_sensor(heartbeat_rate_config)
        cg.add(micradar_component.set_heartbeat_rate_sensor(sens))
    if distance_config := config.get(CONF_DISTANCE):
        sens = await sensor.new_sensor(distance_config)
        cg.add(micradar_component.set_distance_sensor(sens))
    if num_targets_config := config.get(CONF_NUM_TARGETS):
        sens = await sensor.new_sensor(num_targets_config)
        cg.add(micradar_component.set_num_targets_sensor(sens))
    if bed_status_config := config.get(CONF_BED_STATUS):
        sens = await sensor.new_sensor(bed_status_config)
        cg.add(micradar_component.set_bed_status_sensor(sens))
    if wake_up_config := config.get(CONF_WAKE_UP):
        sens = await sensor.new_sensor(wake_up_config)
        cg.add(micradar_component.set_wake_up_sensor(sens))
    if light_sleep_config := config.get(CONF_LIGHT_SLEEP):
        sens = await sensor.new_sensor(light_sleep_config)
        cg.add(micradar_component.set_light_sleep_sensor(sens))
    if deep_sleep_config := config.get(CONF_DEEP_SLEEP):
        sens = await sensor.new_sensor(deep_sleep_config)
        cg.add(micradar_component.set_deep_sleep_sensor(sens))
    if sleep_quality_config := config.get(CONF_SLEEP_QUALITY):
        sens = await sensor.new_sensor(sleep_quality_config)
        cg.add(micradar_component.set_sleep_quality_sensor(sens))
    if x_config := config.get(CONF_X):
        sens = await sensor.new_sensor(x_config)
        cg.add(micradar_component.set_move_x_sensor(sens))
    if y_config := config.get(CONF_Y):
        sens = await sensor.new_sensor(y_config)
        cg.add(micradar_component.set_move_y_sensor(sens))
    if z_config := config.get(CONF_Z):
        sens = await sensor.new_sensor(z_config)
        cg.add(micradar_component.set_move_z_sensor(sens))
    for n in range(MAX_TARGETS):
        if target_conf := config.get(f"target_{n + 1}"):
            if x_config := target_conf.get(CONF_X):
                sens = await sensor.new_sensor(x_config)
                cg.add(micradar_component.set_move_x_sensor(n, sens))
            if y_config := target_conf.get(CONF_Y):
                sens = await sensor.new_sensor(y_config)
                cg.add(micradar_component.set_move_y_sensor(n, sens))
            if speed_config := target_conf.get(CONF_SPEED):
                sens = await sensor.new_sensor(speed_config)
                cg.add(micradar_component.set_move_speed_sensor(n, sens))
            if height_config := target_conf.get(CONF_HEIGHT):
                sens = await sensor.new_sensor(height_config)
                cg.add(micradar_component.set_move_height_sensor(n, sens))
            if distance_config := target_conf.get(CONF_DISTANCE):
                sens = await sensor.new_sensor(distance_config)
                cg.add(micradar_component.set_move_distance_sensor(n, sens))
            if resolution_config := target_conf.get(CONF_RESOLUTION):
                sens = await sensor.new_sensor(resolution_config)
                cg.add(micradar_component.set_move_resolution_sensor(n, sens))