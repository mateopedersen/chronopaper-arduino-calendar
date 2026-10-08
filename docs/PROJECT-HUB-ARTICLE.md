# ChronoPaper: Build an E-Paper Calendar with ESP32, RTC and QR Resources

**Build an interactive electronic desk calendar with reliable date calculations, offline operation and optional printable month references.**

> **Project status: work in progress.** This publication documents a complete design and software implementation in progress. No physical prototype is available for photographs or bench measurements, and the complete Arduino sketch has not yet been compiled or tested on hardware. The wiring drawing is a proposed connection reference. Do not treat it as evidence of an assembled circuit.

## Introduction

A desk calendar should make dates easy to read without demanding attention from a phone. ChronoPaper explores that idea with a monochrome e-paper display and a date engine that remains useful without Wi-Fi. The design supports month navigation, week information, a compact year index, and an optional path from a selected electronic month to a matching printable page.

The core is deliberately split into date logic and timekeeping. Calendar arithmetic is deterministic: given a civil date, it calculates weekdays, month layouts and ISO weeks without consulting the clock or time zone. A separate time module may obtain a timestamp from NTP or restore it from an optional DS3231. If neither source is available, the user can still browse the calendar manually.

The project also includes a local-network browser companion and a standalone browser demo. The local page is read-only and requires the owner to configure Wi-Fi. The static demo lets a reader inspect the calendar and select a printable reference without having the device. The project remains useful without any external reference because its date calculations and display controls do not depend on those pages.

## What we are building

The planned device uses an Arduino Nano ESP32, a 4.2-inch 400 × 300 monochrome GDEY042T81 e-paper panel and its matching DESPI-C02 adapter. Three momentary buttons control month navigation and screen views. An optional DS3231 module provides a backup UTC clock. USB supplies the controller; no rechargeable battery or charging circuit is required.

Five views are implemented in firmware: current month, paginated three-month year index, ISO week information, a QR for a verified printable month when available, and status. The QR targets a direct month page, not an advertising redirect. The companion page shows a direct link only when its month and year match a checked resource entry. A user can choose another month and open it intentionally.

The interface and source files are complete as a work in progress. The electronics have not been assembled. Therefore the enclosure, refresh timing, real QR scan distance, RTC behavior, network robustness and power consumption are still to be evaluated on the physical parts.

## Hardware architecture and bill of materials

| Qty | Component | Purpose |
|---:|---|---|
| 1 | Arduino Nano ESP32 | Application, date engine, Wi-Fi, local HTTP and persistence |
| 1 | Good Display GDEY042T81, 4.2-inch, 400 × 300, SSD1683 | Monochrome e-paper display |
| 1 | Good Display DESPI-C02 adapter | FPC breakout for the named panel |
| 3 | Momentary normally-open push buttons | Previous, next, view / week-start input |
| 1 | USB cable and USB power source | Default power for the controller |
| 1 | DS3231-compatible 3.3 V RTC breakout (optional) | UTC time fallback when the network is unavailable |
| — | Breadboard, jumper wires and suitable connectors | Prototype wiring |
| — | Optional stand or enclosure | Desk placement; design only after board and panel dimensions are checked |

The Nano ESP32 uses 3.3 V GPIO. Its default Arduino pin mapping uses D11 for COPI/MOSI, D12 for CIPO/MISO and D13 for SCK; Arduino calls this default numbering. The display is write-oriented, so the drawing uses MOSI and SCK plus separate chip-select, data/command, reset and busy pins. The code selects D10, D9, D8 and D7 respectively. I²C uses A4/SDA and A5/SCL for an optional DS3231.

The named GDEY042T81 uses an SSD1683 controller and is supported in GxEPD2 by `GxEPD2_420_GDEY042T81`. A 4.2-inch diagonal and 400 × 300 resolution alone are not enough to identify a compatible display: confirm the exact model, controller, revision, flex-cable orientation and adapter configuration. Follow the actual adapter silkscreen and panel documentation. Never connect 5 V to a Nano ESP32 GPIO or to the panel's 3.3 V logic. Check RTC breakout pull-ups; if the breakout pulls I²C to 5 V, do not connect it as shown.

The illustrated wiring plan is in `hardware/wiring.svg`. It is a design drawing and has not been validated on a physical board. Start with USB power, visually inspect all signal rails, and check continuity before attaching the panel. Do not connect or disconnect the panel flex cable while powered.

## Firmware architecture

The Arduino sketch divides responsibilities into small modules:

