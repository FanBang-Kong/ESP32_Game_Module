# ESP32_Game_Module
## Plane Shooter and Brick Breaker Games for ESP32

## Introduction

This project contains two classic arcade games, **Plane Shooter** and **Brick Breaker**, developed for the ESP32 microcontroller with an ST7789 240x240 TFT display. The games are controlled using a joystick connected to the ESP32.

本项目在 ESP32 微控制器上开发了两个经典的街机游戏，**飞机射击** 和 **打砖块**，使用 ST7789 240x240 TFT 显示屏和ESP32。游戏通过连接到一个小型摇杆进行控制。

---

## Features

- **Plane Shooter**: Control your plane to shoot down enemy aircraft.
- **Brick Breaker**: Use the paddle to bounce the ball and break all the bricks on the screen.

---

## Hardware Requirements

- **ESP32 Development Board**
- **ST7789 240x240 TFT Display**
- **Joystick Module**
- **Connecting Wires**
- **Breadboard** (optional)

---

## Wiring Diagram

**Please refer to the wiring diagram below to connect your hardware components.**

*(Insert wiring diagram image here.)*

### Pin Connections

| ESP32 Pin | TFT Display Pin | Joystick Pin |
|-----------|-----------------|--------------|
| 5         | CS              |              |
| 4         | RST             |              |
| 2         | DC              |              |
| 18        | SCL             |              |
| 23        | SDA             |              |
| 16        | BACKLIGHT_PIN   |              |
| 15        |                 | VRX          |
| 13        |                 | VRY          |
| 21        |                 | SW           |
| GND       | GND             | GND          |
| 3.3V      | VCC             | VCC          |

---
