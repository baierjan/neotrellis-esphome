#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"

#include "Adafruit_NeoTrellis.h"

namespace esphome {
namespace neotrellis {

static const char *const TAG = "neotrellis";
static Adafruit_NeoTrellis trellis;

class NeoTrellisRGBLightOutput : public light::LightOutput {
  public:
    void set_index(const uint16_t index) {
      if (index >= 0 && index < trellis.pixels.numPixels()) {
        this->index = index;
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
      trellis.pixels.setPixelColor(this->index, color);
      trellis.pixels.show();
    }

  protected:
    uint16_t index = 0;
};

} // namespace neotrellis
} // namespace esphome
