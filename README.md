# Critter Connect 2 - Wearable GPS-based Animal Interaction Device

**Critter Connect 2** is an innovative wearable device designed to interact with the user’s environment through GPS technology. The device uses GPS coordinates to determine the user's location and identifies the nearest *Point of Interest* (POI) based on these coordinates. When the user is within 100 meters of a POI, the wearable lights up with a color corresponding to the animal species typically found in that area, creating an engaging and interactive experience.

Additionally, the device allows the user to play the sound of the animal associated with the POI by pressing a button, enhancing the immersive experience. If the user is not within the *Zone of Interest* (ZOI), the device remains inactive—no lights or sounds are triggered.

## Features

- **Location Awareness**: The device uses GPS to constantly update the user’s position and compare it with predefined Points of Interest (POIs).
- **Animal Sounds**: When the user enters the ZOI, they can press a button to trigger the sound of the animal typically found in that area.
- **LED Indicator**: The device lights up when within 100 meters of a POI to indicate the presence of an animal-related location.
- **Energy Efficiency**: The device conserves energy by remaining inactive when outside the ZOI, not playing sounds or lighting up. This ensures longer battery life for the wearable device.

## Circuit Design and Assembly

The system's hardware consists of four major components, which work together to provide the core functionality:

1. **Microcontroller (Adafruit Feather RP2040)**: The heart of the system, controlling all components, processing GPS data, and managing user inputs.
2. **GNSS Module (L76K GNSS Module)**: A GPS receiver that provides real-time location data to the microcontroller.
3. **MP3 Module (DFPlayer Pro)**: Plays audio files of animal sounds when triggered by the user. It interfaces with a push button and speaker for the sound output.
4. **Speaker & Button**: The user presses the button to play the sound of the animal when within the ZOI. The speaker emits the animal sound to enhance the experience.
5. **Motor Driver (Adafruit TB6612 1.2A DC/Stepper Motor Driver)**: Used to control a motor for additional features, such as haptic feedback or a mechanical part within the wearable device.

## Components

### Microcontroller

The **Adafruit Feather RP2040** is a powerful and compact microcontroller that handles communication between the GNSS module, MP3 module, and other components. It is designed for efficient processing and low power consumption, making it ideal for wearable devices.

<img src="https://cdn-learn.adafruit.com/assets/assets/000/100/345/large1024/adafruit_products_FeatherRP_top_angle.jpg?1614788869" width=50% height=50%>

#### Features:
- Dual-core ARM Cortex M0+ processor
- 264KB RAM and 8MB Flash storage
- Multiple GPIO pins for interfacing with sensors, buttons, and modules
- Low power consumption for battery efficiency

---

### GNSS Module

The **L76K GNSS Module** is used to obtain the user’s geographic coordinates. This module communicates with the microcontroller, sending updated location data. The precise GPS data is then used to calculate the proximity to Points of Interest (POIs).

<img src="https://files.seeedstudio.com/wiki/Seeeduino-XIAO-Expansion-Board/GPS_Module/L76K/1-L76K-GNSS-Module-for-Seeed-Studio-XIAO-45font.jpg" width=50% height=50%>

#### Key Features:
- Provides accurate latitude and longitude coordinates
- Fast satellite acquisition for real-time location tracking
- Low power consumption for portable devices

---

### MP3 Module

The **DFPlayer Pro** is responsible for playing MP3 files of animal sounds. When the user presses the push button and is within the ZOI, the system triggers the audio playback, providing an immersive animal sound experience.

<img src="https://dfimg.dfrobot.com/store/data/DFR0768/DFR0768.jpg" width=50% height=50%>

#### MP3 Player Features:
- Supports microSD cards to store MP3 audio files
- Audio output to speakers for playback
- Low power consumption to suit portable devices
- Button interface for user interaction

---

### Speaker & Button

The **Adafruit Mini Oval Speaker** is used for playing the animal sounds, and the **Adafruit Mini On/Off Push Button** allows the user to activate the sound playback. These components provide reliable and compact interaction for the wearable device.

