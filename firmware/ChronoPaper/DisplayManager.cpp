#include "DisplayManager.h"
#include "ResourceLinks.h"
#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <qrcode.h>

namespace chrono_paper {
namespace {
constexpr uint8_t kCs = D10, kDc = D9, kReset = D8, kBusy = D7;
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> epd(
    GxEPD2_420_GDEY042T81(kCs, kDc, kReset, kBusy));
const char *const monthNames[] = {"January", "February", "March", "April", "May", "June",
                                  "July", "August", "September", "October", "November", "December"};
const char *const weekdayMonday[] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
const char *const weekdaySunday[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const char *monthName(uint8_t month) { return month >= 1 && month <= 12 ? monthNames[month - 1] : "Invalid"; }
const char *weekdayName(uint8_t sunday0) {
  static const char *const names[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  return sunday0 < 7 ? names[sunday0] : "Unknown";
}

void frame(const char *title, int year) {
  epd.fillScreen(GxEPD_WHITE);
  epd.setTextColor(GxEPD_BLACK);
  epd.setFont(nullptr);
  epd.setTextSize(2);
  epd.setCursor(12, 28);
  epd.print(title);
  epd.setTextSize(1);
  epd.setCursor(350, 22);
  epd.print(year);
  epd.drawFastHLine(12, 39, 376, GxEPD_BLACK);
}

void footer(const Settings &settings) {
  epd.drawFastHLine(12, 282, 376, GxEPD_BLACK);
  epd.setTextSize(1);
  epd.setCursor(12, 295);
  epd.print(settings.weekStart == WeekStart::Monday ? "MONDAY FIRST" : "SUNDAY FIRST");
  epd.setCursor(270, 295);
  epd.print("VIEW "); epd.print(static_cast<uint8_t>(settings.view) + 1); epd.print("/5");
}
} // namespace

void DisplayManager::begin() {
  epd.init(115200, true, 10, false);
  epd.setRotation(0); // 400 x 300 landscape.
  epd.setTextColor(GxEPD_BLACK);
  epd.setFullWindow();
  epd.firstPage();
  do { epd.fillScreen(GxEPD_WHITE); } while (epd.nextPage());
}

void DisplayManager::render(const Settings &settings, const Date &today, bool timeValid,
                            const char *timeSource, bool wifiConnected, bool rtcAvailable) {
  epd.setFullWindow();
  epd.firstPage();
  do { draw(settings, today, timeValid, timeSource, wifiConnected, rtcAvailable); }
  while (epd.nextPage());
  epd.hibernate();
}

void DisplayManager::draw(const Settings &settings, const Date &today, bool timeValid,
                          const char *timeSource, bool wifiConnected, bool rtcAvailable) {
  switch (settings.view) {
  case View::Month: drawMonth(settings, today, timeValid); break;
  case View::Year: drawYear(settings); break;
  case View::Week: drawWeek(settings, today, timeValid); break;
  case View::Resource: drawResource(settings); break;
  case View::Status: drawStatus(settings, timeSource, wifiConnected, rtcAvailable); break;
  }
}

void DisplayManager::drawMonth(const Settings &settings, const Date &today, bool timeValid) {
  char title[20];
  snprintf(title, sizeof(title), "%s %u", monthName(settings.month), settings.year);
  frame(title, settings.year);
  const char *const *labels = settings.weekStart == WeekStart::Monday ? weekdayMonday : weekdaySunday;
  const int left = 12, width = 376, colWidth = width / 7, headerY = 61, gridY = 82, rowHeight = 32;
  epd.setTextSize(1);
  for (int col = 0; col < 7; ++col) {
    epd.setCursor(left + col * colWidth + 4, headerY);
    epd.print(labels[col]);
  }
  epd.drawFastHLine(left, 67, width, GxEPD_BLACK);
  Date cells[42]; uint8_t rows = 0;
  if (!buildMonthGrid(settings.year, settings.month, settings.weekStart, cells, rows)) return;
  const uint8_t cellCount = rows * 7;
  for (uint8_t i = 0; i < cellCount; ++i) {
    const int col = i % 7, row = i / 7;
    const int x = left + col * colWidth, y = gridY + row * rowHeight;
    const Date date = cells[i];
    const bool inMonth = date.year == settings.year && date.month == settings.month;
    const bool isToday = timeValid && date.year == today.year && date.month == today.month && date.day == today.day;
    if (isToday) epd.drawCircle(x + colWidth / 2, y + 7, 10, GxEPD_BLACK);
    if (inMonth) {
      epd.setCursor(x + colWidth / 2 - (date.day < 10 ? 2 : 5), y + 10);
      epd.print(date.day);
    } else {
      epd.drawPixel(x + colWidth / 2, y + 7, GxEPD_BLACK);
    }
    epd.drawFastHLine(x, y + rowHeight - 1, colWidth, GxEPD_BLACK);
  }
  for (int col = 0; col <= 7; ++col) epd.drawFastVLine(left + col * colWidth, gridY, rows * rowHeight, GxEPD_BLACK);
  if (!timeValid) { epd.setCursor(12, 270); epd.print("TIME NOT SET - NAVIGATION STILL WORKS"); }
  footer(settings);
}

void DisplayManager::drawYear(const Settings &settings) {
  frame("YEAR NAVIGATOR", settings.year);
  const int startMonth = settings.yearPage * 3 + 1;
  epd.setTextSize(1);
  epd.setCursor(12, 57);
  epd.print("MONTHS "); epd.print(startMonth); epd.print("-" ); epd.print(startMonth + 2);
  for (int i = 0; i < 3; ++i) {
    const int month = startMonth + i;
    if (month > 12) continue;
    const Date first{settings.year, static_cast<uint8_t>(month), 1};
    const uint8_t sunday0 = weekdaySunday0(first);
    const int y = 94 + i * 57;
    epd.drawRect(12, y - 18, 376, 47, GxEPD_BLACK);
    epd.setTextSize(2); epd.setCursor(22, y); epd.print(monthName(month));
    epd.setTextSize(1); epd.setCursor(205, y - 4); epd.print(daysInMonth(settings.year, month)); epd.print(" days");
    epd.setCursor(205, y + 12); epd.print("begins "); epd.print(weekdayName(sunday0));
    epd.setCursor(310, y + 12); epd.print(naturalWeekRows(settings.year, month, settings.weekStart)); epd.print(" rows");
  }
  footer(settings);
}

void DisplayManager::drawWeek(const Settings &settings, const Date &today, bool timeValid) {
  const Date reference = timeValid ? today : Date{settings.year, settings.month, 1};
  IsoWeek week{};
  frame("WEEK INFORMATION", reference.year);
  if (!isoWeekFor(reference, week)) { footer(settings); return; }
  epd.setTextSize(2); epd.setCursor(24, 84); epd.print("ISO WEEK "); epd.print(week.number);
  epd.setTextSize(1); epd.setCursor(24, 116); epd.print("ISO WEEK-YEAR "); epd.print(week.year);
  epd.setTextSize(2); epd.setCursor(24, 160); epd.print("MONDAY");
  epd.setCursor(230, 160); epd.print("SUNDAY");
  epd.setTextSize(1); epd.setCursor(24, 182); epd.print(week.monday.year); epd.print('-');
  if (week.monday.month < 10) epd.print('0'); epd.print(week.monday.month); epd.print('-');
  if (week.monday.day < 10) epd.print('0'); epd.print(week.monday.day);
  epd.setCursor(230, 182); epd.print(week.sunday.year); epd.print('-');
  if (week.sunday.month < 10) epd.print('0'); epd.print(week.sunday.month); epd.print('-');
  if (week.sunday.day < 10) epd.print('0'); epd.print(week.sunday.day);
  epd.setCursor(24, 222); epd.print("REFERENCE DATE: "); epd.print(reference.year); epd.print('-');
  epd.print(reference.month); epd.print('-'); epd.print(reference.day);
  if (!timeValid) { epd.setCursor(24, 250); epd.print("Using selected month because time is not set."); }
  footer(settings);
}

void DisplayManager::drawResource(const Settings &settings) {
  frame("PRINTABLE RESOURCE", settings.year);
  const PrintableResource resource = printableResource(settings.year, settings.month);
  epd.setTextSize(2); epd.setCursor(12, 72); epd.print(monthName(settings.month));
  epd.setTextSize(1); epd.setCursor(12, 92); epd.print(settings.year); epd.print(" printable calendar");
  if (!resource.verified || resource.url == nullptr) {
    epd.drawRect(12, 118, 240, 76, GxEPD_BLACK);
    epd.setTextSize(2); epd.setCursor(25, 151); epd.print("Printable reference");
    epd.setCursor(25, 176); epd.print("unavailable");
    epd.setTextSize(1); epd.setCursor(12, 218); epd.print("No year-matched destination has been verified.");
    footer(settings);
    return;
  }
  epd.setCursor(12, 126); epd.print("Scan to open the verified month reference.");
  epd.setCursor(12, 146); epd.print("Source: Beta Calendars");
  epd.setCursor(12, 172); epd.print("Printable version opens in your browser.");
  QRCode qr{};
  uint8_t qrStorage[256]{};
  const int8_t result = qrcode_initText(&qr, qrStorage, 4, ECC_MEDIUM, resource.url);
  if (result == 0) {
    const int scale = 2;
    const int quiet = 4;
    const int x0 = 282, y0 = 82;
    const int size = qr.size + quiet * 2;
    epd.fillRect(x0, y0, size * scale, size * scale, GxEPD_WHITE);
    for (uint8_t y = 0; y < qr.size; ++y)
      for (uint8_t x = 0; x < qr.size; ++x)
        if (qrcode_getModule(&qr, x, y))
          epd.fillRect(x0 + (x + quiet) * scale, y0 + (y + quiet) * scale, scale, scale, GxEPD_BLACK);
    epd.drawRect(x0 - 1, y0 - 1, size * scale + 2, size * scale + 2, GxEPD_BLACK);
  } else {
    epd.setCursor(282, 125); epd.print("QR encode error");
  }
  footer(settings);
}

void DisplayManager::drawStatus(const Settings &settings, const char *timeSource,
                               bool wifiConnected, bool rtcAvailable) {
  frame("STATUS & SETTINGS", settings.year);
  epd.setTextSize(1);
  epd.setCursor(18, 72); epd.print("Wi-Fi"); epd.setCursor(160, 72); epd.print(wifiConnected ? "CONNECTED (LOCAL NETWORK)" : "OFFLINE");
  epd.setCursor(18, 103); epd.print("Time source"); epd.setCursor(160, 103); epd.print(timeSource);
  epd.setCursor(18, 134); epd.print("DS3231"); epd.setCursor(160, 134); epd.print(rtcAvailable ? "DETECTED; STORES UTC" : "NOT DETECTED / OPTIONAL");
  epd.setCursor(18, 165); epd.print("Week begins"); epd.setCursor(160, 165);
  epd.print(settings.weekStart == WeekStart::Monday ? "MONDAY" : "SUNDAY");
  epd.setCursor(18, 196); epd.print("Controls"); epd.setCursor(160, 196); epd.print("D2 PREV / D3 NEXT / D4 VIEW");
  epd.setCursor(160, 212); epd.print("Hold D4 to change week start.");
  epd.setCursor(18, 247); epd.print("E-paper refresh on user action; no rapid animation.");
  footer(settings);
}

} // namespace chrono_paper
