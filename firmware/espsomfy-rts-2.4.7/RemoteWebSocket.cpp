#include <ArduinoJson.h>
#include "RemoteWebSocket.h"
#include "RemoteWebSocketConfig.h"
#include "Network.h"
#include "Somfy.h"

extern Network net;
extern SomfyShadeController somfy;

void RemoteWebSocket::begin() {
  client.beginSslWithCA(
    REMOTE_WS_HOST,
    REMOTE_WS_PORT,
    REMOTE_WS_PATH,
    REMOTE_WS_CA_CERT
  );
  client.setReconnectInterval(5000);
  client.enableHeartbeat(30000, 10000, 2);
  client.onEvent([this](WStype_t type, uint8_t *payload, size_t length) {
    onEvent(type, payload, length);
  });
}

void RemoteWebSocket::loop() {
  if (net.connected()) client.loop();
}

void RemoteWebSocket::onEvent(WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_CONNECTED) {
    DynamicJsonDocument hello(128);
    hello["type"] = "device_hello";
    hello["device_id"] = REMOTE_WS_DEVICE_ID;
    String message;
    serializeJson(hello, message);
    client.sendTXT(message);
    Serial.println("VPS WebSocket connected");
  } else if (type == WStype_TEXT) {
    executeCommand(payload, length);
  } else if (type == WStype_DISCONNECTED) {
    Serial.println("VPS WebSocket disconnected");
  }
}

void RemoteWebSocket::executeCommand(uint8_t *payload, size_t length) {
  DynamicJsonDocument document(256);
  if (deserializeJson(document, payload, length)) {
    Serial.println("Invalid VPS WebSocket payload");
    return;
  }

  if (document["type"] != "command") return;

  const uint8_t shutterId = document["shutter_id"] | 0;
  const char *action = document["action"] | "";
  SomfyShade *shade = somfy.getShadeById(shutterId);
  if (shade == nullptr) {
    Serial.printf("Unknown shutter id: %u\n", shutterId);
    return;
  }

  if (strcmp(action, "up") == 0) {
    shade->sendCommand(somfy_commands::Up);
  } else if (strcmp(action, "down") == 0) {
    shade->sendCommand(somfy_commands::Down);
  } else if (strcmp(action, "my") == 0) {
    shade->sendCommand(somfy_commands::My);
  } else {
    Serial.printf("Unknown action: %s\n", action);
  }
}
