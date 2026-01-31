# PIC24 Robotic Arm Controller

An embedded systems project that controls a 5-servo robotic arm using a PIC24 microcontroller, featuring dual input control via joystick and UART keyboard commands.

Developed as part of the Embedded Systems Design course at EHU during the Computer Engineering degree.

## Overview

This project implements a complete control system for a robotic arm with 5 degrees of freedom. The system supports two control modes: manual control through an analog joystick with triggers, and remote control via UART serial communication. Real-time feedback is displayed on an LCD screen showing arm position, sensor readings, and system status.

## Features

### Dual Control Modes
- **Joystick Control**: Analog joystick for X/Y axis movement, lateral lever for Z axis, front trigger for gripper
- **UART Control**: Keyboard-based control through serial terminal (TeraTerm or similar)
- **Mode Switching**: Toggle between control modes using physical button (S6)

### Robotic Arm Control
- 5 servomotors controlling different joints
- Smooth PWM-based servo control using Timer2 interrupts
- Safe position function for emergency arm positioning
- Independent gripper control (open/close and tilt)

### Sensor Integration
- **Potentiometer**: Controls gripper tilt servo position
- **Temperature Sensor**: Real-time temperature monitoring
- **Joystick Axes**: X, Y, Z movement with analog precision

### User Interface
- **LCD Display**: Shows arm status, joystick values, chronometer, and sensor readings
- **Scrollable Content**: Button-controlled display scrolling
- **LED Indicators**: Visual feedback for system status
- **Chronometer**: Built-in timer with start/stop/reset functionality

## Hardware Components

| Component | Description |
|-----------|-------------|
| PIC24 Microcontroller | Main processing unit running at 80 MHz |
| 5x Servo Motors | Joint actuation (connected to RD0-RD3, RD9) |
| Analog Joystick | 3-axis input with trigger (RB2, RB8, RB9, RB10) |
| LCD Display | Status and feedback display |
| Potentiometer | Analog input for gripper tilt |
| Temperature Sensor | Environmental monitoring |
| Push Buttons | S3 (scroll), S4 (safe position), S6 (mode switch) |

## Pin Configuration

### Joystick Connections
| Wire Color | Signal | Pin |
|------------|--------|-----|
| Blue | Rx (X-axis) | RB8 |
| Yellow | Ry (Y-axis) | RB9 |
| White | Lever (Z-axis) | RB10 |
| Brown | Trigger | RB2 |
| Red | VCC | 3.3V |
| Black (x2) | GND | GND |

### Servo Connections
| Servo | Function | Pin |
|-------|----------|-----|
| S1 | Base rotation | RD0 |
| S2 | Shoulder | RD1 |
| S3 | Elbow | RD2 |
| S4 | Gripper tilt | RD3 |
| S5 | Gripper open/close | RD9 |

## System Architecture

```
                    ┌─────────────────┐
                    │   PIC24 MCU     │
                    │    (80 MHz)     │
                    └────────┬────────┘
                             │
        ┌────────────────────┼────────────────────┐
        │                    │                    │
        ▼                    ▼                    ▼
┌───────────────┐    ┌───────────────┐    ┌───────────────┐
│   Inputs      │    │   Outputs     │    │  Communication│
├───────────────┤    ├───────────────┤    ├───────────────┤
│ - Joystick    │    │ - 5 Servos    │    │ - UART2       │
│ - Potentiometer│   │ - LCD Display │    │ - RS232       │
│ - Temp Sensor │    │ - LEDs        │    │               │
│ - Buttons     │    │               │    │               │
└───────────────┘    └───────────────┘    └───────────────┘
```

## Modules and Peripherals

### Timers
- **Timer2**: PWM generation for servo control (sequential multiplexing)
- **Timer4**: System tick (4ms intervals) for chronometer

