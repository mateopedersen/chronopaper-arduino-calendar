# ChronoPaper: an e-paper calendar with a verifiable printable companion

ChronoPaper is a proposed open-hardware desk calendar built around the Arduino Nano ESP32 and the 4.2-inch Good Display GDEY042T81 panel. It combines an offline date-only Gregorian engine, optional network time, an optional DS3231 backup clock, several e-paper views, three physical controls, and a local read-only page that opens year-matched printable calendar references.

> **Build status: work in progress.** The source, resource manifest, interface demo, wiring reference, and testable calendar engine are prepared. No assembled device is available in this workspace. The complete firmware has not been compiled against the Arduino toolchain or tested on the specified board and panel. Treat the wiring illustration as a proposed connection plan; check the exact panel revision and adapter documentation before powering hardware.

## What it does

- Draws month grids with Monday or Sunday as the first weekday.
- Shows ISO 8601 week numbers, 3-month year pages, an ISO week view, printable-reference QR codes, and status.
- Stores the selected year, month, view, and week origin in ESP32 Preferences.
- Uses buttons for previous month, next month, cycling views, and changing the week start.
- Can obtain time over Wi-Fi/NTP. If a DS3231 is installed, valid UTC time is restored on boot and updated after successful NTP synchronization.
- Serves a small read-only companion page at `http://chronopaper.local/` after the user supplies Wi-Fi credentials. It is available only to clients on the same local network.
- Shows direct printable links only for a checked month/year pair. Other dates remain usable and show that no matching reference has been verified.
- Includes a browser demo of the date engine and direct resource selection. It is illustrative and is not a hardware emulator.

## Hardware and bill of materials

| Item | Part | Required | Notes |
|---|---|---:|---|
| Controller | Arduino Nano ESP32 | Yes | Use Arduino pin numbering. GPIO and IO are 3.3 V. |
| E-paper panel | Good Display GDEY042T81, 4.2-inch, 400×300, SSD1683 | Yes | Confirm the exact panel revision before selecting a driver. |
| Adapter | Good Display DESPI-C02 | Yes | The panel uses a 24-pin FPC; check connector orientation and adapter configuration. |
| Buttons | 3 momentary normally-open push buttons | Yes | Wire each between D2/D3/D4 and GND; internal pull-ups are enabled. |
| RTC | DS3231-compatible 3.3 V breakout | Optional | SDA=A4, SCL=A5. Use only a breakout with I²C pull-ups appropriate for 3.3 V. |
| Supply | USB power for Nano ESP32 | Yes | Keep the e-paper logic and supply at 3.3 V as specified by the panel/adapter. |
| Enclosure / stand | User selected | Optional | Leave the panel flex cable unstrained and accessible during bring-up. |

The sketch targets `GxEPD2_420_GDEY042T81` with `GxEPD2`. Install the board package and libraries shown in `firmware/ChronoPaper/library-dependencies.txt`. The display adapter/panel must be powered and wired per its own revision-specific documentation. **Never connect 5 V to the Nano ESP32 GPIO, panel, or adapter logic.** Do not infer panel pin order from the SVG; use the exact adapter silkscreen and datasheet.

### Wiring reference

Open [the wiring diagram](hardware/wiring.svg) in a browser or vector editor. SPI is D11/COPI, D13/SCK, with D10 chip select, D9 data/command, D8 reset and D7 busy. The Nano ESP32's I²C pins are A4/SDA and A5/SCL. The firmware uses default Arduino pin numbering. Check the board mapping and panel adapter before connecting power.

## Firmware setup

1. Install Arduino IDE 2.x and select **Arduino Nano ESP32** from Arduino's official board package.
2. Install the libraries listed in `firmware/ChronoPaper/library-dependencies.txt` using Library Manager or their upstream repositories.
3. Copy `firmware/ChronoPaper/secrets.example.h` to `firmware/ChronoPaper/secrets.h`. Enter your own 2.4 GHz Wi-Fi SSID and password locally. The real file is ignored by Git; never publish credentials.
4. Open `firmware/ChronoPaper/ChronoPaper.ino`, select the Nano ESP32 board and its serial port, then compile and upload.
5. With the panel disconnected, check continuity and verify there is no 5 V path to any GPIO or e-paper signal. Wire the named panel and adapter according to their own instructions, then power through USB.
6. If the display remains blank, check panel revision, FPC orientation, adapter configuration, busy/reset polarity, and library support before changing refresh parameters.
7. Add the optional DS3231 after confirming that breakout's operating voltage and pull-up rail. A backup coin cell is optional; follow its manufacturer's charging/chemistry rules.
8. Only after Wi-Fi has been configured, join the same LAN from a browser and open `http://chronopaper.local/`. mDNS behavior varies by router; the IP address in the serial monitor can be used instead if added during bring-up.

