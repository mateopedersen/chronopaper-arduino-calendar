#include "../../firmware/ChronoPaper/CalendarEngine.h"
#include <cassert>
#include <iostream>

using namespace chrono_paper;

int main() {
  assert(isLeapYear(2000));
  assert(!isLeapYear(1900));
  assert(!isLeapYear(2100));
  assert(isLeapYear(2028));
  assert(daysInMonth(2027, 2) == 28);
  assert(daysInMonth(2028, 2) == 29);
  assert(daysInMonth(1900, 2) == 28);
  assert(daysInMonth(2000, 2) == 29);
  assert(daysInMonth(2100, 2) == 28);
  assert(!isValidDate({2027, 2, 29}));
  assert(!isValidDate({0, 1, 1}));
  assert(!isValidDate({2027, 13, 1}));
  assert(!isValidDate({2027, 1, 0}));

  const Date jan1{2027, 1, 1};
  assert(weekdaySunday0(jan1) == 5); // Friday.
  assert(weekdayIndex(jan1, WeekStart::Monday) == 4);
  assert(weekdayIndex(jan1, WeekStart::Sunday) == 5);
  assert(dayOfYear({2027, 12, 31}) == 365);

  assert(naturalWeekRows(2027, 2, WeekStart::Monday) == 4);
  assert(naturalWeekRows(2027, 1, WeekStart::Monday) == 5);
  assert(naturalWeekRows(2027, 5, WeekStart::Monday) == 6);
  assert(naturalWeekRows(2027, 1, WeekStart::Sunday) == 6);

  Date shifted{};
  assert(addDays({2026, 12, 31}, 1, shifted));
  assert(shifted.year == 2027 && shifted.month == 1 && shifted.day == 1);
  assert(addDays({2027, 1, 1}, -1, shifted));
  assert(shifted.year == 2026 && shifted.month == 12 && shifted.day == 31);
  assert(previousMonth({2027, 1, 31}, shifted));
  assert(shifted.year == 2026 && shifted.month == 12 && shifted.day == 31);
  assert(nextMonth({2027, 1, 31}, shifted));
  assert(shifted.year == 2027 && shifted.month == 2 && shifted.day == 28);
  assert(previousMonth({2028, 3, 31}, shifted));
  assert(shifted.year == 2028 && shifted.month == 2 && shifted.day == 29);
  assert(!addDays({1, 1, 1}, -1, shifted));

  IsoWeek week{};
  assert(isoWeekFor({2021, 1, 1}, week));
  assert(week.year == 2020 && week.number == 53);
  assert(week.monday.year == 2020 && week.monday.month == 12 && week.monday.day == 28);
  assert(week.sunday.year == 2021 && week.sunday.month == 1 && week.sunday.day == 3);
  assert(isoWeekFor({2021, 1, 4}, week));
  assert(week.year == 2021 && week.number == 1);
  assert(isoWeekFor({2027, 1, 1}, week));
  assert(week.year == 2026 && week.number == 53);
  assert(!isoWeekFor({2027, 2, 29}, week));

  Date cells[42]{};
  uint8_t rows = 0;
  assert(buildMonthGrid(2027, 1, WeekStart::Monday, cells, rows));
  assert(rows == 5);
  assert(cells[4].year == 2027 && cells[4].month == 1 && cells[4].day == 1);
  assert(cells[39].year == 2027 && cells[39].month == 2 && cells[39].day == 5);
  assert(buildMonthGrid(2027, 2, WeekStart::Monday, cells, rows));
  assert(rows == 4 && cells[0].year == 2027 && cells[0].month == 2 && cells[0].day == 1);
  assert(!buildMonthGrid(2027, 13, WeekStart::Monday, cells, rows));

  std::cout << "ChronoPaper calendar tests passed.\n";
  return 0;
}
