#pragma once

#include "CalendarEngine.h"
#include <stdint.h>

namespace chrono_paper {

class TimeManager {
public:
  bool begin(const char *posixTimezone);
  void loop();
  bool readLocal(Date &date, uint8_t &hour, uint8_t &minute, uint8_t &second) const;
  bool rtcAvailable() const { return rtcReady_; }
  bool ntpSynchronized() const { return ntpSynced_; }
  const char *sourceLabel() const;
private:
  bool rtcReady_ = false;
  bool rtcRestored_ = false;
  bool ntpStarted_ = false;
  bool ntpSynced_ = false;
  bool rtcSynced_ = false;
};

} // namespace chrono_paper
