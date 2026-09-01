#pragma once

// Copy this file to RemoteWebSocketConfig.h and set the values for your server.
#define REMOTE_WS_HOST "your-domain.example"
#define REMOTE_WS_PORT 443
#define REMOTE_WS_PATH "/my-esp-home-rts/ws/device"
#define REMOTE_WS_DEVICE_ID "esp32-somfy"

// Paste your server's certificate authority (CA) certificate here.
// It is required for the ESP32 secure WSS connection.
static const char REMOTE_WS_CA_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
REPLACE_WITH_YOUR_CA_CERTIFICATE
-----END CERTIFICATE-----
)EOF";
