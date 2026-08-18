# POC — Connected Somfy RTS Shutters

This project allows remote control of three Somfy RTS roller shutters.
An ESP32 with a CC1101 module, running the ESPSomfy-RTS firmware, controls the shutters on the local network.
The POC includes a web application and a FastAPI backend to relay commands via a secure WebSocket connection initiated by the ESP32.
The ESP32 is therefore not directly exposed to the Internet.
Currently, only the minimal backend skeleton is present.

## POC — Volets connectés Somfy RTS

Ce projet permet de piloter à distance trois volets roulants Somfy RTS.
Un ESP32 avec module CC1101, équipé du firmware ESPSomfy-RTS, commande les volets sur le réseau local.
Le POC prévoit une application web et un backend FastAPI pour relayer les commandes via une connexion WebSocket sécurisée initiée par l'ESP32.
L'ESP32 n'est donc pas exposé directement sur Internet.
Actuellement, seul le squelette minimal du backend est présent.
