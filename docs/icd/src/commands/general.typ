#import "packet.typ": *

// ============================================= Field Descriptions =============================================
#let device-modes = (
  enum-value([0], [Memory (Device is runnning based on the configuration saved to memory)]),
  enum-value([1], [Controlled (Device is controlled through the interface)]),
)
// ==============================================================================================================

= General

== Device Information

#packet(
  title: "Command",
  cmd-id: [0x0100],
  length: [0],
  data-field([-], [-], [-], [No data]),
)

#packet(
  title: "Response",
  cmd-id: [0x8100],
  length: [7],
  data-field([0 - 3], [Serial], [uint32], [Device serial number]),
  data-field([4], [FW_Ver_Major], [uint8], [Firmware version - Major Number]),
  data-field([5], [FW_Ver_Minor], [uint8], [Firmware version - Minor Number]),
  data-field([6], [HW_Ver], [uint8], [Hardware revision]),
)

== Device Status

#packet(
  title: "Command",
  cmd-id: [0x0101],
  length: [0],
  data-field([-], [-], [-], [No data]),
)

#packet(
  title: "Response",
  cmd-id: [0x8101],
  length: [2],
  data-field([0], [Status], [uint8], [Status]),
  data-field([1], [Mode], [uint8], [Current device mode]),
)

#enum-table(
  "Status",
  enum-value([0], [Operational]),
  enum-value([1], [Initialization]),
  enum-value([2], [Error]),
)

#pagebreak()
#enum-table("Mode", ..device-modes)
