#pragma once

#include <WebSocketsClient.h>

class RemoteWebSocket {
  public:
    void begin();
    void loop();

  private:
    WebSocketsClient client;
    void onEvent(WStype_t type, uint8_t *payload, size_t length);
    void executeCommand(uint8_t *payload, size_t length);
};
