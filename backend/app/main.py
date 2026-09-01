from typing import Literal

from fastapi import FastAPI, HTTPException, WebSocket, WebSocketDisconnect

app = FastAPI(
    title="MY-ESP-HOME-RTS API",
    version="0.1.0",
    # The API is published behind Nginx under this prefix.
    # Can be commented out if not needed.
    root_path="/my-esp-home-rts",
)

esp32_connection: WebSocket | None = None


@app.get("/health")
async def health():
    return {"status": "ok", "device_connected": esp32_connection is not None}


@app.websocket("/ws/device")
async def websocket_device(websocket: WebSocket):
    global esp32_connection

    await websocket.accept()
    esp32_connection = websocket

    print("Device connected")

    try:
        while True:
            message = await websocket.receive_text()
            print(f"Device message: {message}")

    except WebSocketDisconnect:
        print("Device disconnected")
    finally:
        if esp32_connection is websocket:
            esp32_connection = None


@app.api_route("/shutters/{shutter_id}/{command}", methods=["POST"])
async def send_command(
    shutter_id: int,
    command: Literal["up", "down", "my"],
):
    if esp32_connection is None:
        raise HTTPException(status_code=503, detail="No device connected")

    payload = {
        "type": "command",
        "shutter_id": shutter_id,
        "action": command
    }

    await esp32_connection.send_json(payload)

    return {
        "status": "sent",
        "command": payload,
    }
