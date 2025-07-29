# Smart Home Replica Model (Bluetooth-Controlled)

A compact smart-home **replica** that demonstrates door access, lighting control, and environmental monitoring, all controlled over **Bluetooth Classic** from an Android app. The system integrates common sensors/actuators and a microcontroller to blend **theory** with **hands‑on** learning in electronics and automation.

## Features
- 🔐 **Door access control:** servo-driven lock with authenticated commands.
- 💡 **Lighting control:** relay/LED channel with manual and remote modes.
- 🌡️ **Environmental monitoring:** temperature & humidity (DHT22), gas (MQ‑2), and ambient light (LDR).
- 📱 **Android app:** connects to HC‑05/HC‑06 and issues simple text commands.
- 🔒 **Session auth:** `AUTH <PIN>` gate for write operations (default `1234`, configurable in firmware).
- 🧰 **Text protocol:** human-readable, newline‑delimited commands and JSON replies.

## Repository Structure
```
smart-home-replica/
├─ firmware/SmartHomeReplica/SmartHomeReplica.ino
├─ mobile-app/android-kotlin/                # Minimal Android app
│  ├─ app/src/main/java/com/example/smarthome/MainActivity.kt
│  ├─ app/src/main/res/layout/activity_main.xml
│  ├─ app/src/main/AndroidManifest.xml
│  ├─ app/build.gradle
│  ├─ build.gradle
│  ├─ settings.gradle
├─ hardware/
│  ├─ wiring.md
│  └─ bom.md
├─ docs/
│  └─ commands.md
├─ README.md
└─ LICENSE
```

## Quick Start

### 1) Hardware (Arduino‑class MCU + HC‑05)
- Upload `firmware/SmartHomeReplica.ino` to an **Arduino Uno/Nano** (or equivalent 5V MCU).
- Connect **HC‑05** Bluetooth module (default 9600 bps). See `hardware/wiring.md`.
- Sensors: **DHT22** (temp/humidity), **MQ‑2** (gas, analog), **LDR** (analog). Actuators: **Relay/LED** for light, **Servo** for lock.

### 2) Android App (Kotlin)
- Open `mobile-app/android-kotlin` in **Android Studio**.
- Pair your phone with the HC‑05 (PIN usually `1234` or `0000`), then **Run** the app.
- In the app: tap **Connect**, choose your HC‑05, then use the buttons to control and **Read Env**.

### 3) Command Protocol
See `docs/commands.md`. Examples:
```
AUTH 1234
LIGHT ON
READ ENV
DOOR OPEN
STATUS
```

## Safety & Notes
- Gas/temperature readings are **for demonstration** only; **do not** rely on them for safety‑critical use.
- Use level shifting / proper power design as needed. Keep high‑voltage relay wiring isolated.

## License
MIT — see `LICENSE`.
