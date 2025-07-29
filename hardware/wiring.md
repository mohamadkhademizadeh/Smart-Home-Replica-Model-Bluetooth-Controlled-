# Wiring Guide (Arduino Uno reference)

> Adjust pins as needed; keep relay/high-voltage wiring isolated from low-voltage logic.

| Module      | Arduino Pin | Notes                               |
|-------------|-------------|-------------------------------------|
| HC-05 TXD   | D10 (SoftwareSerial RX) | 5V tolerant; add divider if module is 3.3V |
| HC-05 RXD   | D11 (SoftwareSerial TX) | Use voltage divider to 3.3V if required    |
| HC-05 VCC   | 5V          |                                      |
| HC-05 GND   | GND         |                                      |
| DHT22 DATA  | D7          | Pull-up 10kΩ recommended             |
| MQ-2 AOUT   | A0          | Analog gas sensor raw value          |
| LDR         | A1          | LDR + 10kΩ divider (to 5V/GND)      |
| Relay/LED   | D8          | Active HIGH (adjust for your board)  |
| Servo (lock)| D9 (PWM)    | External 5V recommended; common GND  |

### Libraries (Arduino IDE -> Library Manager)
- **DHT sensor library** by Adafruit (for DHT22)
- **Adafruit Unified Sensor** (dependency)
- **Servo** (built-in)

### Power
- Recommended: Separate 5V supply for servo/relay with common ground.
