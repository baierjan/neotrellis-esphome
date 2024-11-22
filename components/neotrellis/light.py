import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_ID, CONF_OUTPUT_ID, CONF_INDEX
from . import neotrellis_ns

NeoTrellisRGBLightOutput = neotrellis_ns.class_(
    "NeoTrellisRGBLightOutput", light.LightOutput
)

CONFIG_SCHEMA = light.RGB_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(NeoTrellisRGBLightOutput),
        cv.Optional(CONF_INDEX, default=0): cv.int_range(min=0, max=4*4*4),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await light.register_light(var, config)
    cg.add(var.set_index(config[CONF_INDEX]))
    cg.add_library("WIRE", None)
    cg.add_library("SPI", None)
    cg.add_library("adafruit/Adafruit BusIO", "1.16")
    cg.add_library("adafruit/Adafruit seesaw Library", "1.7")
