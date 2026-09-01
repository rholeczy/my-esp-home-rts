# my-esp-home-rts

<div align="center">
  <img src="../../resources/home_logo.png" alt="Logo maison" width="120">
  <img src="../../resources/websocket.png" alt="Logo WebSocket" width="120">

  <p><strong>Commande à distance de volets roulants Somfy RTS.</strong></p>
  <p>Version 0.1.0 (bêta)</p>
</div>

## Présentation

Ce projet permet de contrôler des volets roulants Somfy RTS avec un ESP32 et un module radio CC1101. L'ESP32 se connecte au serveur WebSocket configuré et reçoit les commandes du backend FastAPI. L'ESP32 n'est pas directement exposé sur Internet.

L'intégration du firmware est basée sur [ESPSomfy-RTS](https://github.com/rstrouse/ESPSomfy-RTS), version `2.4.7`.

## Matériel

Le matériel suivant est nécessaire :

- Une carte de développement ESP32. [Exemple de produit](https://www.amazon.de/dp/B07K68RQTS?ref=ppx_yo2ov_dt_b_fed_asin_title)
- Un module radio CC1101. [Exemple de produit](https://www.amazon.de/dp/B0D6B3885X?ref=ppx_yo2ov_dt_b_fed_asin_title)
- Une antenne compatible avec le module CC1101.
- Une télécommande Somfy RTS pour associer les volets.

Le schéma de montage n'est pas encore disponible. L'ESP32 et le module CC1101 doivent rester alimentés après la configuration.

## Prérequis

- [Arduino IDE](https://docs.arduino.cc/software/ide/)
- Python 3.9 ou une version ultérieure
- Un ESP32 connecté en USB

Installez les paquets suivants depuis Arduino IDE :

| Emplacement | Paquet | Version |
| --- | --- | --- |
| Gestionnaire de cartes | `esp32` par Espressif Systems | `2.0.10` |
| Gestionnaire de bibliothèques | `ArduinoJson` par Benoit Blanchon | `6.21.3` |
| Gestionnaire de bibliothèques | `PubSubClient` par Nick O'Leary | `2.8` |
| Gestionnaire de bibliothèques | `SmartRC-CC1101-Driver-Lib` par SmartRC | `2.5.7` |
| Gestionnaire de bibliothèques | `WebSockets` par Markus Sattler | `2.4.0` |

## Configuration WebSocket de l'ESP32

La configuration WebSocket est personnelle et n'est pas enregistrée dans le dépôt.

1. Ouvrez le dossier `SomfyController` dans Arduino IDE.
2. Copiez `RemoteWebSocketConfig.example.h` et renommez la copie en `RemoteWebSocketConfig.h`.
3. Renseignez l'hôte WebSocket, le chemin, l'identifiant de l'appareil et le certificat d'autorité dans `RemoteWebSocketConfig.h`.
4. Compilez et téléversez le sketch sur l'ESP32.

Le même exemple de configuration est aussi disponible dans `firmware/espsomfy-rts-2.4.7`.

## Restaurer le système de fichiers web de l'ESP32

`firmware/SomfyController.littlefs.bin` contient le système de fichiers web par défaut d'ESPSomfy-RTS. Il fournit l'interface web locale de l'ESP32 utilisée pour la première configuration du Wi-Fi, de la radio et des volets.

Téléversez ce fichier uniquement si l'interface locale affiche une erreur comme `Error opening /index.html`, ou après avoir effacé la mémoire flash de l'ESP32. Cette opération remplace le système de fichiers de l'ESP32 et supprime sa configuration enregistrée, y compris les réglages Wi-Fi et les volets.

Avec l'ESP32 connecté en USB, lancez la commande suivante depuis la racine du projet. Remplacez `<serial-port>` par le port série affiché par Arduino IDE, par exemple `/dev/cu.usbserial-0001` sur macOS.

```bash
/Users/<your-user>/Library/Arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool --chip esp32 --port <serial-port> --baud 115200 write_flash 0x290000 firmware/SomfyController.littlefs.bin
```

Redémarrez l'ESP32 à la fin de la commande, puis refaites sa configuration depuis son interface web locale.

## Configurer ESPSomfy-RTS

Après le téléversement du sketch, effectuez la première configuration depuis l'interface web de l'ESP32.

1. Connectez-vous au réseau Wi-Fi temporaire créé par l'ESP32.
2. Configurez le réseau Wi-Fi de la maison depuis l'interface ESPSomfy-RTS.
3. Redémarrez l'ESP32.
4. Trouvez son adresse IP privée dans l'interface d'administration du routeur.
5. Ouvrez l'interface de l'ESP32 dans un navigateur avec cette adresse IP.
6. Configurez et activez le module radio.
7. Ajoutez un volet avec **Add Shade** et choisissez son nom.
8. Mettez le volet en mode association en maintenant le bouton `PROG` de sa télécommande physique jusqu'à ce que le volet bouge.
9. Appuyez une fois sur `PROG` dans l'interface ESPSomfy-RTS. Le volet doit bouger une deuxième fois pour confirmer l'association.
10. Réglez **Repeat Commands** sur deux ou trois répétitions, enregistrez le volet, puis testez les boutons Up, My et Down.

Si le volet ne répond pas, modifiez sa configuration et recommencez l'association. La [documentation officielle d'ESPSomfy-RTS](https://github.com/rstrouse/ESPSomfy-RTS/wiki) contient davantage de détails sur la configuration.

Les identifiants des volets sont attribués dans l'ordre de leur ajout. Par exemple, les trois premiers volets ont les identifiants `1`, `2` et `3`.

## Lancer le backend en local

Depuis la racine du projet :

```bash
cd backend
python3 -m pip install -r requirements.txt
uvicorn app.main:app --reload
```

L'API locale est disponible à l'adresse `http://127.0.0.1:8000`. La documentation de l'API est disponible à l'adresse `http://127.0.0.1:8000/docs`.

Pour envoyer une commande au volet `1` :

```bash
curl -X POST http://127.0.0.1:8000/shutters/1/down
```

Les commandes disponibles sont `up`, `my` et `down`.

## Simuler une connexion ESP32

Installez [websocat](https://github.com/vi/websocat), démarrez le backend, puis ouvrez un deuxième terminal :

```bash
websocat ws://127.0.0.1:8000/ws/device
```

Après l'ouverture de la connexion, envoyez le message suivant :

```json
{"type":"device_hello","device_id":"esp32-somfy"}
```

Dans un autre terminal, envoyez une commande avec `curl`. Le JSON de la commande est affiché dans le terminal `websocat`.
