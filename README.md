# RodTrackESP: Smart Rod Cutting Monitoring System

This project is a smart rod cutting monitoring system using ESP8266 and an IR sensor. It counts rods, measures cutting time, calculates the average, and displays live data on an OLED screen and a web server. The system also allows WhatsApp sharing, email reporting, and CSV download via a browser interface.

## 🛠️ Features

- Real-time rod count and timing
- OLED display updates
- Web server dashboard
- CSV download of data
- Share summary via WhatsApp and Gmail
- Mobile-friendly interface

## 🔌 Hardware Required

- ESP8266 (NodeMCU)
- IR Sensor (Obstacle/Beam Break)
- OLED 0.96" (128x64, I2C)
- Jumper wires
- Wi-Fi connection

## 🔧 Circuit Connections

| Component    | ESP8266 Pin |
|--------------|-------------|
| IR Sensor    | D5          |
| OLED SDA     | D2 (GPIO4)  |
| OLED SCL     | D1 (GPIO5)  |

> Make sure you use 3.3V for powering OLED and IR sensors (not 5V).

## 📲 Web Interface

- Shows live rod count, last cutting time, and average time.
- Data table of last 50 records.
- Share data:
  - [WhatsApp](https://wa.me/)
  - [Gmail Compose](https://mail.google.com/)
- Button to download CSV report.

## 🚀 Getting Started

1. Install the following libraries in Arduino IDE:
   - `ESP8266WiFi`
   - `Adafruit GFX`
   - `Adafruit SSD1306`

2. Replace WiFi credentials in code:
   ```cpp
   const char* ssid = "Your_WiFi";
   const char* password = "Your_Password";


