#include "SettingsStore.h"
#include <Preferences.h>

namespace chrono_paper {
namespace { Preferences preferences; }

void SettingsStore::begin() {
  ready_ = preferences.begin("chronopaper", false);
}

Settings SettingsStore::load(const Date &fallback) {
  if (!ready_) return {fallback.year, fallback.month, WeekStart::Monday, View::Month, 0};
  Settings settings{
      static_cast<int16_t>(preferences.getUShort("year", fallback.year)),
      preferences.getUChar("month", fallback.month),
      static_cast<WeekStart>(preferences.getUChar("weekStart", static_cast<uint8_t>(WeekStart::Monday))),
      static_cast<View>(preferences.getUChar("view", static_cast<uint8_t>(View::Month))),
      preferences.getUChar("yearPage", 0)};
  if (daysInMonth(settings.year, settings.month) == 0) {
    settings.year = fallback.year;
    settings.month = fallback.month;
  }
  if (settings.weekStart != WeekStart::Sunday && settings.weekStart != WeekStart::Monday)
    settings.weekStart = WeekStart::Monday;
  if (static_cast<uint8_t>(settings.view) > static_cast<uint8_t>(View::Status))
    settings.view = View::Month;
  if (settings.yearPage > 3) settings.yearPage = 0;
  return settings;
}

void SettingsStore::save(const Settings &settings) {
  if (!ready_) return;
  preferences.putUShort("year", settings.year);
  preferences.putUChar("month", settings.month);
  preferences.putUChar("weekStart", static_cast<uint8_t>(settings.weekStart));
  preferences.putUChar("view", static_cast<uint8_t>(settings.view));
  preferences.putUChar("yearPage", settings.yearPage);
}

} // namespace chrono_paper
