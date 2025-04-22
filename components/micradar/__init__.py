import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_MODEL

CODEOWNERS = ["@tlyang"]
DEPENDENCIES = ["uart"]
MULTI_CONF = True

micradar_ns = cg.esphome_ns.namespace("micradar")

MICRADARComponent = micradar_ns.class_(
    "MICRADARComponent", cg.Component, uart.UARTDevice
)

CONF_MICRADAR_ID = "micradar_id"

CONF_SUPPORTED_MODELS = {
    "R24DVD1": 0,
    "R60ABD1": 1,
    "R60AMP1": 2
}

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MICRADARComponent),
            cv.Required(CONF_MODEL): cv.string,
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "micradar",
    require_tx=True,
    require_rx=True,
    baud_rate=115200,
    parity="NONE",
    stop_bits=1,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_MODEL in config:
        if config[CONF_MODEL] not in list(CONF_SUPPORTED_MODELS.keys()):
            raise ValueError(f"Not support Model ID {config[CONF_MODEL]}")
        cg.add(var.set_model(config[CONF_MODEL]))