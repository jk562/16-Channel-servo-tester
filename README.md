# 16-Channel-servo-tester
# Arduino 16-Channel Servo Tester (Memory Optimized)

This project is a **lightweight, memory-optimized servo tester** for **Arduino Nano** using a **PCA9685 16-channel PWM driver** and a **1.3" SH1106 OLED display**.  
It allows controlling up to **16 servo motors** with multiple modes, a pot-controlled scrollable menu, and 3 push buttons for navigation.

---

## ✨ Features
- 🚀 Ultra-optimized for Arduino Nano → minimal memory usage
- 📟 OLED interface with scrollable menus and scrollbar indicators
- 🎚️ Potentiometer control for menu navigation and servo angles
- 🕹️ Four Servo Test Modes:
  1. **Centre Mode** → All servos move to 90°
  2. **Manual Mode** → Control all servos together using potentiometer (0°–180°)
  3. **Auto Mode** → Continuous sweep (0° → 180° → 0°)
  4. **Individual Servo Mode** → Select any of the 16 channels and control angle manually
- ⌨️ Three Push Buttons → Select, OK, Back for menu navigation
- 🧭 Smooth menu and servo selection scrolling
- 🔧 Debounced button handling
- 🛡️ Servo pulse limits for safe operation

---

## 🛠️ Hardware Required
- **Arduino Nano (ATmega328P)**
- **PCA9685 16-Channel PWM Servo Driver**
- **1.3" SH1106 OLED Display (I2C)**
- **10kΩ Potentiometer** (for angle/menu control)
- **3 × Push Buttons** (Select / OK / Back)
- **16 × Servo Motors** (standard hobby)
- **External 5V power supply** for servos

---

## 📌 Pin Connections

| Component          | Pin on Arduino Nano |
|--------------------|-------------------|
| OLED SDA           | A4                |
| OLED SCL           | A5                |
| PCA9685 SDA        | A4                |
| PCA9685 SCL        | A5                |
| Potentiometer      | A0                |
| Button Select      | D2                |
| Button OK          | D3                |
| Button Back        | D4                |
| Servo Power V+     | External 5V       |
| Servo GND          | Common Ground     |

> ⚠️ Important: Servos must be powered externally. Arduino Nano cannot supply enough current for multiple servos.

---

## 📖 How It Works
1. On startup, displays a splash screen: **“SERVO TESTER – 16 Channel”**
2. Main menu has four options: Centre, Manual, Auto, Individual  
   - Navigate using potentiometer or Select button  
   - Enter a mode with OK button  
   - Return to menu with Back button
3. **Modes update OLED** with servo status, angle value, and scrollbar
4. **Auto Mode** sweeps all servos smoothly  
5. **Individual Mode** allows selection and control of one servo at a time

---

## 🖼️ MENU (OLED)
1. Main Menu → Scrollable list with bar  
2. Centre Mode → All servos at 90°  
3. Manual Mode → Pot controls all servos, angle shown with gauge  
4. Auto Mode → Sweeping servos with current angle display  
5. Servo Select → Scrollable list of 16 servos with scrollbar  
6. Individual Servo Mode → Control selected servo manually

---

## 📂 Code Highlights
- Uses `#define` constants instead of `enum`/`strings` → saves RAM  
- Minimal display buffer (`_1_HW_I2C`) → low memory usage  
- Potentiometer controls both menu and servo angles  
- Debounced button reading for reliable input  
- Reusable functions for menu, sweeping, and servo centering  

---

## 🔌 Usage Instructions
1. Upload the Arduino sketch to the Nano  
2. Connect OLED and PCA9685 via I2C  
3. Connect potentiometer and buttons  
4. Power servos with external supply  
5. On startup → Splash screen → Navigate menu with pot or Select button  
6. Enter mode with OK button  
7. Use Back button to return to main menu  

---

## ⚡ Applications
- Robotics & mechatronics prototyping  
- Servo calibration & testing  
- Multi-servo robotic projects (hexapods, quadrupeds, robotic arms)  
- Educational tool for PWM and servo control  

---

## 🔧 Future Improvements
- Add EEPROM memory to save last servo positions  
- Support for individual servo speed adjustment  
- Customizable servo limits per channel  

---

