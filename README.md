# Arduino Touch Screen Drawing Program

An interactive drawing program for Arduino-based TFT touch screens using the MCUFRIEND_kbv library. This program turns your TFT display into a digital canvas where you can draw with different colors and brush sizes.

## Features

- Real-time touch input drawing
- Multiple color selection
- Adjustable brush sizes
- Clear screen functionality
- User-friendly interface
- Memory-optimized implementation
- Smooth drawing experience

## Hardware Requirements

- Arduino board (tested on Arduino Uno)
- MCUFRIEND TFT display with touch capability
- Compatible touch screen shield

## Setup Instructions

1. Required Libraries:
   - Adafruit_GFX
   - MCUFRIEND_kbv
   - TouchScreen

2. Wiring:
   - Connect TFT display to Arduino following MCUFRIEND pin mapping
   - Ensure touch screen pins are properly connected

3. Library Installation:
   ```bash
   # Using Arduino IDE Library Manager
   1. Tools -> Manage Libraries
   2. Search for "MCUFRIEND_kbv"
   3. Search for "Adafruit GFX Library"
   4. Search for "TouchScreen"
   ```

## Usage

1. Upload the program to your Arduino board
2. Touch screen to draw
3. Use toolbar buttons to:
   - Select colors
   - Change brush size
   - Clear screen
   - Access additional tools

## Memory Usage

The program is optimized for Arduino's limited resources:
- Program Storage: Efficiently uses available flash memory
- Dynamic Memory: Minimizes RAM usage
- No external memory required

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is open source and available under the MIT License.

## Acknowledgments

- MCUFRIEND_kbv library developers
- Adafruit GFX library team
- Arduino community
