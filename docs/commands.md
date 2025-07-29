# Serial/Bluetooth Command Protocol

- Encoding: ASCII, newline (`\n`) terminated
- Case-insensitive keywords; arguments space-separated
- Replies: `OK`, `ERR <reason>`, or JSON for data

## Authentication
```
AUTH <PIN>
```
- Required for *write* commands (LIGHT, DOOR). Default PIN is `1234` (change in firmware).

## Lighting
```
LIGHT ON
LIGHT OFF
```

## Door Lock (Servo)
```
DOOR OPEN
DOOR CLOSE
```

## Environment Readings
```
READ ENV
```
Reply JSON:
```json
{"t_c": 24.1, "rh": 53.2, "gas_raw": 412, "ldr_raw": 721}
```

## Status
```
STATUS
```
Reply JSON:
```json
{"auth": true, "light": "ON", "door": "CLOSED"}
```

## Help
```
HELP
```