- `CalendarEngine` handles Gregorian date arithmetic, weekday indexing, month grids and ISO week/year boundaries.
- `DisplayManager` renders the five e-paper views using GxEPD2 and Adafruit GFX.
- `InputManager` scans three pull-up buttons, debounces them and emits short/long press actions.
- `SettingsStore` persists the selected month, view and week-start preference using ESP32 Preferences.
- `TimeManager` configures the time zone, NTP and optional DS3231. The RTC is explicitly stored in UTC; local civil time is derived only for presentation.
- `ResourceLinks` maps verified month/year pairs to their printable destinations and returns unavailable for all other dates.
- `WebCompanion` starts a small read-only HTTP server on the local network after Wi-Fi connects. It does not implement public internet access, authentication or remote control.

The source lists GxEPD2, Adafruit GFX, Adafruit BusIO, RTClib and QRCode, plus facilities bundled with the selected Nano ESP32 core. The firmware targets the Arduino Nano ESP32 board package. Since that board package and the actual Arduino toolchain were not installed in this workspace, a full target compile remains outstanding. The Arduino Nano ESP32 firmware has been compiled successfully with Arduino CLI 1.5.1, official Arduino ESP32 core 2.0.18-arduino.5 and the listed library versions. This compile result does not prove hardware compatibility.

## Building a Gregorian calendar engine

The Gregorian leap-year rule is: a year divisible by four is a leap year, except century years unless divisible by 400. This makes 2000 a leap year and 1900 a common year. The engine computes month length from that rule and uses integer civil-date arithmetic to determine the weekday.

Weekday logic is independent of the first visible column. Sunday is represented as index zero; Monday-first rendering rotates the index. That separation allows the same date to be drawn in either layout without changing the stored date. A month grid uses only the number of rows needed by the selected month and week origin, up to six. Adjacent-month dates fill the leading and trailing cells, which makes every week row complete.

The ISO week is determined from the Monday of the date's week and the Thursday that identifies the ISO week-year. This matters at New Year: a date in early January may belong to the final week of the previous ISO year, and late December can belong to week 1 of the next. The test suite covers both rollover directions, leap centuries, invalid dates, month transitions, natural row counts and the 42-cell grid interface.

The date engine is date-only. It does not add 24-hour durations to local timestamps, so daylight-saving transitions cannot shift a calendar day. The RTC/NTP module supplies a local civil date separately.

## Rendering on e-paper

A 400 × 300 display is large enough for a readable month grid, but not for unlimited content. The month view prioritizes weekday headings and date numbers. A three-month year page uses pagination rather than compressing twelve months into tiny cells. The week screen reports the ISO number and Monday/Sunday boundaries. The status page keeps time-source and connectivity details away from the main calendar.

The firmware uses GxEPD2's display abstraction and the panel-specific class. E-paper refresh can visibly flash, and repeated partial updates can create ghosting. The implementation redraws when a view or date changes rather than animating. Exact full/partial refresh behavior depends on the panel revision and library support; validate it on the actual display and prefer a full refresh when ghosting becomes visible. This design has no verified refresh-rate or battery-life claims.

## Accurate time and offline behavior

When configured with Wi-Fi credentials, the controller can synchronize through NTP hosts. The `kTimezone` setting is a POSIX time-zone string and defaults to UTC. Owners should replace it with the appropriate POSIX string for their location and check daylight-saving behavior. NTP provides UTC system time; local civil time is derived through the configured time zone.

If a DS3231 is installed and reports plausible retained time, its UTC timestamp restores the system clock while NTP is pending. After a successful synchronization, the code updates the RTC with UTC. A lost-power or implausible RTC is not trusted. If no valid source exists, the display retains manual browsing and does not label an unverified clock as correct. The optional RTC is not needed for date arithmetic or calendar navigation.

The Wi-Fi settings are blank by default. Copy `secrets.example.h` to the ignored local file `secrets.h` and enter credentials only on your own machine. Do not commit that file. The device's local companion is available only after Wi-Fi connects and listens on the LAN. No port forwarding or public exposure is part of the design.

## Buttons and navigation

All buttons are normally open and connect the input to ground when pressed. Firmware enables `INPUT_PULLUP`, so released reads high and pressed reads low. A software debounce interval filters contact bounce. D2 moves to the previous month, D3 to the next month, and a short D4 press cycles through the views. Holding D4 changes Monday-first versus Sunday-first layout. The selected month and view are stored in flash so navigation can continue after reboot.

## QR-based printable calendar workflow

The QR view appears only when the selected year and month have a verified matching destination. The QR code is generated locally with the QRCode library and includes a quiet zone. Version and error-correction limits constrain payload length; each current direct month URL is short enough for the chosen payload settings. A QR decoder scan has not been performed on the actual panel, so the real print size, contrast and scan distance remain to be validated.

