#pragma once

#include "CalendarEngine.h"

namespace chrono_paper {

struct PrintableResource {
  const char *label;
  const char *url;
  bool verified;
};

PrintableResource printableResource(int year, int month);

} // namespace chrono_paper
