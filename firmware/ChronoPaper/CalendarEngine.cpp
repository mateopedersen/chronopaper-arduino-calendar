#include "CalendarEngine.h"

namespace chrono_paper {
namespace {
int64_t daysFromCivil(int year, unsigned month, unsigned day) {
  year -= month <= 2;
  const int era = (year >= 0 ? year : year - 399) / 400;
  const unsigned yoe = static_cast<unsigned>(year - era * 400);
  const unsigned doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return static_cast<int64_t>(era) * 146097 + static_cast<int64_t>(doe) - 719468;
}

Date civilFromDays(int64_t z) {
  z += 719468;
  const int64_t era = (z >= 0 ? z : z - 146096) / 146097;
  const unsigned doe = static_cast<unsigned>(z - era * 146097);
  const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  int year = static_cast<int>(yoe) + static_cast<int>(era * 400);
  const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  const unsigned mp = (5 * doy + 2) / 153;
  const unsigned day = doy - (153 * mp + 2) / 5 + 1;
  const unsigned month = mp + (mp < 10 ? 3 : -9);
  year += month <= 2;
  return {static_cast<int16_t>(year), static_cast<uint8_t>(month), static_cast<uint8_t>(day)};
}
} // namespace

bool isLeapYear(int year) {
  return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

uint8_t daysInMonth(int year, int month) {
  if (year < 1 || year > 9999 || month < 1 || month > 12) return 0;
  static const uint8_t lengths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  return static_cast<uint8_t>(lengths[month - 1] + ((month == 2 && isLeapYear(year)) ? 1 : 0));
}

bool isValidDate(Date date) {
  const uint8_t limit = daysInMonth(date.year, date.month);
  return limit != 0 && date.day >= 1 && date.day <= limit;
}

uint8_t weekdaySunday0(Date date) {
  if (!isValidDate(date)) return 255;
  int value = static_cast<int>((daysFromCivil(date.year, date.month, date.day) + 4) % 7);
  if (value < 0) value += 7;
  return static_cast<uint8_t>(value);
}

uint8_t weekdayIndex(Date date, WeekStart weekStart) {
  const uint8_t sundayIndex = weekdaySunday0(date);
  if (sundayIndex == 255) return 255;
  return static_cast<uint8_t>((sundayIndex + (weekStart == WeekStart::Monday ? 6 : 0)) % 7);
}

uint16_t dayOfYear(Date date) {
  if (!isValidDate(date)) return 0;
  uint16_t total = date.day;
  for (int month = 1; month < date.month; ++month) total += daysInMonth(date.year, month);
  return total;
}

uint8_t naturalWeekRows(int year, int month, WeekStart weekStart) {
  const uint8_t length = daysInMonth(year, month);
  if (length == 0) return 0;
  const uint8_t lead = weekdayIndex({static_cast<int16_t>(year), static_cast<uint8_t>(month), 1}, weekStart);
  return static_cast<uint8_t>((lead + length + 6) / 7);
}

bool addDays(Date input, int32_t delta, Date &output) {
  if (!isValidDate(input)) return false;
  const int64_t result = daysFromCivil(input.year, input.month, input.day) + delta;
  const Date candidate = civilFromDays(result);
  if (candidate.year < 1 || candidate.year > 9999) return false;
  output = candidate;
  return true;
}

bool previousMonth(Date input, Date &output) {
  if (!isValidDate(input)) return false;
  int year = input.year;
  int month = input.month - 1;
  if (month == 0) { month = 12; --year; }
  if (year < 1) return false;
  const uint8_t day = input.day > daysInMonth(year, month) ? daysInMonth(year, month) : input.day;
  output = {static_cast<int16_t>(year), static_cast<uint8_t>(month), day};
  return true;
}

bool nextMonth(Date input, Date &output) {
  if (!isValidDate(input)) return false;
  int year = input.year;
  int month = input.month + 1;
  if (month == 13) { month = 1; ++year; }
  if (year > 9999) return false;
  const uint8_t day = input.day > daysInMonth(year, month) ? daysInMonth(year, month) : input.day;
  output = {static_cast<int16_t>(year), static_cast<uint8_t>(month), day};
  return true;
}

bool isoWeekFor(Date date, IsoWeek &output) {
  if (!isValidDate(date)) return false;
  const uint8_t monday0 = weekdayIndex(date, WeekStart::Monday);
  Date monday, sunday, thursday, jan4;
  if (!addDays(date, -static_cast<int32_t>(monday0), monday) ||
      !addDays(monday, 6, sunday) || !addDays(monday, 3, thursday)) return false;
  const int weekYear = thursday.year;
  jan4 = {static_cast<int16_t>(weekYear), 1, 4};
  Date weekOneMonday;
  if (!addDays(jan4, -static_cast<int32_t>(weekdayIndex(jan4, WeekStart::Monday)), weekOneMonday)) return false;
  const int64_t difference = daysFromCivil(monday.year, monday.month, monday.day) -
                             daysFromCivil(weekOneMonday.year, weekOneMonday.month, weekOneMonday.day);
  if (difference < 0 || difference % 7 != 0) return false;
  output = {static_cast<int16_t>(weekYear), static_cast<uint8_t>(difference / 7 + 1), monday, sunday};
  return true;
}

bool buildMonthGrid(int year, int month, WeekStart weekStart, Date cells[42], uint8_t &rows) {
  if (daysInMonth(year, month) == 0 || cells == nullptr) return false;
  const Date first{static_cast<int16_t>(year), static_cast<uint8_t>(month), 1};
  const uint8_t lead = weekdayIndex(first, weekStart);
  rows = naturalWeekRows(year, month, weekStart);
  Date cursor;
  if (!addDays(first, -static_cast<int32_t>(lead), cursor)) return false;
  for (uint8_t i = 0; i < 42; ++i) {
    cells[i] = cursor;
    if (i < 41 && !addDays(cursor, 1, cursor)) return false;
  }
  return true;
}

} // namespace chrono_paper