### Interrupts
| Source | Purpose |
|--------|---------|
| ADC1 | Analog-to-digital conversion complete |
| Timer2 | Servo PWM control cycle |
| Timer4 | Chronometer increment |
| UART2 RX | Keyboard command reception |
| UART2 TX | Serial data transmission |
| CN (Change Notification) | Button press detection |

### ADC Configuration
- 2000 conversions per second
- 8-sample averaging for noise reduction
- Channels: RB2, RB4, RB5, RB8, RB9, RB10

## Keyboard Controls (UART Mode)

### Arm Movement
| Key | Action |
|-----|--------|
| W / w | Move up |
| S / s | Move down |
| A / a | Move left |
| D / d | Move right |
| T / t | Move forward (Z-axis) |
| G / g | Move backward (Z-axis) |

### Gripper Control
| Key | Action |
|-----|--------|
| U / u | Tilt gripper up |
| J / j | Tilt gripper down |
| K / k | Close gripper |
| H / h | Open gripper |

### Chronometer
| Key | Action |
|-----|--------|
| P / p | Pause |
| C / c | Continue |
| T / t | Reset |

## Project Structure

```
pic24-robotic-arm/
├── main.c                 # Main program loop and initialization
├── commons.h              # Shared definitions and constants
├── ADC.c / ADC.h          # Analog-to-digital converter module
├── CN.c / CN.h            # Change notification (button handling)
├── GPIO.c / GPIO.h        # Pin configuration and control
├── LCD.c / LCD.h          # LCD display driver
├── timers.c / timers.h    # Timer configuration and ISRs
├── UART2_RS232.c / .h     # Serial communication
├── oscilator.c / .h       # Clock configuration (80 MHz)
├── mensajes.c / .h        # Message handling for display/UART
├── utilidades.c / .h      # Utility functions
├── i2c_funciones.c / .h   # I2C communication functions
├── srf08.c / srf08.h      # Ultrasonic sensor driver
└── Makefile               # Build configuration

docs/                  # Project documentation
   └── pic24_robotic_arm_report.pdf
```

## Technical Implementation

### Servo Control Strategy

The system uses Timer2 to generate PWM signals for all 5 servos through sequential multiplexing:

1. Timer2 interrupt fires
2. Current servo output is set LOW
3. Move to next servo in sequence
4. Load new period value (PR2) from duties[] array
5. Set servo output HIGH
6. Repeat cycle

This approach allows precise control of multiple servos using a single timer.

### ADC Sampling

The ADC module samples multiple analog inputs in sequence:
- Joystick X, Y, Z axes
- Potentiometer
- Temperature sensor

Each input is sampled 8 times and averaged to reduce noise, providing stable readings for smooth arm control.

### Program Flow

```
main()
│
├── Initialize all modules
│   ├── Oscillator (80 MHz)
│   ├── GPIO pins
│   ├── Timers (T2, T4)
│   ├── ADC
│   ├── UART
│   ├── LCD
│   └── CN module
│
└── while(1)
    ├── Check ADC flag → Update servo positions
    ├── Check UART flag → Process commands
    ├── Check button flags → Handle user input
    └── Update LCD display
```

## Building and Flashing

### Prerequisites
- MPLAB X IDE
- XC16 Compiler
- PICkit programmer

### Compilation

1. Open project in MPLAB X IDE
2. Select configuration (default)
3. Build project (Production → Build)

### Flashing

1. Connect PICkit to PIC24 board
2. Select programmer in MPLAB X
3. Program device

## Technologies

- **Microcontroller**: PIC24
- **Language**: C (XC16 compiler)
- **IDE**: MPLAB X
- **Clock Speed**: 80 MHz
- **Communication**: UART/RS232

## Course Context

This project integrates concepts from all previous lab sessions in the Embedded Systems Design course:
- GPIO configuration and control
- Timer-based PWM generation
- ADC for analog sensor reading
- UART serial communication
- Interrupt-driven programming
- LCD interfacing

## License

This project was developed for educational purposes as part of university coursework.
