# Critter Connect 2

**Critter Connect 2** is an innovative wearable device designed to interact with the user’s environment through GPS technology. The device uses GPS coordinates to determine the user's location and identifies the nearest *Point of Interest* (POI) based on these coordinates. When the user is within 100 meters of a POI, the wearable lights up with a color corresponding to the animal species typically found in that area, creating an engaging experience.

Additionally, the device allows the user to play the sound of the animal associated with the POI by pressing a button, enhancing the immersive experience. If the user is not within the *Zone of Interest* (ZOI), the device remains inactive—no lights or sounds are triggered.

---

## Features

- **Location Awareness**: The device uses GPS to constantly update the user’s position and compare it with predefined Points of Interest (POIs).
- **Animal Sounds**: When the user enters the ZOI, they can press a button to trigger the sound of the animal typically found in that area.
- **LED Indicator**: The device lights up when within 100 meters of a POI.
- **Energy Efficiency**: The device conserves energy by remaining inactive when outside the ZOI, not playing sounds or lighting up.

---

## Circuit Design and Assembly

The system's hardware consists of four major components, which work together to provide the core functionality:

1. **Microcontroller (Adafruit ESP32C6)**: The heart of the system, controlling all components, processing GPS data, and managing user inputs.
2. **GNSS Module (L76K GNSS Module)**: A GPS receiver that provides real-time location data to the microcontroller.
3. **MP3 Module (DFPlayer Pro)**: Plays audio files of animal sounds when triggered by the user. It interfaces with a capacitive button and speaker for the sound output.
4. **Speaker & Button**: The user presses the button to play the sound of the animal when within the ZOI. The speaker emits the animal sound to enhance the experience.

---

### Microcontroller

The **Adafruit ESP32C6** is a powerful and compact microcontroller that handles communication between the GNSS module, MP3 module, and other components. It is designed for efficient processing and low power consumption, making it ideal for wearable devices.

![Adafruit ESP32C6](https://cdn-learn.adafruit.com/assets/assets/000/127/036/large1024/Adafruit_Feather_ESP32C6_Express_-_Feather_-_Top.jpg?1672743144)

- **Microcontroller Features**:
  - Compact and low-power design
  - Integrated Wi-Fi 6 and Bluetooth for future network capabilities
  - GPIO pins for connecting external components like buttons, LEDs, and sensors
  - Low-latency and high-speed processing

---

### GNSS Module

The **L76K GNSS Module** is used to obtain the user’s geographic coordinates. This module communicates with the microcontroller, sending updated location data. The precise GPS data is then used to calculate the proximity to Points of Interest (POIs).

![L76K GNSS Module](https://files.seeedstudio.com/wiki/Seeeduino-XIAO-Expansion-Board/GPS_Module/L76K/1-L76K-GNSS-Module-for-Seeed-Studio-XIAO-45font.jpg)

- **Key Features**:
  - Provides accurate latitude and longitude coordinates
  - Fast satellite acquisition for real-time location tracking
  - Low power consumption for portable devices

---

### MP3 Module

The **DFPlayer Pro** is responsible for playing MP3 files of animal sounds. When the user presses the capacitive button and is within the ZOI, the system triggers the audio playback, providing an immersive animal sound experience.

![DFPlayer Pro](https://dfimg.dfrobot.com/store/data/DFR0768/DFR0768.jpg)

- **MP3 Player Features**:
  - Supports microSD cards to store MP3 audio files
  - Audio output to speakers for playback
  - Low power consumption to suit portable devices
  - Button interface for user interaction

---

### Speaker & Button

The **speaker** connected to the MP3 module plays the sounds, while the **capacitive button** allows the user to activate the sound playback. The speaker can be any compatible speaker connected to the module, ensuring flexibility in choosing the right speaker for your needs.

- **Speaker Features**:
  - Can be any standard speaker that supports audio output from the MP3 module
  - Compact and energy-efficient to match the wearable device's design
- **Button**:
  - Capacitive touch button for easy and reliable user interaction

---

## Assembly

The components are connected as follows:

1. The **Adafruit ESP32C6** microcontroller is the central unit and interfaces with all components.
2. The **L76K GNSS Module** is connected to the microcontroller via UART or I2C communication to provide location data.
3. The **DFPlayer Pro** is connected to the microcontroller and the speaker, providing audio playback.
4. The **capacitive button** is wired to the microcontroller for user input, triggering the animal sound when pressed.

The device is designed to be compact and wearable, with all components carefully integrated into a lightweight form factor.
