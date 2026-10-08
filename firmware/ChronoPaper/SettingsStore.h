#pragma once

#include "CalendarEngine.h"

namespace chrono_paper {

enum class View : uint8_t { Month = 0, Year = 1, Week = 2, Resource = 3, Status = 4 };

struct Settings {
  int16_t year;
  uint8_t month;
  WeekStart weekStart;
  View view;
  uint8_t yearPage;
};

class SettingsStore {
public:
  void begin();
  Settings load(const Date &fallback);
  void save(const Settings &settings);
private:
  bool ready_ = false;
};

} // namespace chrono_paper
