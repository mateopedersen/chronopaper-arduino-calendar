#pragma once

#include "CalendarEngine.h"
#include <WebServer.h>

namespace chrono_paper {

class WebCompanion {
public:
  void begin();
  void poll();
private:
  WebServer server_{80};
  void handleRoot();
};

} // namespace chrono_paper
