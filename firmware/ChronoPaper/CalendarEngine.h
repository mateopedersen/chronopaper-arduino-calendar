#pragma once

#include <stdint.h>

namespace chrono_paper {

enum class WeekStart : uint8_t { Sunday = 0, Monday = 1 };

struct Date {
  int16_t year;
  uint8_t month;
  uint8_t day;
};

struct IsoWeek {
  int16_t year;
  uint8_t number;
  Date monday;
  Date sunday;
};

bool isLeapYear(int year);
uint8_t daysInMonth(int year, int month);
bool isValidDate(Date date);
uint8_t weekdaySunday0(Date date); // Sunday=0 ... Saturday=6.
uint8_t weekdayIndex(Date date, WeekStart weekStart); // 0 is first visible column.
uint16_t dayOfYear(Date date); // 1-based.
uint8_t naturalWeekRows(int year, int month, WeekStart weekStart);
bool addDays(Date input, int32_t delta, Date &output);
bool previousMonth(Date input, Date &output);
bool nextMonth(Date input, Date &output);
bool isoWeekFor(Date date, IsoWeek &output);
bool buildMonthGrid(int year, int month, WeekStart weekStart, Date cells[42], uint8_t &rows);

} // namespace chrono_paper