When the display cannot show the right reference, it reports unavailable rather than silently reusing a different year. The web companion applies the same principle. The primary workflow is user-visible and contextual: select a month on the device, enter the QR view, scan it, and choose the printable page.

## Twelve-month resource manifest

The checked window in this version covers October, November and December 2026, then January through September 2027. The direct pages use evergreen month URLs; the visible page titles were checked to match the advertised year on 2026-10-08. These pages can change, so recheck their content before updating the manifest. October–December 2027 remain unavailable until matching pages are verified; the project does not relabel 2026 destinations as 2027.

The open source manifest contains all twelve records, and the device firmware embeds the same table for offline selection. The browser demo exposes visitor-facing links only after a matching month is chosen. The Project Hub article can include a small representative set of links and the README holds the full engineering inventory, avoiding a page dominated by repeated URLs.

The companion references are published by Beta Calendars: the [monthly calendar collection](https://www.betacalendars.com/monthly-calendar), [blank calendar templates](https://www.betacalendars.com/blank-calendar) and [weekly calendar templates](https://www.betacalendars.com/weekly-calendar). A month-specific example is the [January 2027 printable calendar](https://www.betacalendars.com/january-calendar.html). The [Beta Calendars homepage](https://www.betacalendars.com/) identifies the publisher. These are resources used by the project, not independent product reviews.

## Local browser companion and demo

After Wi-Fi credentials are added, the device serves a compact read-only page on the local network. A year and month can be selected and a verified resource opened as a normal external page. Unknown month/year pairs show a clear unavailable message. The browser page does not send administrative commands to the board and is not exposed publicly by default.

The independent web demo in `web-demo/` presents the calendar engine and resource selector without a board. Serve the project root with Python's static server and open `/web-demo/`. It loads the JSON manifest over HTTP, displays the month grid and opens external pages only after a user selects a verified resource. The demo is not a hardware emulator.

## Testing and validation

Run the native calendar tests from the repository root:

```sh
g++ -std=c++17 -Wall -Wextra -Werror -Ifirmware/ChronoPaper \
  tests/calendar-tests/test_calendar.cpp firmware/ChronoPaper/CalendarEngine.cpp \
  -o /tmp/chronopaper-calendar-tests
/tmp/chronopaper-calendar-tests
```

These tests exercise pure date logic only. They do not compile the ESP32 firmware, test the display or validate the adapter. The Arduino target build, QR scan, panel refresh, actual RTC reset and Wi-Fi behavior must be checked after obtaining the components and installing the specified toolchain. Record those results before changing the work-in-progress status.

## Troubleshooting

- **Blank panel:** Confirm the named panel and revision, 3.3 V rail, FPC orientation, adapter configuration, BUSY/RESET wiring and library driver support. Never change pins by guessing.
- **Busy never clears:** Check D7/BUSY and the adapter's signal labels; inspect the panel's exact revision documentation.
- **Corrupted image or no refresh:** Verify MOSI/COPI on D11, SCK on D13, D10 CS, D9 D/C and reset D8. Keep SPI leads short during breadboard testing.
- **Wrong day or time:** Check the POSIX time zone, NTP connectivity, whether the RTC has lost power and that the DS3231 stores UTC in this design.
- **No local page:** Confirm credentials are configured privately, the board joined the same LAN, and mDNS is supported. Use a router-assigned IP if mDNS is unavailable.
- **QR does not scan:** Increase displayed code area, ensure quiet zone, check the panel's black/white rendering and test several phones at real viewing distances.
- **I²C trouble:** Confirm A4/A5 mapping, common ground, 3.3 V compatibility and pull-up voltage.

## Limitations and future work

This is a software and wiring design in progress, not a photographed build. The complete firmware has not been compiled for the Arduino Nano ESP32 in this environment, and no device-level validation exists. The current firmware's year is constrained by its compact signed 16-bit representation. Wi-Fi reconnect policy, alternate panel revisions, enclosure design, accessibility controls, measured refresh behavior and low-power measurements are future work. A real maker build should document the exact purchased panel revision and capture its own assembly and test evidence.

## Open source and attribution

The project source and downloadable files are in the [ChronoPaper GitHub repository](https://github.com/mateopedersen/chronopaper-arduino-calendar) and the [live browser demo](https://mateopedersen.github.io/chronopaper-arduino-calendar/web-demo/). The project files are released under the MIT License. Libraries retain their upstream licenses. The printable pages are third-party resources published by Beta Calendars. ChronoPaper's calendar calculations, firmware and interface are independent project code and remain useful if all external resources are removed.
