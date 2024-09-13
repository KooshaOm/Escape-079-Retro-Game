# Escape 079: FPGA Arcade-Style Game

## Table of Contents
1. [Project Overview](#project-overview)
2. [Key Features](#key-features)
3. [Technical Details](#technical-details)
4. [Installation and Setup](#installation-and-setup)
5. [Usage Instructions](#usage-instructions)
   - [Controls](#controls)
   - [Gameplay](#gameplay)
6. [Examples and Demos](#examples-and-demos)
7. [Contribution](#contribution)
8. [Licensing](#licensing)
9. [Contact Information](#contact-information)
10. [Acknowledgments](#acknowledgments)
11. [Future Improvements](#future-improvements)
12. [Challenges and Solutions](#challenges-and-solutions)



## Project Overview
Escape 079 is a 2-player arcade-style game designed for the Intel DE1-SoC FPGA board using C. It provides an engaging experience where one player, the human, navigates a facility to collect power-ups and solve binary puzzles, while the other player, the AI virus, attempts to thwart their progress with strategic gate placements and blackouts.



## Key Features

- **Human Character**:
  - **Movement**: Controlled via WASD keys on a PS2 keyboard.
  - **Power-ups**: Collect three scattered power-ups to enhance speed.
  - **Endgame Challenge**: Solve a 10-bit binary code displayed on LEDs using switches within a time limit.

- **AI Character**:
  - **Gate Placement**: Blocks paths with gates, indicated by a cyan marker that turns red upon confirmation.
  - **Blackout Ability**: Temporarily renders power-ups invisible with a 3-second cooldown.

- **Gameplay Mechanics**:
  - **Timer**: Both players have 120 seconds to achieve their goals.
  - **Speed Increase**: The human character's speed increases with each collected power-up, escalating the AI’s challenge.
  - **Endgame Scenarios**: Features distinct win/loss screens for an immersive experience.



## Technical Details

- **Technologies Used**:
  - **Programming Languages**: C
  - **Hardware**: Intel DE1-SoC FPGA board.
  - **Software**: Intel Quartus Prime for compilation and CPUlator Computer System Simulator for testing.

- **Components**:
  - **VGA Display**: Enhanced with double buffering to achieve 30 fps and eliminate screen tearing.
  - **PS2 Keyboard**: Custom handling of WASD inputs for player movement.
  - **LEDs and Switches**: Used for displaying binary codes and receiving user inputs.



## Installation and Setup

- **Prerequisites**:
  - Intel DE1-SoC Board
  - Intel Quartus Prime Software
  - PS2 Keyboard
  - Speakers and VGA Display

- **Setup Instructions**:
  1. **CPUlator Computer System Simulator**:
     - [Visit CPUlator](https://cpulator.01xz.net/?sys=nios-de1soc).
     - Download the `concatenatedMainFile.c` file from the repository.
     - Copy the source code from the file and paste it into the CPUlator simulator.

  2. **Intel Quartus Prime Software and DE1-SoC Board**:
     - Clone the repository:
       ```bash
       git clone https://github.com/yourusername/Escape079.git
       ```
     - Navigate to the project directory:
       ```bash
       cd Escape079
       ```
     - Create a new project in Intel Quartus Prime.
     - Add all `.c` files (except `concatenatedMainFile.c`) to the project.
     - Compile the code and upload it to the DE1-SoC board.
     - Connect the PS2 keyboard, speakers, and VGA display to the DE1-SoC board.



## Usage Instructions

### Controls

#### Human Character
- **Movement Controls:**
  - **`W`**: Move Up
  - **`A`**: Move Left
  - **`S`**: Move Down
  - **`D`**: Move Right
  - **`Space`**: Start the game from the menu screen

  **Description**: These keys on the PS2 keyboard allow the human character to navigate through the facility. Each key corresponds to a specific direction, enabling smooth and precise movement.

#### AI Character
- **Gate Placement and Management:**
  - **`KEY0`**: Shift to next door
  - **`KEY1`**: Shift to previous door
  - **`KEY2`**: Lock highlighted door
  - **`KEY3`**: Blackout (turn off lights)

  **Description**: Use these keys to manage gates and blackout abilities. Gates can be placed to block paths, and the blackout ability makes power-ups temporarily invisible.

### Gameplay
- **Human Character**: Collect all three power-ups scattered across the map and solve the 10-bit binary code displayed on the LEDs using switches before time runs out.
- **AI Character**: Use strategic placement of gates and blackouts to prevent the human character from achieving their objectives within the 120-second time limit.



## Examples and Demos

- **Video Demo**: [Watch the Gameplay Video](https://youtu.be/SSyPJ6sB0Eg)
- **Screenshots**:
   ![Map Screenshot](https://github.com/KooshaOm/Escape079/blob/main/media/map.jpg?raw=true)
   ![Game Screenshot](https://github.com/KooshaOm/Escape079/blob/main/media/computer_wins_endscreen.jpg?raw=true)
   ![Endgame Screen](https://github.com/KooshaOm/Escape079/blob/main/media/player_wins_endscreen.jpg?raw=true)



## Contribution

### Koosha
- **Player Wins End Game Logic**: Implemented endgame sequence with dynamic 10-bit pin generation and smooth transition to the end screen.
- **Computer Ability Interactive UI Features**: Developed interactive UI elements showcasing the computer player’s abilities.
- **Player Sprints & Movement**: Enabled smooth player movement with WASD controls and orientation updates.
- **Player Character Collision Detection Logic**: Implemented collision detection to prevent bypassing walls and doors.
- **Hex Game Counter**: Integrated HEX displays for game time countdown on the FPGA board.
- **Keyboard Mapping Implementation**: Adapted keyboard inputs for game controls and functionality.
- **Pick-Up Collection Logic**: Ensured accurate pick-up registration.

### Sepehr
- **Sound Features**: Processed sound files, integrated them with hardware, and implemented real-time sound rendering.
- **End Screen Sound and Graphics**: Developed end sequences with sound and graphical effects.
- **Initial Player Movement**: Implemented primitive player rendering and movement.
- **Keycard & Blackout Rendering Logic**: Managed rendering of keycards and blackout effects.
- **UI Game Counter and Keycard Counter**: Designed and implemented UI for game and keycard counters.
- **Overall Map Design**: Designed the map layout and positions of keycards and doors.



## Licensing
This project is for educational purposes only and is not licensed for public distribution.



## Contact Information

- **Author**: Koosha Omidian
- **Email**: Koosha.omidian@mail.utoronto.ca



## Acknowledgments

- **Intel**: For providing the DE1-SoC FPGA board.
- **CPUlator**: For offering a convenient simulation platform.
- **Visual Studio Code & Intel Quartus Prime**: For the development and compilation tools.



## Future Improvements

- **Visual Enhancements**: Implement higher resolution graphics and improve visual performance.
- **Audio Features**: Integrate advanced sound effects and additional visual elements.
- **Extended Content**: Develop new maps, power-ups, and explore transitioning to a 3D game engine for a richer experience.



## Challenges and Solutions

- **Player Detection**: Implemented complex logic to track player movement, update positions, and handle collisions. This was achieved by refining collision detection algorithms and rendering logic.
- **Integrating I/O Devices**: Coordinated interactions between multiple I/O devices (keyboard, LEDs, switches) was complex. This challenge was managed through detailed design and extensive testing on both the CPUlator simulator and the FPGA board.
