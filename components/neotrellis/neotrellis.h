#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/light/light_output.h"

#include "Adafruit_NeoTrellis.h"

namespace esphome {
namespace neotrellis {

static const char *const TAG = "neotrellis";
static Adafruit_NeoTrellis trellis;

class NeoTrellisKeyTrigger : public Trigger<uint16_t> {};

class NeoTrellis : public Component {
  public:
    void setup() override {
      trellis.begin();
      for (int i=0; i<NEO_TRELLIS_NUM_KEYS; i++) {
        trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
        trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
        trellis.registerCallback(i, process_key);
      }
    }

    void loop() override {
      trellis.read();
    }

    void dump_config() override {
      ESP_LOGCONFIG(TAG, "NeoTrellis:");
      ESP_LOGCONFIG(TAG, "  Buttons: %u", NEO_TRELLIS_NUM_KEYS);
    }

    void register_key_press_trigger(NeoTrellisKeyTrigger *trigger) {
      this->key_press_triggers_.push_back(trigger);
    }

    void register_key_release_trigger(NeoTrellisKeyTrigger *trigger) {
      this->key_release_triggers_.push_back(trigger);
    }

  protected:
    static std::vector<NeoTrellisKeyTrigger *> key_press_triggers_;
    static std::vector<NeoTrellisKeyTrigger *> key_release_triggers_;

    static TrellisCallback process_key(keyEvent ev) {
      if (ev.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING)
        for (auto *trigger : key_press_triggers_)
          trigger->trigger(ev.bit.NUM);
      if (ev.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING)
        for (auto *trigger : key_release_triggers_)
          trigger->trigger(ev.bit.NUM);
      return 0;
    }
};
std::vector<NeoTrellisKeyTrigger *> NeoTrellis::key_press_triggers_;
std::vector<NeoTrellisKeyTrigger *> NeoTrellis::key_release_triggers_;

class NeoTrellisRGBLightOutput : public light::LightOutput {
  public:
    void set_index(const uint16_t index) {
      if (index >= 0 && index < trellis.pixels.numPixels()) {
        this->index_ = index;
      } else {
        esph_log_e(TAG, "LED index is out of bounds, only (0,%d> is allowed!", trellis.pixels.numPixels());
      }
      trellis.begin();
    }

    light::LightTraits get_traits() override {
      auto traits = light::LightTraits();
      traits.set_supported_color_modes({light::ColorMode::RGB});
      return traits;
    }

    void write_state(light::LightState *state) override {
      float red, green, blue;
      state->current_values_as_rgb(&red, &green, &blue);
      uint32_t color = trellis.pixels.Color(255 * red, 255 * green, 255 * blue);
      trellis.pixels.setPixelColor(this->index_, color);
      trellis.pixels.show();
    }

  protected:
    uint16_t index_ = 0;
};

} // namespace neotrellis
} // namespace esphome
