#include "TimeManager.h"
#include <Arduino.h>
#include <RTClib.h>
#include <sys/time.h>
#include <time.h>
#include <Wire.h>
#include <esp_sntp.h>

namespace chrono_paper {
namespace {
RTC_DS3231 rtc;
constexpr time_t kPlausibleEpoch = 1735689600; // 2025-01-01 UTC.
volatile bool sntpCallbackSeen = false;
void onSntpSync(struct timeval *) { sntpCallbackSeen = true; }
}

bool TimeManager::begin(const char *posixTimezone) {
  setenv("TZ", posixTimezone && *posixTimezone ? posixTimezone : "UTC0", 1);
  tzset();
  configTzTime(posixTimezone && *posixTimezone ? posixTimezone : "UTC0",
               "pool.ntp.org", "time.nist.gov", "time.google.com");
  esp_sntp_set_time_sync_notification_cb(onSntpSync);
  ntpStarted_ = true;
  Wire.begin(); // Nano ESP32 defaults to A4/SDA and A5/SCL.
  rtcReady_ = rtc.begin();
  if (rtcReady_ && !rtc.lostPower()) {
    const DateTime stored = rtc.now();
    if (stored.year() >= 2025) {
      const timeval value{static_cast<time_t>(stored.unixtime()), 0};
      settimeofday(&value, nullptr); // RTC is stored as UTC.
      rtcRestored_ = true;
    }
  }
  return rtcReady_;
}

void TimeManager::loop() {
  if (sntpCallbackSeen && !ntpSynced_) {
    ntpSynced_ = true;
    const time_t current = time(nullptr);
    if (rtcReady_) {
      rtc.adjust(DateTime(static_cast<uint32_t>(current)));
      rtcSynced_ = true;
    }
  }
}

bool TimeManager::readLocal(Date &date, uint8_t &hour, uint8_t &minute, uint8_t &second) const {
  const time_t current = time(nullptr);
  if ((!rtcRestored_ && !ntpSynced_) || current < kPlausibleEpoch) return false;
  struct tm local{};
  if (localtime_r(&current, &local) == nullptr) return false;
  Date candidate{static_cast<int16_t>(local.tm_year + 1900), static_cast<uint8_t>(local.tm_mon + 1),
                 static_cast<uint8_t>(local.tm_mday)};
  if (!isValidDate(candidate)) return false;
  date = candidate;
  hour = static_cast<uint8_t>(local.tm_hour);
  minute = static_cast<uint8_t>(local.tm_min);
  second = static_cast<uint8_t>(local.tm_sec);
  return true;
}

const char *TimeManager::sourceLabel() const {
  if (ntpSynced_ && rtcSynced_) return "NTP synchronized; RTC stores UTC";
  if (ntpSynced_) return "NTP synchronized";
  if (rtcRestored_) return "DS3231 UTC fallback; NTP pending";
  return "No valid time source";
}

} // namespace chrono_paper
