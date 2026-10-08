#include "WebCompanion.h"
#include "ResourceLinks.h"
#include <ESPmDNS.h>

namespace chrono_paper {
namespace {
const char* monthName(int month) {
  static const char* names[] = {"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
  return (month >= 1 && month <= 12) ? names[month] : "Unknown";
}
}

void WebCompanion::begin() {
  server_.on("/", HTTP_GET, [this]() { handleRoot(); });
  server_.onNotFound([this]() { server_.send(404, "text/plain", "Not found"); });
  server_.begin();
  MDNS.begin("chronopaper");
  MDNS.addService("http", "tcp", 80);
}

void WebCompanion::poll() { server_.handleClient(); }

void WebCompanion::handleRoot() {
  int year = server_.hasArg("year") ? server_.arg("year").toInt() : 2027;
  int month = server_.hasArg("month") ? server_.arg("month").toInt() : 1;
  if (year < 1 || year > 9999 || month < 1 || month > 12) {
    server_.send(400, "text/plain", "Invalid year or month");
    return;
  }
  const PrintableResource resource = printableResource(year, month);
  String html;
  html.reserve(2300);
  html += F("<!doctype html><html lang=\"en\"><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>ChronoPaper local calendar</title><style>body{font:16px system-ui;max-width:48rem;margin:3rem auto;padding:0 1rem;color:#17202b}main{border:1px solid #bbc5ce;border-radius:12px;padding:1.4rem}label{display:inline-grid;gap:.3rem;margin:.5rem 1rem .5rem 0}select,button{font:inherit;padding:.5rem}a{color:#154f71}small{color:#52606d}</style><main><p>CHRONOPAPER / LOCAL RESOURCE NAVIGATOR</p><h1>");
  html += monthName(month); html += ' '; html += String(year);
  html += F("</h1><p>This read-only page is served by your device on its local network. The calendar firmware remains usable offline.</p><form method=\"get\"><label>Year<input name=\"year\" type=\"number\" min=\"1\" max=\"9999\" value=\"");
  html += String(year);
  html += F("\"></label><label>Month<select name=\"month\">");
  for (int i = 1; i <= 12; ++i) {
    html += F("<option value=\""); html += String(i); html += '"';
    if (i == month) html += F(" selected");
    html += '>'; html += monthName(i); html += F("</option>");
  }
  html += F("</select></label><button type=\"submit\">Show reference</button></form><section><h2>Printable resource</h2>");
  if (resource.verified && resource.url) {
    html += F("<p><a rel=\"noopener noreferrer\" target=\"_blank\" href=\"");
    html += resource.url;
    html += F("\">"); html += resource.label; html += F("</a></p>");
  } else {
    html += F("<p>Printable reference unavailable. No year-matched destination has been verified for this month.</p>");
  }
  html += F("</section><p><small>Beta Calendars publishes the printable reference collection. Links are direct and open only after a user selects a month.</small></p></main></html>");
  server_.send(200, "text/html; charset=utf-8", html);
}

} // namespace chrono_paper
