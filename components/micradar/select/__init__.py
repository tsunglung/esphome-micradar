import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
)
from .. import CONF_MICRADAR_ID, MICRADARComponent, micradar_ns

SceneModeSelect = micradar_ns.class_("SceneModeSelect", select.Select)
UnmanTimeSelect = micradar_ns.class_("UnmanTimeSelect", select.Select)
ExistenceBoundarySelect = micradar_ns.class_("ExistenceBoundarySelect", select.Select)
MotionBoundarySelect = micradar_ns.class_("MotionBoundarySelect", select.Select)
SleepRollOverSelect = micradar_ns.class_("SleepRollOverSelect", select.Select)

CONF_SCENE_MODE = "scene_mode"
CONF_UNMAN_TIME = "unman_time"
CONF_EXISTENCE_BOUNDARY = "existence_boundary"
CONF_MOTION_BOUNDARY = "motion_boundary"
CONF_SLEEP_ROLL_OVER = "sleep_roll_over"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_MICRADAR_ID): cv.use_id(MICRADARComponent),
    cv.Optional(CONF_SCENE_MODE): select.select_schema(
        SceneModeSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:hoop-house",
    ),
    cv.Optional(CONF_UNMAN_TIME): select.select_schema(
        UnmanTimeSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon="mdi:timeline-clock",
    ),
    cv.Optional(CONF_EXISTENCE_BOUNDARY): select.select_schema(
        ExistenceBoundarySelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ),
    cv.Optional(CONF_MOTION_BOUNDARY): select.select_schema(
        MotionBoundarySelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ),
    cv.Optional(CONF_SLEEP_ROLL_OVER): select.select_schema(
        SleepRollOverSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ),
}


async def to_code(config):
    micradar_component = await cg.get_variable(config[CONF_MICRADAR_ID])
    if scenemode_config := config.get(CONF_SCENE_MODE):
        s = await select.new_select(
            scenemode_config,
            options=["None", "Living Room", "Bedroom", "Washroom", "Area Detection"],
        )
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_scene_mode_select(s))
    if unmantime_config := config.get(CONF_UNMAN_TIME):
        s = await select.new_select(
            unmantime_config,
            options=[
                "None",
                "10s",
                "30s",
                "1min",
                "2min",
                "5min",
                "10min",
                "30min",
                "60min",
            ],
        )
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_unman_time_select(s))
    if existence_boundary_config := config.get(CONF_EXISTENCE_BOUNDARY):
        s = await select.new_select(
            existence_boundary_config,
            options=[
                "0.5m",
                "1.0m",
                "1.5m",
                "2.0m",
                "2.5m",
                "3.0m",
                "3.5m",
                "4.0m",
                "4.5m",
                "5.0m",
            ],
        )
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_existence_boundary_select(s))
    if motion_boundary_config := config.get(CONF_MOTION_BOUNDARY):
        s = await select.new_select(
            motion_boundary_config,
            options=[
                "0.5m",
                "1.0m",
                "1.5m",
                "2.0m",
                "2.5m",
                "3.0m",
                "3.5m",
                "4.0m",
                "4.5m",
                "5.0m",
            ],
        )
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_motion_boundary_select(s))
    if sleep_roll_over_config := config.get(CONF_SLEEP_ROLL_OVER):
        s = await select.new_select(
            sleep_roll_over_config,
            options=[
                "Low",
                "Middle",
                "High",
            ],
        )
        await cg.register_parented(s, config[CONF_MICRADAR_ID])
        cg.add(micradar_component.set_sleep_roll_over_select(s))

