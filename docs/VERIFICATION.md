# Verification record

Date of record: 2026-10-08

## Available checks

- `tests/calendar-tests/test_calendar.cpp` is a host-native C++ test suite for the pure calendar/date engine. Build with `g++ -std=c++17 -Wall -Wextra -Werror -Ifirmware/ChronoPaper tests/calendar-tests/test_calendar.cpp firmware/ChronoPaper/CalendarEngine.cpp -o /tmp/chronopaper-calendar-tests && /tmp/chronopaper-calendar-tests`.
- `data/calendar_resources.json` is the browser-demo manifest. Its twelve entries were checked for month/year labels and direct destinations in this workspace. The destination titles were checked during preparation on 2026-10-08. Recheck before later releases.
- `web-demo/` is a static application; serve it from the repository root (not from `web-demo`) so the relative `../data/calendar_resources.json` URL resolves. Example: `python3 -m http.server 8000` in the project root, then visit `/web-demo/`.

## Not verified here

- No Arduino CLI or Arduino IDE toolchain is installed in the execution environment. The complete firmware has not been compiled for Nano ESP32.
- No Nano ESP32, GDEY042T81 panel, DESPI-C02, DS3231 or buttons are attached. Pin behavior, display refresh, QR scanning, RTC fallback, NTP synchronization, local Wi-Fi page and power draw have not been physically tested.
- The wiring diagram is a design reference, not evidence of a built circuit. Confirm panel revision, adapter pin labels and configuration against the current manufacturer instructions before power-up.
- Browser usability and resource-opening behavior require a browser session with JavaScript enabled. Do not treat the static HTML file opened via `file://` as an app test because browser fetch restrictions prevent loading the JSON manifest.
