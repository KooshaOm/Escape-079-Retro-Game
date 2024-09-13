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
7. [Key Contributions](#key-contributions)
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
  - **Power-ups**: Collect eight (8) scattered power-ups to enhance speed.
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
     - Set `Language:` to `C` by clicking on the dropdown menu at the top of the UI.
     - Download the `concatenatedMainFile.c` file from the repository.
     - Copy the source code from the file and paste it into the CPUlator simulator.
     - Click on `Compile and Load` button at the top to compile the code.
     - Uncheck all boxes under `Debugging Checks` section located on the left.
     - Press `Continue` button at the top to run the code.
     - Refer to `Devices` section on the right to use I/O devices.

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
  - **`KEY0`**: Shift to next gate location
  - **`KEY1`**: Shift to previous gate location
  - **`KEY2`**: Lock highlighted gate location
  - **`KEY3`**: Blackout (turn off lights)

  **Description**: Use these keys to manage gates and blackout abilities. Gates can be placed to block paths, and the blackout ability makes power-ups temporarily invisible.

### Gameplay
- **Human Character**: Collect all eight (8) power-ups scattered across the map and solve the 10-bit binary code displayed on the LEDs using switches before time runs out.
- **AI Character**: Use strategic placement of gates and blackouts to prevent the human character from achieving their objectives within the 120-second time limit.



## Examples and Demos

This section showcases various gameplay scenarios in **Escape 079** with accompanying screenshots and descriptions to illustrate key aspects of the game.

### 1. Start Screen

![Start Screen](media/start_screen.png)

**Description**: The start screen instructs the player to press the space button to begin the game. This screen sets the stage for the gameplay by providing initial instructions.

---

### 2. Initial Game State

![Initial Game State](media/intial_game_conditions.png)

**Description**: This screenshot shows the initial state of the game with the player positioned at the top left of the map. The 8 power-ups are scattered across the map, and the player starts exploring the environment.

---

### 3. AI Character Gate Placement

![AI Character Gate Placement](media/gate_placeholder_ability.png)

**Description**: The AI character is moving the gate indicator to a new location on the map (indicated by cyan). This screenshot shows the AI planning its strategy to block the human player.

---

### 4. Gate Confirmation

![Gate Confirmation](media/gate_locked_ability.png)

**Description**: The AI character confirms and builds a gate, which is shown in red. The lock icon at the top changes from open to closed, indicating that the gate blocking ability has been used.

---

### 5. Closed Gate Obstacle

![Closed Gate Obstacle](media/blocked_player.png)

**Description**: The human player encounters a closed (red) gate and cannot pass through. This image demonstrates how gates block the player's path and add challenge to the game.

---

### 6. Blackout Effect

![Blackout Effect](media/blackout_ability.png)

**Description**: The AI character activates the blackout ability, making all power-ups invisible. This screenshot illustrates the temporary obstacle imposed on the human player.

---

### 7. Power-Ups and Binary Pin

![Power-Ups and Binary Pin](media/all_cards_collected.png)

**Description**: The human player has collected all 8 power-ups, as shown by the count at the top right. The randomly generated 10-bit binary pin is displayed at the bottom left, and the same code is shown on the red LEDs.

---

### 8. HEX Display Timer

![HEX Display Timer](media/hex_display_counter.png)

**Description**: The HEX display shows the remaining time out of the 120 seconds for the match. This image demonstrates how the game time is tracked and displayed.

---

### 9. Entering Binary Pin

![Entering Binary Pin](entering_pin_on_switches.png)

**Description**: The human character is entering the 10-bit binary pin using the switches. Each LED position (lit or off) corresponds to the switch setting needed to match the binary code.

---

### 10. Player Wins End Screen

![Player Wins End Screen](media/human_wins_endscreen.png)

**Description**: The player successfully collected all power-ups, entered the binary pin correctly, and escaped within the time limit. This end screen celebrates the player’s victory.

---

### 11. AI Wins End Screen

![AI Wins End Screen](media/computer_wins_endscreen.png)

**Description**: If the time runs out before the human player escapes, the AI wins. This end screen is displayed when the player fails to meet the objectives in time.


## Key Contributions

- **Player Wins End Game Logic**: Implemented endgame sequence with dynamic 10-bit pin generation and smooth transition to the end screen.
- **Computer Ability Interactive UI Features**: Developed interactive UI elements showcasing the computer player’s abilities.
- **Player Sprints & Movement**: Enabled smooth player movement with WASD controls and orientation updates.
- **Player Character Collision Detection Logic**: Implemented collision detection to prevent bypassing walls and closed gates.
- **Hex Game Counter**: Integrated HEX displays for game time countdown on the FPGA board.
- **Keyboard Mapping Implementation**: Adapted keyboard inputs for game controls and functionality.
- **Pick-Up Collection Logic**: Ensured accurate pick-up registration.



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
