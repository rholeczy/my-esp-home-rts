# my-esp-home-rts

<div align="center">
  <img src="./resources/home_logo.png" alt="Home logo" width="120">
  <img src="./resources/websocket.png" alt="WebSocket logo" width="120">

  <p><strong>Remote control for Somfy RTS roller shutters.</strong></p>
  <p>Version 0.1.0 (beta)</p>
  <a href="./documentation/fr_FR/README.md" style="display: inline-flex; align-items: center; gap: 8px;"><img src="resources/fr_FR.png" alt="French" width="32" height="20">French</a>
</div>

## Overview

This project controls Somfy RTS roller shutters with an ESP32 and a CC1101 radio module. The ESP32 connects to the configured WebSocket server and receives commands from the FastAPI backend. The ESP32 is not directly exposed to the Internet.

The firmware integration is based on [ESPSomfy-RTS](https://github.com/rstrouse/ESPSomfy-RTS), version `2.4.7`.

## Hardware

You need the following hardware:

- An ESP32 development board. [Example product](https://www.amazon.de/dp/B07K68RQTS?ref=ppx_yo2ov_dt_b_fed_asin_title)
- A CC1101 radio module. [Example product](https://www.amazon.de/dp/B0D6B3885X?ref=ppx_yo2ov_dt_b_fed_asin_title)
- A compatible antenna for the CC1101 module.
- A Somfy RTS remote control for pairing shutters.

The wiring diagram is not available yet. Keep the ESP32 and the CC1101 module powered after configuration.

## Prerequisites

- [Arduino IDE](https://docs.arduino.cc/software/ide/)
- Python 3.9 or later
- An ESP32 connected by USB

Install the following packages from Arduino IDE:

| Location | Package | Version |
| --- | --- | --- |
| Boards Manager | `esp32` by Espressif Systems | `2.0.10` |
| Library Manager | `ArduinoJson` by Benoit Blanchon | `6.21.3` |
| Library Manager | `PubSubClient` by Nick O'Leary | `2.8` |
| Library Manager | `SmartRC-CC1101-Driver-Lib` by SmartRC | `2.5.7` |
| Library Manager | `WebSockets` by Markus Sattler | `2.4.0` |

## ESP32 WebSocket configuration

The WebSocket configuration is personal and is not committed to the repository.

1. Open the `SomfyController` folder in Arduino IDE.
2. Copy `RemoteWebSocketConfig.example.h` and rename the copy to `RemoteWebSocketConfig.h`.
3. Set the WebSocket host, path, device ID, and CA certificate in `RemoteWebSocketConfig.h`.
4. Compile and upload the sketch to the ESP32.

The same example configuration is also available in `firmware/espsomfy-rts-2.4.7`.

## Restore the ESP32 web file system

`firmware/SomfyController.littlefs.bin` contains the default ESPSomfy-RTS web file system. It provides the local ESP32 web interface used for the initial Wi-Fi, radio, and shutter configuration.

Flash this file only when the local interface displays an error such as `Error opening /index.html`, or after erasing the ESP32 flash memory. Flashing it replaces the ESP32 file system and removes its saved configuration, including Wi-Fi and shutter settings.

With the ESP32 connected by USB, run the following command from the project root. Replace `<serial-port>` with the serial port displayed by Arduino IDE, for example `/dev/cu.usbserial-0001` on macOS.

```bash
/Users/<your-user>/Library/Arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool --chip esp32 --port <serial-port> --baud 115200 write_flash 0x290000 firmware/SomfyController.littlefs.bin
```

Restart the ESP32 when the command finishes, then configure it again from its local web interface.

## Configure ESPSomfy-RTS

After uploading the sketch, complete the initial configuration from the ESP32 web interface.

1. Connect to the temporary Wi-Fi network created by the ESP32.
2. Configure the home Wi-Fi network from the ESPSomfy-RTS interface.
3. Restart the ESP32.
4. Find its private IP address in the router administration panel.
5. Open the ESP32 interface in a browser using this IP address.
6. Configure and enable the radio module.
7. Add a shutter with **Add Shade** and choose a name.
8. Put the shutter in pairing mode by holding the `PROG` button on its physical remote until the shutter moves.
9. Press `PROG` once in the ESPSomfy-RTS interface. The shutter should move a second time to confirm pairing.
10. Set **Repeat Commands** to two or three repeats, save the shutter, and test the Up, My, and Down buttons.

If the shutter does not respond, edit the shutter configuration and repeat the pairing process. The official [ESPSomfy-RTS documentation](https://github.com/rstrouse/ESPSomfy-RTS/wiki) contains more configuration details.

Shutter IDs are assigned in the order they are added. For example, the first three shutters have IDs `1`, `2`, and `3`.

## Run the backend locally

From the project root:

```bash
cd backend
python3 -m pip install -r requirements.txt
uvicorn app.main:app --reload
```

The local API is available at `http://127.0.0.1:8000`. The API documentation is available at `http://127.0.0.1:8000/docs`.

To send a command to shutter `1`:

```bash
curl -X POST http://127.0.0.1:8000/shutters/1/down
```

Available commands are `up`, `my`, and `down`.

## Simulate an ESP32 connection

Install [websocat](https://github.com/vi/websocat), start the backend, then open a second terminal:

```bash
websocat ws://127.0.0.1:8000/ws/device
```

After the connection is open, send the following message:

```json
{"type":"device_hello","device_id":"esp32-somfy"}
```

In another terminal, send a command with `curl`. The command JSON is displayed in the `websocat` terminal.
