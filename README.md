# nRF24L01 Jammer for ESP32

This project implements an nRF24L01-based jammer on an ESP32 microcontroller. It uses the nRF24L01 module to interfere with various wireless communication channels (WiFi, BLE, Bluetooth, USB, Video, RC, and Full spectrum) by cycling through predefined channels. The project includes an LED blinking mechanism to indicate the current mode, controlled via a button.

## Features
- Supports multiple jamming modes: WiFi, BLE, Bluetooth, USB, Video, RC, and Full spectrum.
- Button-based mode switching with debouncing.
- LED blinking patterns to visually indicate the current mode.
- Uses the nRF24L01 module with SPI communication.
- Developed using PlatformIO for easy setup and dependency management.

## Hardware Requirements
- **ESP32 Development Board** (e.g., ESP32 DevKitC)
- **nRF24L01 Module**
- **Push Button** (connected to GPIO 0)
- **LED** (connected to GPIO 2)
- **Jumper Wires** and a breadboard for connections

## Software Requirements
- [Visual Studio Code](https://code.visualstudio.com/) with the [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
- Arduino framework (included via PlatformIO)
- Libraries: `SPI` and `RF24` (automatically installed via PlatformIO)

## Project Structure
```
nRF24L01-Jammer/
├── include/
│   └── led_control.h       # Header file with LED control declarations
├── src/
│   ├── main.ino           # Main program logic
│   ├── led_control.cpp    # Implementation of LED control functions
├── platformio.ini         # PlatformIO configuration file
└── README.md              # This file
```

## Installation
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-username/nRF24L01-Jammer.git
   cd nRF24L01-Jammer
   ```

2. **Open in VS Code**:
   - Open Visual Studio Code.
   - Use the PlatformIO extension to open the project folder (`nRF24L01-Jammer`).

3. **Install Dependencies**:
   - PlatformIO automatically installs the required libraries (`SPI` and `RF24`) based on `platformio.ini`.
   - If dependencies are not installed, run:
     ```bash
     pio lib install "SPI" "RF24"
     ```

4. **Connect Hardware**:
   - Connect the nRF24L01 module to the ESP32:
     - `MOSI` → GPIO 23
     - `MISO` → GPIO 19
     - `SCK` → GPIO 18
     - `CSN` → GPIO 21
     - `CE` → GPIO 22
     - `VCC` → 3.3V
     - `GND` → GND
   - Connect a push button to GPIO 0 (with pull-up resistor).
   - Connect an LED (with a current-limiting resistor) to GPIO 2.

## Usage
1. **Build the Project**:
   - In VS Code, open the PlatformIO toolbar (ant icon in the sidebar).
   - Click **Build** to compile the project.

2. **Upload to ESP32**:
   - Connect the ESP32 to your computer via USB.
   - Click **Upload** in the PlatformIO toolbar to flash the code to the ESP32.

3. **Monitor Output**:
   - Open the **Serial Monitor** in PlatformIO (baud rate: 115200) to view mode changes and status messages.

4. **Operation**:
   - The program starts in WiFi mode (1 blink).
   - Press the button (GPIO 0) to cycle through modes: WiFi (1 blink), BLE (no LED), Bluetooth (2 blinks), USB (no LED), Video (no LED), RC (3 blinks), Full (4 blinks).
   - The LED on GPIO 2 indicates the current mode with a specific number of blinks, followed by a pause.

## Code Overview
- **`main.ino`**: Contains the main logic for initializing the nRF24L01, handling button presses, and switching between channel sets for jamming.
- **`led_control.h`**: Declares constants, variables, and functions for controlling the LED blinking pattern.
- **`led_control.cpp`**: Implements the LED control functions (`startBlinking` and `updateLED`) and defines shared variables.
- **`platformio.ini`**: Configures the PlatformIO environment, specifying the ESP32 board and required libraries.

## Troubleshooting
- **Compilation Errors**:
  - Ensure all files are in the correct folders (`led_control.h` in `include`, `main.ino` and `led_control.cpp` in `src`).
  - Add to `platformio.ini` if `led_control.cpp` is not compiled:
    ```ini
    src_filter = +<*.ino> +<*.cpp>
    ```

- **IntelliSense Issues in VS Code**:
  - If VS Code cannot find `led_control.h`, create or update `.vscode/c_cpp_properties.json`:
    ```json
    {
        "configurations": [
            {
                "name": "PlatformIO",
                "includePath": [
                    "${workspaceFolder}/include",
                    "${workspaceFolder}/src",
                    "${workspaceFolder}/.pio/libdeps/**"
                ],
                "defines": [],
                "compilerPath": "/path/to/your/compiler",
                "cStandard": "c11",
                "cppStandard": "c++17",
                "intelliSenseMode": "gcc-x64"
            }
        ],
        "version": 4
    }
    ```

- **nRF24L01 Not Detected**:
  - Check wiring connections (especially `VCC` to 3.3V and correct SPI pins).
  - Verify that the nRF24L01 module is functional.

## Contributing
Contributions are welcome! Please submit a pull request or open an issue on GitHub for suggestions or bug reports.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments
- Built using the [Arduino framework](https://www.arduino.cc/).
- Uses the [RF24 library](https://github.com/nRF24/RF24) for nRF24L01 communication.