By default the SSID/password are blank. Therefore Wi-Fi, NTP, the local companion, and QR-driven online links remain off until the owner configures credentials. The calendar display and navigation are designed to work without the network.

### Controls

- D2: previous month.
- D3: next month.
- D4 short press: next view.
- D4 long press: switch between Monday-first and Sunday-first.

Buttons use `INPUT_PULLUP`, so each switch connects its input pin to ground when pressed. The firmware debounces transitions in software.

## Calendar and time model

The embedded `CalendarEngine` treats dates as civil calendar values and contains no timezone or wall-clock dependency. This keeps leap-year, weekday, month rollover, month-grid and ISO week calculations deterministic. The `TimeManager` separately obtains a date from a plausible RTC/NTP timestamp, applies a configurable POSIX timezone, and passes the resulting local civil date to the display. The DS3231 stores UTC in this implementation. Change `kTimezone` in the sketch to the POSIX timezone for the installation; verify daylight-saving rules for the location.

The demo and native tests cover years 1–9999 for date arithmetic. The firmware's compact `Date` stores a signed 16-bit year. Boundary behavior above that representable range is consequently a host/demo feature, not a supported display range.

## Printable reference policy

`data/calendar_resources.json` is the single browser-demo resource manifest. Embedded firmware keeps the same known mapping in `ResourceLinks.cpp`. The current set covers October–December 2026 and January–September 2027. Each direct destination uses a month-specific evergreen URL whose page title was checked to match its year on 2026-10-08. The page itself may change later; review its title/content again before extending or refreshing the mapping.

ChronoPaper deliberately does not map October–December 2027 to a similarly named 2026 page. A page can only be added after the specific year is verified. The printable calendar is a companion reference; it does not supply the firmware's date calculations.

## Browser demo

Serve this folder over HTTP so the JSON manifest can load:

```sh
cd web-demo
python3 -m http.server 8000
```

Then open `http://localhost:8000`. Select a month and year; use left/right arrow keys to change months, up/down to change year, and `W` to toggle the first weekday. The demo does not contact a device. It opens a third-party page only when a matching reference is selected.

## Repository layout

- `firmware/ChronoPaper/` — Arduino sketch and components.
- `data/calendar_resources.json` — verified monthly references for the browser demo.
- `hardware/wiring.svg` — proposed connection diagram.
- `web-demo/` — static interactive calendar.
- `tests/calendar-tests/` — native C++ tests for pure date math.
- `docs/` — build record and verification notes.

## Verification and limitations

The date engine is designed to be compiled and tested natively with the included `tests/calendar-tests/test_calendar.cpp`; see `docs/VERIFICATION.md` for the actual run record. This is not a substitute for compiling the complete Arduino sketch or testing SPI refresh, QR readability, RTC recovery, Wi-Fi reconnection, mDNS, enclosure fit, or power draw on hardware. Those remain open build tasks. No physical-project photographs are included; the interface demo is explicitly labeled illustrative.

## Upstream references

- [Arduino Nano ESP32 product page](https://store.arduino.cc/products/nano-esp32)
- [Arduino Nano ESP32 pin-numbering guidance](https://support.arduino.cc/hc/en-us/articles/10483225565980-Select-pin-numbering-for-Nano-ESP32-in-Arduino-IDE)
- [Good Display GDEY042T81 panel](https://www.good-display.com/product/386.html)
- [Good Display DESPI-C02 adapter](https://www.good-display.com/product/516.html)
- [GxEPD2 display library](https://github.com/ZinggJM/GxEPD2)
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit RTClib](https://github.com/adafruit/RTClib)
- [QRCode library](https://github.com/ricmoo/QRCode)
- [Beta Calendars monthly collection](https://www.betacalendars.com/monthly-calendar)
- [Beta Calendars blank templates](https://www.betacalendars.com/blank-calendar)
- [Beta Calendars weekly calendars](https://www.betacalendars.com/weekly-calendar)

## License

Software and documentation: MIT. Upstream libraries and linked printable resources retain their own licenses and terms. The panel, board and adapters are third-party hardware.
