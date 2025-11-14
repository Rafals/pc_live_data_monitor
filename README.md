# Live PC Stats Monitor with ESP32

## 🚀 Project Overview

This project aims to create a dedicated, real-time hardware monitor display using an **ESP32** microcontroller.

The system works by setting up a local HTTP API on the PC side, which is powered by data collected from the **LibreHardwareMonitor** library. The ESP32 acts as a client, making HTTP GET requests to this local API, parsing the JSON response, and displaying the live PC statistics on a connected display (e.g., OLED, TFT).

Initially, the project focuses on core CPU metrics, but it is designed to be easily extensible.

## ✨ Features

* **Real-time Monitoring:** Get live updates of your PC's hardware status.
* **Decoupled Architecture:** Separates data collection (PC side, Arduino/C# application) from the display logic (ESP32).
* **JSON API:** Uses a standard JSON format for data transfer between the PC and the ESP32 for flexibility.
* **Current Metrics (Initial):**
    * **CPU Temperature**

## 🔧 Technology Stack

* **PC Side (API Server):**
    * **Arduino/C# Application:** To interface with LibreHardwareMonitor and serve the JSON API endpoint via HTTP.
    * **LibreHardwareMonitor:** For accessing granular CPU/GPU sensor data.
* **Display Side (Client):**
    * **ESP32 Microcontroller:** Handles Wi-Fi connection, HTTP requests, JSON parsing, and display rendering.
    * **Arduino IDE/PlatformIO:** For ESP32 programming.
    * **Libraries (e.g., `HTTPClient`, `ArduinoJson`):** For network communication and data handling.
    * **Display Library (e.g., U8g2, Adafruit GFX):** For visualization.

## 📈 Future Plans

* **Expanded Metrics:**
    * CPU Clock Speed (Core/Package)
    * CPU Load/Utilization
    * GPU Temperature, Clock Speed, and Load
    * RAM Usage
* **Display Enhancements:** Implementing clear visual indicators, graphs, or different screen layouts. 
* **Configuration:** Implement a web portal on the ESP32 for easy Wi-Fi configuration and API URL setup.
* **Error Handling:** Robust handling for API connection failures or invalid JSON responses.

## 💻 Setup and Usage

*(Here you will add specific instructions on how to compile and run the PC server application and how to flash the code onto the ESP32, including Wi-Fi setup and API address configuration.)*
