#import "icd.typ": *
#import "changelog.typ"

#show: icd.with(
  date: datetime(day: 29, month: 7, year: 2026),
  product: "NMEA Simulator",
  version: "1.0",
  changelog-entries: changelog.entries,
)

#include "overview.typ"
#pagebreak()

#include "packet_overview.typ"
#pagebreak()

#include "error_handling.typ"
#pagebreak()

#include "commands/general.typ"
