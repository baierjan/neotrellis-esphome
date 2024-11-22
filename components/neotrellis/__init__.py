import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_ON_PRESS, CONF_ON_RELEASE, CONF_TRIGGER_ID

CODEOWNERS = ["@baierjan"]

MULTI_CONF = 1

neotrellis_ns = cg.esphome_ns.namespace("neotrellis")

NeoTrellis = neotrellis_ns.class_(
    "NeoTrellis", cg.Component
)
NeoTrellisKeyTrigger = neotrellis_ns.class_(
    "NeoTrellisKeyTrigger", automation.Trigger.template(cg.uint16)
)

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(NeoTrellis),
        cv.Optional(CONF_ON_PRESS): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(NeoTrellisKeyTrigger),
            }
        ),
        cv.Optional(CONF_ON_RELEASE): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(NeoTrellisKeyTrigger),
            }
        ),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for conf in config.get(CONF_ON_PRESS, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
        cg.add(var.register_key_press_trigger(trigger))
        await automation.build_automation(trigger, [(cg.uint16, "x")], conf)

    for conf in config.get(CONF_ON_RELEASE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID])
        cg.add(var.register_key_release_trigger(trigger))
        await automation.build_automation(trigger, [(cg.uint16, "x")], conf)
