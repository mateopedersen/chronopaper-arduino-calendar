# Verification record

Date of record: 2026-10-08

## Available checks

- `tests/calendar-tests/test_calendar.cpp` is a host-native C++ test suite for the pure calendar/date engine. Build with `g++ -std=c++17 -Wall -Wextra -Werror -Ifirmware/ChronoPaper tests/calendar-tests/test_calendar.cpp firmware/ChronoPaper/CalendarEngine.cpp -o /tmp/chronopaper-calendar-tests && /tmp/chronopaper-calendar-tests`.
- `data/calendar_resources.json` is the browser-demo manifest. Its twelve entries were checked for month/year labels and direct destinations in this workspace. The destination titles were checked during preparation on 2026-10-08. Recheck before later releases.
- `web-demo/` is a static application; serve it from the repository root (not from `web-demo`) so the relative `../data/calendar_resources.json` URL resolves. Example: `python3 -m http.server 8000` in the project root, then visit `/web-demo/`.

## Arduino target compile

- Arduino CLI 1.5.1, official Arduino ESP32 core 2.0.18-arduino.5, FQBN `arduino:esp32:nano_nora`, GxEPD2 1.6.9, Adafruit GFX 1.12.6, Adafruit BusIO 1.17.4 and RTClib 2.1.4 compiled the complete sketch successfully with `--warnings all`. The MIT QRCode source is vendored in the sketch directory to avoid a conflicting ESP32-core `qrcode.h`. Compiler reported 603,293 bytes flash (19%) and 53,056 bytes global memory (16%). The vendored upstream C file emits its own pragma/type-limit warnings. Build command: `arduino-cli compile --fqbn arduino:esp32:nano_nora --warnings all firmware/ChronoPaper`.

## Not verified here

- The sketch has not been uploaded to a physical Nano ESP32 or verified on hardware.
- No Nano ESP32, GDEY042T81 panel, DESPI-C02, DS3231 or buttons are attached. Pin behavior, display refresh, QR scanning, RTC fallback, NTP synchronization, local Wi-Fi page and power draw have not been physically tested.
- The wiring diagram is a design reference, not evidence of a built circuit. Confirm panel revision, adapter pin labels and configuration against the current manufacturer instructions before power-up.
- Browser usability and resource-opening behavior require a browser session with JavaScript enabled. Do not treat the static HTML file opened via `file://` as an app test because browser fetch restrictions prevent loading the JSON manifest.
