# ESPHome component for NeoTrellis

## Description

Have something as a control panel for HomeAssistant. The idea is to have a hardware panel with buttons and lights to be able to execute simple actions easily and to have a feedback about states.

## Goals

* G1: Have a simple board with buttons and lights
* G2: Buttons will be able to start some predefined action / automation
* G3: Lights will be able to show states from the HomeAssistant
* G4: The hardware is hidden in some nice compact box

## The Implementation Story

### Chapter 1: The Hardware

I would like to have something like StreamDeck or better LocalDeck. It needs to be fully configurable, it has to be usable without connecting to a computer (so not "just a special keyboard") and I would prefer to configure it through the standard software stack I already have (e.g. using through ESPHome).

After looking through some e-shops, I was able to find Adafruit NeoTrellis which could be paired with ESP / Raspberry Pico as a micro-controller. Selecting proper ESP module (instead of the available prebuild RPi) would allow me to use ESPHome for the software part. On top of that, it could be put into nice 3D printed box.

The wiring diagram to connect the NeoTrellis with my ESP-WROOM-32 is pretty trivial, I just need to connect the I2C pins and that's it, G1 completed.

### Chapter 2: ESPHome

I decided to skip the Arduino IDE step and decided to try to integrate it into ESPHome right away. It turned out, that the hardest part is to find out the correct library as there is so much stuff published from Adafruit and this board has several variants. After some trial and error I was able to identify the correct dependencies and was able to compile, flash and boot a code which can light up an LED. The critical code from my yaml file is:

    esphome:
      name: ${devicename}
      libraries:
        - WIRE
        - SPI
        - adafruit/Adafruit BusIO
        - adafruit/Adafruit seesaw Library
      on_boot:
        then:
        - lambda: |-
            static Adafruit_NeoTrellis trellis;
            trellis.begin();
            trellis.pixels.setPixelColor(2, 0xff0000);
            trellis.pixels.show();

Now it is time to wrap the code to either a custom component or even better to an external component to be able to write the code without many lambdas.

### Chapter 3: External component

I skipped the custom component right away as that is already deprecated and will be removed soon. Instead I went straight to the creation of the external component. After some more trial and error I was able to write down some code which can compile itself. Now I have a working basic component which can be easily used to define a simple RGB light.

    light:
      - platform: neotrellis
        name: Button 4
        index: 4

This can be already extended with some automation to mirror a state of some HomeAssistant component. For example to show the status of ceiling light, one need to add the following code. This also completes G3.

    external_components:
        - source: github://baierjan/neotrellis-esphome
    light:
      - platform: neotrellis
        id: ceiling_light_status
        index: 1
    switch:
      - platform: homeassistant
        id: ceiling_light
        entity_id: light.ceiling_light
        on_turn_on:
          then:
            - light.turn_on:
                id: ceiling_light_status
        on_turn_off:
          then:
            - light.turn_off:
                id: ceiling_light_status

It turned out I need to inspire myself in the matrix keyboard component and make use of callbacks which are already implemented in the original library. That will make my live much easier. The code to satisfy G2 is ready.

    external_components:
      - source: github://baierjan/neotrellis-esphome
    light:
      - platform: neotrellis
        id: status
        index: 7
    neotrellis:
      - id: neotrellis_keyboard
        on_press:
          - lambda: |-
              ESP_LOGI("KEY", "received key %d", x);
              if (x==7)
                id(status).toggle().perform();
        on_release:
          - lambda: ESP_LOGI("KEY", "released key %d", x);

## Resources

* https://hackweek.opensuse.org/24/projects/customizable-control-panel-for-home-assistant
* https://learn.adafruit.com/adafruit-neotrellis/overview
* https://esphome.io/components/external_components
* https://registry.platformio.org/libraries/adafruit/Adafruit%20seesaw%20Library

## Known issues

* Interrupt pin is not (yet?) supported.
* There is no support for "MultiTrellis", but that should not be difficult to implement in the future.
