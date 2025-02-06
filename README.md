# Critter Connect 2

The project involves creating a wearable device equipped with *GPS* functionality. This device continuously accesses *GPS* coordinates to determine the user's location. Using these coordinates, it identifies the nearest predefined *Point of Interest* (POI). If this *POI* is within 100 meters of the user, the wearable lights up with a color associated with the animal species typical to that area.

## Features
- It allows playing the sound of the animal if the user is within 100 meters from the *POI* by pressing the button.
- If the user is outside the *Zone of Interest* (ZOI), the lights do not light up, and the *MP3* does not play any sound.

---

## Circuit Design and Assembly

The circuit consists of four main components:

1. **Microcontroller** – Oversees the operation of all other components.
2. **GNSS Module** – Determines the user’s location using GPS coordinates.
3. **MP3 Module** – Plays animal sounds when inside the ZOI, using a speaker and a button for user interaction.

---

### Microcontroller

The core of the system is the **Seeed Studio XIAO ESP32C3**, which manages the other components.

![XIAO ESP32-C3](https://media-cdn.seeedstudio.com/media/catalog/product/cache/bb49d3ec4ee05b6f018e93f896b8a25d/1/-/1-113991054-seeed-studio-xiao-esp32c3-45font_1.jpg =250x)

### GNSS Module

The **L76K GNSS Module** is responsible for determining the user's location and communicating it to the microcontroller.

![XIAO GNSS module](https://files.seeedstudio.com/wiki/Seeeduino-XIAO-Expansion-Board/GPS_Module/L76K/1-L76K-GNSS-Module-for-Seeed-Studio-XIAO-45font.jpg =250x)

### MP3 Module

The **DFPlayer Pro** is responsible for playing the animal sounds when triggered. The capacitive button and speaker complete the system for sound playback.

![DFROBOT MP3 module](https://dfimg.dfrobot.com/store/data/DFR0768/DFR0768.jpg =250x)

The speaker can be any compatible speaker connected to the *MP3* module.
