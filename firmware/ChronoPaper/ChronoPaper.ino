#include <Arduino.h>
#include <WiFi.h>
#include <cstring>
#include "CalendarEngine.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "SettingsStore.h"
#include "TimeManager.h"
#include "WebCompanion.h"

#if __has_include("secrets.h")
#include "secrets.h" // Local-only file; never commit Wi-Fi credentials.
#else
#define CHRONOPAPER_WIFI_SSID ""
#define CHRONOPAPER_WIFI_PASSWORD ""
#endif

using namespace chrono_paper;

namespace {
constexpr char kTimezone[] = "UTC0"; // Replace with a POSIX TZ string for the installation locale.
SettingsStore settingsStore;
TimeManager timeManager;
InputManager inputManager;
DisplayManager displayManager;
WebCompanion webCompanion;
Settings settings{2026, 10, WeekStart::Monday, View::Month, 0};
Date currentDate{2026, 10, 1};
uint32_t lastDisplayRefresh = 0;
bool haveValidTime = false;
bool webStarted = false;
uint8_t localHour = 0, localMinute = 0, localSecond = 0;

void refreshScreen() {
  displayManager.render(settings, currentDate, haveValidTime, timeManager.sourceLabel(),
                        WiFi.status() == WL_CONNECTED, timeManager.rtcAvailable());
  lastDisplayRefresh = millis();
}

void selectAdjacentMonth(int direction) {
  const Date selected{settings.year, settings.month, 1};
  Date next{};
  const bool moved = direction < 0 ? previousMonth(selected, next) : nextMonth(selected, next);
  if (!moved) return;
  settings.year = next.year;
  settings.month = next.month;
  settings.yearPage = static_cast<uint8_t>((settings.month - 1) / 3);
  settingsStore.save(settings);
}

void applyAction(ButtonAction action) {
  if (action == ButtonAction::None) return;
  if (action == ButtonAction::Previous) selectAdjacentMonth(-1);
  if (action == ButtonAction::Next) selectAdjacentMonth(1);
  if (action == ButtonAction::CycleView) settings.view = static_cast<View>((static_cast<uint8_t>(settings.view) + 1) % 5);
  if (action == ButtonAction::ToggleWeekStart)
    settings.weekStart = settings.weekStart == WeekStart::Monday ? WeekStart::Sunday : WeekStart::Monday;
  settingsStore.save(settings);
  refreshScreen();
}
}

void setup() {
  Serial.begin(115200);
  settingsStore.begin();
  if (strlen(CHRONOPAPER_WIFI_SSID) > 0) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(CHRONOPAPER_WIFI_SSID, CHRONOPAPER_WIFI_PASSWORD);
  }
  timeManager.begin(kTimezone);
  inputManager.begin();
  displayManager.begin();
  settings = settingsStore.load({2026, 10, 1});
  refreshScreen();
}

void loop() {
  const uint32_t nowMs = millis();
  if (WiFi.status() == WL_CONNECTED) {
    if (!webStarted) { webCompanion.begin(); webStarted = true; }
    webCompanion.poll();
  }
  timeManager.loop();
  Date nextDate{}; uint8_t hour = 0, minute = 0, second = 0;
  const bool validNow = timeManager.readLocal(nextDate, hour, minute, second);
  if (validNow && (!haveValidTime || nextDate.year != currentDate.year || nextDate.month != currentDate.month ||
                   nextDate.day != currentDate.day)) {
    currentDate = nextDate; localHour = hour; localMinute = minute; localSecond = second;
    haveValidTime = true;
    if (settings.view == View::Month || settings.view == View::Week) refreshScreen();
  } else if (!validNow && haveValidTime) {
    haveValidTime = false;
    refreshScreen();
  }
  applyAction(inputManager.poll(nowMs));
  delay(5);
}
