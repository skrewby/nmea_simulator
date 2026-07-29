#let changelog-entry(version, date, description) = (
  version: version,
  date: date,
  description: description,
)

#let changelog(..entries) = {
  heading(numbering: none, outlined: false)[Changelog]

  table(
    columns: (1fr, 1fr, 5fr),
    align: horizon,
    table.header([*Version*], [*Date*], [*Description*]),
    ..entries
      .pos()
      .map(e => (
        [#e.version],
        [#e.date.display()],
        list(..e.description.map(d => [#d])),
      ))
      .flatten(),
  )
}

#let icd(
  date: none,
  product: none,
  version: none,
  changelog-entries: (),
  body,
) = {
  set page(
    paper: "a4",
    margin: (x: 2.0cm, y: 2.0cm),
  )

  show title: rules => {
    set text(size: 22pt)
    set align(center + horizon)
    rules
  }

  show heading: rules => {
    set block(below: 1.2em)
    rules
  }
  set heading(numbering: "1.1.")

  align(center + top)[
    #set text(size: 18pt)

    #product
  ]

  title[Interface Control Document]

  align(center + bottom)[
    #set text(size: 18pt)

    Version #version

    #date.display()
  ]

  pagebreak()

  changelog(..changelog-entries)

  pagebreak()

  outline(title: [Table of Contents])

  pagebreak()

  set page(
    header: grid(
      columns: (1fr, 1fr, 1fr),
      align(left, text(size: 9pt)[#product]),
      align(center, text(size: 9pt)[Interface Control Document]),
      align(right, text(size: 9pt)[Version #version]),
    ),
    footer: context align(
      right,
      text(size: 9pt)[#counter(page).display("1 of 1", both: true)],
    ),
  )

  body
}