#### Speaker

<img src="https://cdn-shop.adafruit.com/970x728/3923-02.jpg" width=50% height=50%>

- Compact 8Ω speaker
- High-quality audio output for clear animal sounds
- Low power consumption for efficiency

#### Button

<img src="https://cdn-shop.adafruit.com/970x728/3870-00.jpg" width=50% height=50%>

- Small and durable push button
- Used to trigger sound playback
- Easy to integrate into wearable designs

---

### Motor Driver

The **Adafruit TB6612 1.2A DC/Stepper Motor Driver** is a motor driver module used to control motors for additional features, such as haptic feedback, vibration, or moving parts in the wearable device.

<img src="https://cdn-shop.adafruit.com/970x728/2448-06.jpg" width=50% height=50%>

#### Features:
- Drives DC and stepper motors
- Can control 2 DC motors or 1 stepper motor
- 1.2A per channel motor drive
- Low power consumption and compact size for portable use

---

## Assembly

The components are connected as follows:

1. The **Adafruit Feather RP2040** microcontroller is the central unit and interfaces with all components.
2. The **L76K GNSS Module** is connected to the microcontroller via UART or I2C communication to provide location data.
3. The **DFPlayer Pro** is connected to the microcontroller and the Adafruit Mini Oval Speaker, providing audio playback.
4. The **Adafruit Mini On/Off Push Button** is wired to the microcontroller for user input, triggering the animal sound when pressed.
5. The **Adafruit TB6612 Motor Driver** is connected to control a motor for any mechanical movements or haptic feedback required in the wearable device.

The device is designed to be compact and wearable, with all components carefully integrated into a lightweight form factor.

## Power Supply

The device can be powered using a **LiPo battery**, which can be charged via the **Adafruit Feather RP2040's built-in charging circuit**. This allows for easy recharging and ensures that the device remains portable and efficient.

---

## Software

The firmware for Critter Connect 2 is written in **Arduino** and is compatible with the **Arduino IDE**. The software handles:

- GPS data acquisition and location tracking
- POI proximity detection and LED indication
- Button input for triggering animal sound playback
- Efficient power management to ensure long battery life
- Motor control using the **Adafruit TB6612 Motor Driver** for additional features

### Libraries Used

- **TinyGPSPlus**: For interfacing with the L76K GNSS module to fetch GPS data.
- **DFRobot_DF1201S**: For controlling the DFPlayer Pro module to play MP3 files.
- **Adafruit_NeoPixel**: For controlling RGB LEDs to indicate proximity to POIs.
- **SoftwareSerial**: For serial communication with the GNSS module and DFPlayer Pro module (if needed).
- **HardwareSerial**: For using hardware serial ports to communicate with modules like the GPS.
- **Wire**: For I2C communication, if any sensors or modules require it.
- **Stepper**: For controlling the motor via a stepper driver.
- **TB6612**: For controlling the motor through the Adafruit TB6612 Motor Driver.

You can install these libraries directly through the Arduino IDE Library Manager by going to **Sketch > Include Library > Manage Libraries...**, and then searching for the library by name.

---

## Getting Started

To get started with Critter Connect 2:

1. Clone this repository to your local machine.
2. Install the necessary libraries for **Arduino IDE**, including libraries for GPS, MP3 playback, and button handling.
3. Open the **CritterConnect2.ino** file in the Arduino IDE.
4. Select the **Adafruit Feather RP2040** as the board in the **Tools > Board** menu.
5. Connect the **Adafruit Feather RP2040** to your computer via USB.
6. Upload the code to the board.
7. Connect the components as described in the **Assembly** section.
8. Power on the device and start exploring the Points of Interest around you!

---

## Acknowledgements

- **Adafruit** for providing the Feather RP2040 and other excellent components.
- **DFRobot** for the DFPlayer Pro MP3 module.
- **Seeed Studio** for the L76K GNSS module.
- **Adafruit** for the TB6612 1.2A DC/Stepper Motor Driver.
