#let data-field(bytes, field, datatype, description) = (
  bytes: bytes,
  field: field,
  datatype: datatype,
  description: description,
)

#let packet(title: none, cmd-id: none, length: none, ..fields) = {
  table(
    columns: (auto, auto, auto, 1fr),
    table.cell(colspan: 4, fill: luma(220))[*#title*],
    [*CMD_ID*], table.cell(colspan: 3)[#cmd-id],
    [*LENGTH*], table.cell(colspan: 3)[#length],
    table.cell(colspan: 4, align: center)[*DATA*],
    table.header(align(center)[*Byte(s)*], [*Field*], [*Type*], [*Description*]),
    ..fields
      .pos()
      .map(f => (
        align(center)[#f.bytes],
        [#f.field],
        [#f.datatype],
        [#f.description],
      ))
      .flatten(),
  )
}

#let enum-value(value, description) = (value: value, description: description)

#let enum-table(field, ..values) = table(
  columns: (1fr, 13fr),
  table.cell(colspan: 2, fill: luma(240))[*#field*],
  ..values.pos().map(v => (align(center)[#v.value], [#v.description])).flatten(),
)
