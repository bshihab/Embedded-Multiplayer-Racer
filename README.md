# Embedded Multiplayer Racer (Cortex-M0+)

> Voted "Best Game" in ECE319K Class Competition (Fall 2025)

A multiplayer racing game engine written in C for the TI MSPM0G3507 microcontroller. This project contains real-time physics, sprite rendering, and UART communication between two microcontrollers.

[![Watch the Demo](https://img.youtube.com/vi/CwQ1BzuiUfw/0.jpg)](https://www.youtube.com/watch?v=CwQ1BzuiUfw)
*Click the image above to watch the gameplay demo.*

## Features

* **Multiplayer:** UART-based protocol handling data packets for position syncing and item events (Banana/Bomb).
* **Physics Engine:** Custom collision detection (AABB), velocity and breaking mechanics, and bounce mechanics.
* **Hardware Drivers:** Direct register-level drivers for:
    * **UART:** Interrupt-driven networking.
    * **SPI:** display communication.
    * **ADC:** Joystick input processing.
    * **DAC/Timer:** Sound generation and game loop timing (30Hz).

## Hardware Requirements

* **Microcontroller:** TI MSPM0G3507 LaunchPad.
* **Display:** ST7735 128x160 TFT LCD.
* **Input:** Analog Joystick (2-axis) + Buttons.
* **Audio:** DAC made with resistors + amplifier.

## Build & Run

1.  Clone the repo:
    ```bash
    git clone https://github.com/bshihab/Embedded-Multiplayer-Racer.git
    ```
2.  Import the project folder into **Code Composer Studio**.
3.  Build and Flash to the LaunchPad.
4.  **Multiplayer:** Connect two boards via UART (Tx to Rx, Rx to Tx, GND to GND).

## Project Structure

* `/images`: Raw sprite data arrays converted from BMPs.
* `/sounds`: Audio data arrays for game sound effects.
* `Lab9Main.c`: Main game loop and state machine.
* `Graphics.c/h`: LCD rendering logic.
* `GameObjects.c/h`:  sprite management and collision logic.

## License

Developed by **Bilal Shihab** and **Prarthana Balaji** for ECE319K at UT Austin.
