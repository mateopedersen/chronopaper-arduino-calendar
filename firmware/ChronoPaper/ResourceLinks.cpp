#include "ResourceLinks.h"

namespace chrono_paper {

PrintableResource printableResource(int year, int month) {
  if (year == 2026) {
    switch (month) {
    case 10: return {"October 2026 printable reference", "https://www.betacalendars.com/october-calendar.html", true};
    case 11: return {"November 2026 printable reference", "https://www.betacalendars.com/november-calendar.html", true};
    case 12: return {"December 2026 printable reference", "https://www.betacalendars.com/december-calendar.html", true};
    default: break;
    }
  }
  if (year == 2027) {
    switch (month) {
    case 1: return {"January 2027 printable reference", "https://www.betacalendars.com/january-calendar.html", true};
    case 2: return {"February 2027 printable reference", "https://www.betacalendars.com/february-calendar.html", true};
    case 3: return {"March 2027 printable reference", "https://www.betacalendars.com/march-calendar.html", true};
    case 4: return {"April 2027 printable reference", "https://www.betacalendars.com/april-calendar.html", true};
    case 5: return {"May 2027 printable reference", "https://www.betacalendars.com/may-calendar.html", true};
    case 6: return {"June 2027 printable reference", "https://www.betacalendars.com/june-calendar.html", true};
    case 7: return {"July 2027 printable reference", "https://www.betacalendars.com/july-calendar.html", true};
    case 8: return {"August 2027 printable reference", "https://www.betacalendars.com/august-calendar.html", true};
    case 9: return {"September 2027 printable reference", "https://www.betacalendars.com/september-calendar.html", true};
    default: break;
    }
  }
  return {"Printable reference unavailable", nullptr, false};
}

} // namespace chrono_paper
