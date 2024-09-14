# Escape 079: FPGA Arcade-Style Game

## Table of Contents
1. [Project Overview](#project-overview)
2. [Key Features](#key-features)
3. [Technical Details](#technical-details)
4. [Installation and Setup](#installation-and-setup)
5. [Usage Instructions](#usage-instructions)
6. [Examples and Demos](#examples-and-demos)
7. [Key Contributions](#key-contributions)
8. [Licensing](#licensing)
9. [Contact Information](#contact-information)
10. [Acknowledgments](#acknowledgments)
11. [Future Improvements](#future-improvements)
12. [Challenges and Solutions](#challenges-and-solutions)

## Project Overview

Escape 079 is a two-player arcade-style game designed for the Intel DE1-SoC FPGA board and implemented in C. This project aims to demonstrate the integration of fundamental Computer Organization concepts, including the use of pooling, reading/writing into status/control registers, and interacting with various I/O devices.

The game features:
- **PS2 Keyboard** for human character movement (WASD keys).
- **10 Red LEDs, 10 Switches, and 4 Pushbuttons** for in-game interaction.
- **VGA Display** with double buffering to prevent screen tearing and ensure smooth graphics.
- **Speakers** for interactive audio effects.

A key challenge was effectively integrating these I/O devices with the game logic, particularly in player detection. This involved complex logic to manage player movement, update their position, and handle collisions with other objects.

## Key Features

### Human Character:
- **Movement**: Controlled via WASD keys on a PS2 keyboard.
- **Power-ups**: Collect eight scattered power-ups to increase speed, making it harder for the AI to block paths.
- **Endgame Challenge**: Solve a 10-bit binary code displayed on LEDs to escape the facility.

### AI Character:
- **Gate Placement**: Blocks paths with gates, indicated by a cyan marker that turns red upon confirmation.
- **Blackout Ability**: Temporarily makes power-ups invisible with a 3-second cooldown, creating obstacles for the human player.

### Gameplay Mechanics:
- **Timer**: Both players have 120 seconds to achieve their goals.
- **Speed Increase**: Human character's speed increases with collected power-ups, escalating the AI’s challenge.
- **Endgame Scenarios**: Distinct win/loss screens based on game outcomes.

The balance between human and AI characters is achieved by increasing the human’s speed with each power-up, making it progressively harder for the AI to block them effectively.

## Technical Details

### Technologies Used:
- **Programming Language**: C
- **Hardware**: Intel DE1-SoC FPGA board
- **Software**: Intel Quartus Prime for compilation and CPUlator Computer System Simulator for testing

### Components:
- **VGA Display**: Utilizes double buffering to achieve 30 fps and prevent screen tearing. Dynamic elements are managed by saving their previous and current positions, optimizing performance.
- **PS2 Keyboard**: Custom handling of WASD inputs for smooth player movement.
- **LEDs and Switches**: Display binary codes and receive user inputs.

### Challenges and Solutions:
- **Player Detection**: Refined collision detection algorithms to handle player movement in various directions accurately.
- **I/O Device Coordination**: Addressed latency issues by profiling and optimizing code to handle simultaneous I/O operations effectively.

Optimizations included drawing only dynamic elements rather than the entire screen, significantly improving game performance.

## Installation and Setup

### Prerequisites:
- Intel DE1-SoC Board
- Intel Quartus Prime Software
- PS2 Keyboard
- VGA Display and Speakers

### Setup Instructions:
**Using CPUlator Computer System Simulator:**
1. Visit [CPUlator](https://cpulator.01xz.net/).
2. Set Language to C.
3. Download and paste the `concatenatedMainFile.c` code into the simulator.
4. Click `Compile and Load`, uncheck all debugging options, and run the code.

**Using Intel Quartus Prime Software:**
1. Clone the repository:
    ```bash
    git clone https://github.com/yourusername/Escape079.git
    ```
2. Navigate to the project directory:
    ```bash
    cd Escape079
    ```
3. Create a new project in Intel Quartus Prime.
4. Add all `.c` files (excluding `concatenatedMainFile.c`).
5. Compile and upload to the DE1-SoC board.
6. Connect the PS2 keyboard, VGA display, and speakers.

## Usage Instructions

### Controls:
**Human Character:**
- **W**: Move Up
- **A**: Move Left
- **S**: Move Down
- **D**: Move Right
- **Space**: Start the game

**AI Character:**
- **KEY0**: Shift to next gate location
- **KEY1**: Shift to previous gate location
- **KEY2**: Lock highlighted gate location
- **KEY3**: Blackout (turn off lights)

### Gameplay:
- **Human Character**: Collect all 8 power-ups and solve the 10-bit binary code to escape.
- **AI Character**: Use gates and blackouts to stall the human player and prevent them from escaping.

**Strategies:**
- **Human Player**: Collect power-ups quickly to increase speed and evade AI traps. Use movement to maneuver around gates.
- **AI Player**: Strategically place gates and use blackouts to hinder the human player’s progress and manage their time effectively.

## Examples and Demos

Screenshots and descriptions of key gameplay scenarios are provided to illustrate the game's mechanics and features. Each screenshot highlights different aspects of the game, such as the start screen, AI gate placement, and various gameplay states.

## Key Contributions

- **Endgame Logic**: Implemented dynamic 10-bit pin generation and smooth transition to end screens.
- **Interactive UI**: Developed features for the computer player’s abilities and interactions.
- **Movement and Collision**: Enabled smooth movement and collision detection with optimized algorithms.
- **HEX Game Counter**: Integrated HEX displays for game time countdown.

## Licensing

This project is a personal educational endeavor and is not licensed for public distribution or modification. External use must credit the author (Koosha Omidian).

## Contact Information

Author: Koosha Omidian  
Email: Koosha.omidian@mail.utoronto.ca  
For any inquiries or feedback, please reach out via email.

## Acknowledgments

- **Intel**: For providing the DE1-SoC FPGA board.
- **CPUlator**: For offering a simulation platform.
- **Visual Studio Code & Intel Quartus Prime**: For development and compilation tools.
- **Partner**: Acknowledgment to my partner for their contributions to the project.

## Future Improvements

- **Visual Enhancements**: Add more maps, improve graphics, and introduce higher resolution options.
- **Audio Features**: Integrate advanced sound effects and more interactive audio elements.
- **Extended Content**: Develop additional power-ups, abilities, and characters.
- **3D Implementation**: Transition to a game engine for 3D graphics and online gameplay.

## Challenges and Solutions

- **Player Detection**: Refined collision detection algorithms to accurately handle player movements and interactions.
- **I/O Device Coordination**: Optimized code to handle multiple I/O devices simultaneously, reducing latency and improving performance.
