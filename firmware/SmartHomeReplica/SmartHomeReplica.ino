\
/*
  Smart Home Replica (Bluetooth-Controlled)
  - Arduino Uno/Nano
  - HC-05 at 9600 bps (default)
  - DHT22 on D7
  - Relay/LED on D8
  - Servo on D9
  - HC-05: SoftwareSerial RX=D10, TX=D11
  - MQ-2 analog A0, LDR analog A1

  Protocol: newline-terminated ASCII commands (see docs/commands.md)
*/

#include <SoftwareSerial.h>
#include <Servo.h>
#include <DHT.h>

// -------- Pins --------
const uint8_t PIN_BT_RX = 10; // Arduino receives on D10 from HC-05 TXD
const uint8_t PIN_BT_TX = 11; // Arduino transmits on D11 to HC-05 RXD
const uint8_t PIN_DHT   = 7;
const uint8_t PIN_LIGHT = 8;
const uint8_t PIN_SERVO = 9;
const uint8_t PIN_MQ2   = A0;
const uint8_t PIN_LDR   = A1;

// -------- Config --------
#define DHTTYPE DHT22
const unsigned long SENSOR_READ_INTERVAL_MS = 1000;
const int SERVO_OPEN_DEG  = 90;
const int SERVO_CLOSE_DEG = 0;
String AUTH_PIN = "1234"; // change for production

// -------- Globals --------
SoftwareSerial bt(PIN_BT_RX, PIN_BT_TX);
Servo doorServo;
DHT dht(PIN_DHT, DHTTYPE);

bool isAuthed = false;
bool lightOn = false;
bool doorOpen = false;

unsigned long lastSensorMs = 0;
float lastT = NAN, lastRH = NAN;
int lastGas = 0, lastLdr = 0;

String buf;

void setLight(bool on) {
  lightOn = on;
  digitalWrite(PIN_LIGHT, on ? HIGH : LOW);
}

void setDoor(bool open) {
  doorOpen = open;
  doorServo.write(open ? SERVO_OPEN_DEG : SERVO_CLOSE_DEG);
  delay(300); // brief move; keep minimal to avoid blocking too much
}

void replyOK() { bt.println("OK"); }
void replyERR(const char* reason) { bt.print("ERR "); bt.println(reason); }

void replyStatus() {
  bt.print("{\"auth\": ");
  bt.print(isAuthed ? "true" : "false");
  bt.print(", \"light\": \"");
  bt.print(lightOn ? "ON" : "OFF");
  bt.print("\", \"door\": \"");
  bt.print(doorOpen ? "OPEN" : "CLOSED");
  bt.println("\"}");
}

void replyEnv() {
  bt.print("{\"t_c\": ");
  if (isnan(lastT)) bt.print("null"); else bt.print(lastT, 1);
  bt.print(", \"rh\": ");
  if (isnan(lastRH)) bt.print("null"); else bt.print(lastRH, 1);
  bt.print(", \"gas_raw\": "); bt.print(lastGas);
  bt.print(", \"ldr_raw\": "); bt.print(lastLdr);
  bt.println("}");
}

String trimLower(String s) {
  s.trim();
  s.toLowerCase();
  return s;
}

void handleLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  // tokenize
  String cmd = line;
  int sp = cmd.indexOf(' ');
  String arg = "";
  if (sp >= 0) {
    arg = cmd.substring(sp + 1);
    cmd = cmd.substring(0, sp);
  }
  cmd.toUpperCase();

  if (cmd == "HELP") {
    bt.println("CMDS: AUTH <PIN>, LIGHT ON|OFF, DOOR OPEN|CLOSE, READ ENV, STATUS, HELP");
    return;
  }
  if (cmd == "AUTH") {
    arg.trim();
    if (arg.length() == 0) { replyERR("PIN?"); return; }
    if (arg == AUTH_PIN) { isAuthed = true; replyOK(); }
    else { isAuthed = false; replyERR("BAD_PIN"); }
    return;
  }
  if (cmd == "STATUS") { replyStatus(); return; }
  if (cmd == "READ") {
    arg.trim(); arg.toUpperCase();
    if (arg == "ENV") { replyEnv(); return; }
    replyERR("READ?");
    return;
  }
  if (cmd == "LIGHT") {
    if (!isAuthed) { replyERR("UNAUTH"); return; }
    arg.trim(); arg.toUpperCase();
    if (arg == "ON") { setLight(true); replyOK(); return; }
    if (arg == "OFF") { setLight(false); replyOK(); return; }
    replyERR("LIGHT?");
    return;
  }
  if (cmd == "DOOR") {
    if (!isAuthed) { replyERR("UNAUTH"); return; }
    arg.trim(); arg.toUpperCase();
    if (arg == "OPEN") { setDoor(true); replyOK(); return; }
    if (arg == "CLOSE") { setDoor(false); replyOK(); return; }
    replyERR("DOOR?");
    return;
  }
  replyERR("UNKNOWN");
}

void setup() {
  pinMode(PIN_LIGHT, OUTPUT);
  setLight(false);

  doorServo.attach(PIN_SERVO);
  setDoor(false);

  dht.begin();

  Serial.begin(115200);
  bt.begin(9600); // HC-05 default
  Serial.println("SmartHomeReplica boot");
  bt.println("SmartHomeReplica ready");
}

void loop() {
  // --- Sensor refresh (non-blocking) ---
  unsigned long now = millis();
  if (now - lastSensorMs >= SENSOR_READ_INTERVAL_MS) {
    lastSensorMs = now;
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t)) lastT = t;
    if (!isnan(h)) lastRH = h;
    lastGas = analogRead(PIN_MQ2);
    lastLdr = analogRead(PIN_LDR);
  }

  // --- Read incoming from BT ---
  while (bt.available()) {
    char c = (char)bt.read();
    if (c == '\r') continue;
    if (c == '\n') {
      handleLine(buf);
      buf = "";
    } else {
      if (buf.length() < 128) buf += c; // rudimentary guard
    }
  }

  // Optional: forward to USB serial for debugging
  while (Serial.available()) {
    bt.write(Serial.read());
  }
}
