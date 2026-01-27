# Color Drop: An Interactive Hand-Responsive Letter Display Microcontroller Project

## Project Overview
This project is an interactive art experience that explores how a user’s presence and movements can dynamically alter a digital environment. Inspired by Camille Utterback’s *Text Rain* exhibit, the experience captures human hand movement and allows the user to interact with falling alphabet letters on a screen.

As letters fall, they bounce off the user’s hand and accumulate if the hand remains still. Moving the hand causes letters to fall off or scatter, creating playful and contemplative interactions. Users can also modify display parameters (such as letter color or physics behavior) using a gesture sensor.

This project is built on the **Northwestern Micro:bit v2 Base** starter code and targets embedded interaction using infrared sensing and a TFT LCD display.

---

## Project Goals
- Capture hand movement using an infrared sensor and visualize it in real time.
- Display falling alphabet letters that interact physically with the detected hand.
- Implement simple physics (collision, stacking, scattering).
- Allow gesture-based control over display parameters (e.g., color).
- Stretch goals:
  - Capture motion beyond the hand.
  - Create an intuitive UI requiring little to no verbal instruction.

---

## Hardware Components

### Input
- [SparkFun RGB and Gesture Sensor](https://www.sparkfun.com/sparkfun-rgb-and-gesture-sensor-apds-9960.html)  
- [SparkFun Grid-EYE Infrared Array Breakout – AMG8833 (Qwiic)](https://www.sparkfun.com/sparkfun-grid-eye-infrared-array-breakout-amg8833-qwiic.html)

### Output
- [TFT LCD Display Module (2.0", 240×320, SPI Interface)](https://www.sparkfun.com/tft-lcd-display-module-screen-2-0in-240x320-spi-interface.html)


### Platform
- Micro:bit v2

---

## Software Stack
- Northwestern Micro:bit v2 Base (CE346 starter code)
- nRF52 toolchain
- SPI drivers for TFT LCD display
- I²C drivers for Grid-EYE infrared sensor
- Gesture sensor libraries
- Custom physics logic for letter-hand interaction

---

## User Interaction
1. The user places their hand within the infrared sensor’s field of view.
2. Alphabet letters fall from the top of the screen.
3. Letters collide with the detected hand and accumulate when the hand is still.
4. Moving the hand causes letters to scatter or fall.
5. Gestures change visual or physics parameters (e.g., letter color or gravity).

---

## Repository Structure

This repository is built on the **Northwestern Micro:bit v2 Base**, which provides the core project foundation, including:

- Board initialization
- Standard application structure
- Build system integration via `nrf52x-base`

### Project-Specific Modifications

The following directories contain code added or modified for this project:

- `software/app/gesture-code` — gesture input handling
- `software/app/project-code` — main project application logic


---

## Requirements

### Tools
- `build-essential` (make)
- `git`
- `python3`
- `pyserial`
  - Ubuntu: `sudo apt install python3-serial`
  - macOS: `pipx install pyserial`

### ARM Cross-Compiler Toolchain
- GCC ARM Embedded Toolchain  
  https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

**Ubuntu:**
```bash
sudo apt install gcc-arm-none-eabi
