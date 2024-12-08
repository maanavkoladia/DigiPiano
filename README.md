# DigiPiano

The **DigiPiano** is a velocity-sensitive digital piano designed using **STM32** and **ESP32** microcontrollers. It combines advanced sensor technology, high-quality audio playback, and Bluetooth connectivity to deliver a realistic and modern musical experience.

## Project Overview

- **Key Tracking Mechanism:**
  - Uses **Hall effect sensors** and magnets to detect key presses and releases.
  - Tracks key velocity to simulate the dynamics of a real piano.

- **Audio Playback:**
  - Plays pre-recorded piano note samples stored in external memory.
  - Outputs sound via AUX to speakers or streams wirelessly over Bluetooth.

- **Custom Sustain Circuitry:**
  - Includes a custom-designed sustain circuit to enhance the playing experience.

- **Communication Between Microcontrollers:**
  - The STM32 and ESP32 communicate efficiently using **SPI** and **UART** protocols, enabling seamless data exchange for key detection and audio output.

## Implementation Highlights

- The firmware handles multichannel analog data acquisition and processes it to detect key strokes with precision.
- Audio playback is managed through external memory, ensuring low-latency performance.
- Bluetooth functionality allows for wireless connectivity, enabling modern use cases such as streaming to Bluetooth speakers or devices.
- The hardware design incorporates a custom PCB layout for compactness and reliability, integrating sensors, microcontrollers, and audio components.

## See It in Action

Check out this [Instagram reel](https://www.instagram.com/reel/DDNmyurpquJ/?igsh=Y2NvNWVrb3MxazM1) to see the DigiPiano in action!

The DigiPiano is a versatile and user-friendly project, blending traditional piano functionality with modern digital and IoT capabilities.

