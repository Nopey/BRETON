# BRETON Scripting
Screens are loaded by scripts. The clickzones inside the screen all run scripts.

## Comments
Comments are marked with `#` or `//`, and simply remove the rest of the line following those marks.

## Statements
Statements are a single character, possibly with some prefixes,
that does a thing.

### Conditional
The `?` statement evaluates a condition. There are several sub-statements:
TODO: Are there any other conditional subcommands?
if there are others, use `?f` for checking a flag.
for example, `?flagBrokenTV`
The result of the condition is stored in a register for any statements following that have a `+` or `-` prefix.

Statements with a `+` prefix will be run only if the condition is true,
and statements with a `-` prefix are the opposite.

### Set, Unset, or Toggle Flag
`f`, followed by `+` for set, `-` for unset, and `!` for toggle.
F+BrokenTV
f-BrokenTV
F!BrokenTV

### Decals
Have a name, and an integer state.
The name is for scripting, especially for associating with clickzones.
A state can be 0 for hidden, or any other number for the image with that id.
`d 100 100 TV TV.png BrokenTV.png`

### Clickzones
`z` for zone
followed by `x`, `y`, `scale`, and `[decalName]`.
`z 100 100 50 0

### Backgrounds
`b` for background
followed by a path
