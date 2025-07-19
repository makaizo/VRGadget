# VRGadget
A PlatformIO-based controller designed to receive commands from a VR device over MQTT and control an edge device. It also supports direct operation through onboard button inputs.

## Features

- Haptic feedback features to enhance the immersive experience in VR:
    - Heating control with a Peltier module (start/finish heating)
    - Cooling control with a Peltier module (start/finish cooling)
    - Splash control with a ultrasonic mist generator module (start/finish splash)
  - :information_source: Usage:
    - You can combine heating/cooling and splash.  
    e.g. if you call heating command and splash command, the gadget will heat and splash at the same time.
    - You cannot use heating and cooling at the same time.  
    e.g. if you call heating command and later, cooling command, the gadget will cancel heating and start cooling.

## Project Structure

- `src/` - Main application code
- `lib/` - Custom libraries (AtomMotion, MQTTClient, CredentialHandler)
- `data/` - Configuration files and credentials

## Setup

1. Copy `data/credentials.example.json` to `data/credentials.json`
2. Configure your WiFi and MQTT settings in the credentials file
3. Build and upload using PlatformIO

## Hardware

Compatible with ESP32-based devices and AtomMotion hardware.
