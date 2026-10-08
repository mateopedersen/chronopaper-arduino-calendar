#pragma once

#include "CalendarEngine.h"
#include "SettingsStore.h"

namespace chrono_paper {

class DisplayManager {
public:
  void begin();
  void render(const Settings &settings, const Date &today, bool timeValid,
              const char *timeSource, bool wifiConnected, bool rtcAvailable);
private:
  void draw(const Settings &settings, const Date &today, bool timeValid,
            const char *timeSource, bool wifiConnected, bool rtcAvailable);
  void drawMonth(const Settings &settings, const Date &today, bool timeValid);
  void drawYear(const Settings &settings);
  void drawWeek(const Settings &settings, const Date &today, bool timeValid);
  void drawResource(const Settings &settings);
  void drawStatus(const Settings &settings, const char *timeSource,
                  bool wifiConnected, bool rtcAvailable);
};

} // namespace chrono_paper
