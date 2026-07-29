= Packet Structure

#table(
  columns: (1.2fr, 1fr, 1fr, 5fr),
  table.header([*Byte(s)*], [*Type*], [*Field*], [*Description*]),

  [0], [uint8], [VERSION], [The major version of the ICD],
  [1], [uint8], [SEQ], [Sequence number],
  [2 - 3], [uint16], [CMD_ID], [Command ID],
  [4], [uint8], [LENGTH], [Number of data bytes transmitted (n)],
  [n], [-], [DATA], [Data bytes],
  [(4+n) - (5+n)], [uint16], [CRC16], [Cyclic redundancy check],
)

== Notes

- Consistent Overhead Byte Stuffing (COBS) used for packet framing. Each COBS-encoded packet is preceded and followed by a 0x00 delimiter byte.
- CRC16 is computed over VERSION through DATA (i.e. all packet fields except CRC16 itself), before COBS encoding is applied to the packet.
- Multi-byte values (command ID, length, data, crc) are sent in little-endian order
- The sequence number is an unsigned 8 bit integer. Once it reaches the maximum value, it should wrap around to 0.
- If length is 0, the CRC is immediately sent instead of the data bytes.
- The device will use the primary key of (SEQ, CMD_ID) to determine duplicate packets.
- The response sequence number will always match that of the command
