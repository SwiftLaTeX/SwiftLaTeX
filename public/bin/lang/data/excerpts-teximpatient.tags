comments = [[
Extracted from “TeX for the Impatient” by Paul W. Abrahams, Kathryn
A. Hargreaves, Karl Berry, and Daniel H. James

URL: https://www.gnu.org/software/teximpatient/
Original license: GNU FDL
]]
data = {
   ["+"] = [[
`\+`, `\tabalign`

These commands begin a single line in a tabbed alignment. The only
difference between `\+` and `\tabalign` is that `\+` is an outer
macro—you can’t use it when TeX is reading tokens at high speed .

If you place an ‘`&`’ at a position to the right of all existing tabs in
a tabbing alignment, the ‘`&`’ establishes a new tab at that position.
]],
   AA = [[
`\AA` (Å), `\AE`, `\L` (Ł), `\OE`, `\O` (Ø), `\aa` (å), `\ae`, `\l` (ł),
`\oe`, `\o` (ø), `\ss` (ß)

These commands produce various letters and ligatures from European
alphabets. They are useful for occasional words and phrases in these
languages—but if you need to typeset a large amount of text in a
European language, you should probably be using a version of TeX adapted
to that language.[1]

You’ll need a space after these commands when you use them within a
word, so that TeX will treat the following letters as part of the word
rather than as part of the command. You needn’t be in math mode to use
these commands.

[1] The TeX Users Group can provide you with information about European
language versions of TeX.
]],
   TeX = [[
This command produces the TeX logo. Remember to follow it by `\!vs` or
to enclose it in a group when you want a space after it.
]],
   abovedisplayshortskip = [[
This parameter specifies the amount of vertical glue that TeX inserts
before a math display when the display starts to the right of where the
previous line ends, i.e., when it does not visually overlap the previous
line. Plain TeX sets `\abovedisplayshortskip` to `0pt plus3pt`. See
pages 188–189 of *The TeXbook* for a more detailed explanation of this
parameter.
]],
   abovedisplayskip = [[
This parameter specifies the amount of vertical glue that TeX inserts
before a display when the display starts to the left of where the
previous line ends, i.e., when it visually overlaps the previous line.
Plain TeX sets `\abovedisplayskip` to `12pt plus3pt minus9pt`. See
pages 188–189 of *The TeXbook* for a more detailed explanation of this
parameter.
]],
   accent = [[
This command puts an accent over the character following this command.
The accent is the character at position `charcode` in the current font.
TeX assumes that the accent has been designed to fit over a character
$1$ex high in the same font as the accent. If the character to be
accented is taller or shorter, TeX adjusts the position accordingly. You
can change fonts between the accent and the next character, thus drawing
the accent character and the character to be accented from different
fonts. If the accent character isn’t really intended to be an accent,
TeX won’t complain; it will just typeset something ridiculous.
]],
   active = [[
This command contains the category code for an active character, namely,
the number $13$.
]],
   acute = [[
`\acute`, `\bar`, `\breve`, `\b`, `\check`, `\ddot`, `\dot`, `\grave`,
`\hat`, `\tilde`, `\vec`, `\widehat`, `\widetilde`

These commands produce accent marks in math formulas. You’ll ordinarily
need to leave a space after any one of them. A wide accent can be
applied to a multicharacter subformula; TeX will center the accent over
the subformula. The other accents are usefully applied only to a single
character.
]],
   adjdemerits = [[
 This parameter specifies additional demerits that TeX attaches to a
breakpoint between two adjacent lines that are “visually incompatible”.
Such a pair of lines makes a paragraph appear uneven. Incompatibility is
evaluated in terms of the tightness or looseness of lines:

1.  A line is tight if its glue needs to shrink by at least $50\%$.

2.  A line is decent if its badness is $12$ or less.

3.  A line is loose if its glue needs to stretch by more than $50\%$.

4.  A line is very loose if its glue needs to stretch so much that its
    badness exceeds $100$.

Two adjacent lines are visually incompatible if their categories are not
adjacent, e.g., a tight line is next to a loose one or a decent line is
next to a very loose one.

Demerits are in units of badness squared, so you need to assign a rather
large value to this parameter (in the thousands) for it to have any
effect. Plain TeX sets `\adjdemerits` to $10000$.
]],
   advance = [[
This command adds a compatible quantity to a register. For `glue` or
`muglue` all three components (natural value, stretch, and shrink) are
added. Any of the quantities can be negative. For purposes of these
calculations (and other assignments as well), `glue` can be converted to
a `dimen` by dropping the stretch and shrink, and a `dimen` can be
converted to a `number` by taking its value in scaled points . You can
omit the word `by` in these commands—TeX will understand them anyway.
]],
   advancepageno = [[
This command adds $1$ to the page number $n$ in `\pageno` if $n\ge0$ and
subtracts $1$ from it if $n<0$.
]],
   afterassignment = [[
When TeX encounters this command it saves `token` in a special place.
After it next performs an assignment, it inserts `token` into the input
and expands it. If you call `\afterassignment` more than once before an
assignment, only the last call has any effect. One use of
`\afterassignment` is in writing macros for commands intended to be
written in the form of assignments, as in the example below.

See page 279 of *The TeXbook* for a precise description of the behavior
of `\afterassignment`.
]],
   aftergroup = [[
When TeX encounters this command during input, it saves `token`. After
the end of the current group, it inserts `token` back into the input and
expands it. If a group contains several `\aftergroup`s, the
corresponding tokens are *all* inserted following the end of the group,
in the order in which they originally appeared.

The example that follows shows how you can use `\aftergroup` to postpone
processing a token that you generate within a conditional test.
]],
   allowbreak = [[
This command tells TeX to allow a page break where one could not
ordinarily occur. `\allowbreak` can also be used in horizontal mode.
]],
   alpha = [[
`\Delta` (Δ), `\Gamma` (Γ), `\Lambda` (Λ), `\Omega` (Ω), `\Phi` (Φ),
`\Pi` (Π), `\Psi` (Ψ), `\Sigma` (Σ), `\Theta` (Θ), `\Upsilon` (Υ), `\Xi`
(Ξ), `\alpha` (α), `\beta` (β), `\chi` (χ), `\delta` (δ), `\epsilon`
(ϵ), `\eta` (η), `\gamma` (γ), `\iota` (ι), `\kappa` (κ), `\lambda` (λ),
`\mu` (μ), `\nu` (ν), `\omega` (ω), `\phi` (ϕ), `\pi` (π), `\psi` (ψ),
`\rho` (ρ), `\sigma` (σ), `\tau` (τ), `\theta` (θ), `\upsilon` (υ),
`\varepsilon` (ε), `\varphi` (φ), `\varpi` (ϖ), `\varrho` (𝜚),
`\varsigma` (ς), `\vartheta` (ϑ), `\xi` (ξ), `\zeta` (ζ)

These commands produce Greek letters suitable for mathematics. You can
only use them within a math formula, so if you need a Greek letter
within ordinary text you must enclose it in dollar signs (`$`). TeX does
not have commands for Greek letters that look like their roman
counterparts, since you can get them by using those roman counterparts.
For example, you can get a lowercase omicron in a formula by writing the
letter ‘o’, i.e., ‘`{\rm o}`’ or an uppercase beta (‘B’) by writing
‘`{\rm B}`’.

Don’t confuse the following letters:

-   `\upsilon` (‘$\upsilon$’), `{\rm v}` (‘v’), and `\nu` (‘$\nu$’).

-   `\varsigma` (‘$\varsigma$’) and `\zeta` (‘$\zeta$’).

You can get slanted capital Greek letters by using the math italic
(`\mit`) font.

TeX treats Greek letters as ordinary symbols when it’s figuring how much
space to put around them.
]],
   arrowvert = [[
`\Arrowvert`, `\arrowvert`, `\bracevert`, `\lmoustache`, `\rmoustache`

These commands produce portions of certain large delimiters

and can themselves be used as delimiters. They refer to characters in
the `cmex10` math font.
]],
   asymp = [[
`\approx` (≈), `\asymp` (≍), `\bowtie` (⋈), `\cong` (≅), `\dashv` (⊣),
`\doteq` (≐), `\equiv` (≡), `\frown` (⌢), `\geq`, `\ge` (≥), `\gg` (≫),
`\in` (∈), `\leq`, `\le` (≤), `\ll` (≪), `\mid` (∣), `\models` (⊨),
`\neq` (≠), `\ne` (≠), `\ni` (∋), `\notin` (∉), `\owns` (∋), `\parallel`
(∥), `\perp` (⊥), `\preceq` (≼), `\prec` (≺), `\propto` (∝), `\simeq`
(≃), `\sim` (∼), `\smile` (⌣), `\sqsubseteq` (⊑), `\sqsupseteq` (⊒),
`\subseteq` (⊆), `\subset` (⊂), `\succeq` (≽), `\succ` (≻), `\supseteq`
(⊇), `\supset` (⊃), `\vdash` (⊢)

These commands produce the symbols for various relations. Relations are
one of TeX’s classes of math symbols. TeX puts different amounts of
space around different classes of math symbols. When TeX needs to break
a line of text within a math formula, it will consider placing the break
after a relation—but only if the relation is at the outermost level of
the formula, i.e., not enclosed in a group.

In addition to the commands listed here, TeX treats ‘`=\verb`’ and the
“arrow” commands as relations.

Certain relations have more than one command that you can use to produce
them:

-   ‘$\ge$’ (|| and `\geq`).

-   ‘$\le$’ (`\le` and `\leq`).

-   ‘$\ne$’ (`\ne`, `\neq`, and `\not=`).

-   ‘$\ni$’ (`\ni` and `\owns`).
]],
   badness = [[
This command yields the numerical value of the badness of the box
(either horizontal or vertical) that TeX has most recently produced. If
the box was overfull, `\badness` will be $1000000$; in all other cases
it will be between $0$ and $10000$.
]],
   baselineskip = [[
`\baselineskip`, `\lineskiplimit`, `\lineskip`

These three parameters jointly determine how much space TeX leaves
between consecutive boxes of an ordinary vertical list, e.g., the lines
of a paragraph. This space is called “interline glue”. It is also
inserted between the component boxes of a vbox constructed in internal
vertical mode.

In the usual case, when the boxes aren’t abnormally high or deep,
TeX makes the distance from the baseline of one box to the baseline of
the next one equal to `\baselineskip`. It does this by inserting
interline glue equal to `\baselineskip` minus the depth of the upper box
(as given by `\prevdepth`) and the height of the lower box. But if this
interline glue would be less than `\lineskiplimit`, indicating that the
two boxes are too close together, TeX inserts the `\lineskip` glue
instead.[1] See pages 79–80 of *The TeXbook* for a precise description.

Note that `\baselineskip` and `\lineskip` measure *different things*:
the distance between baselines on the one hand and the distance between
the bottom of one box and the top of the next box on the other hand. See
page 78 of *The TeXbook* for further details. The first example below
shows the effects of `\lineskiplimit`.

You can obtain the effect of double spacing by doubling the value of
`\baselineskip` as illustrated in the second example below. A change to
`\baselineskip` at any point before the end of a paragraph affects the
entire paragraph.

[1] TeX actually accounts for the beginning of a vertical list by
setting `\prevdepth` to $-1000$pt and testing `\prevdepth` before
*every* box. If `\prevdepth`$\>\le-1000$pt it does not insert any
interline glue.
]],
   batchmode = [[
This command tells TeX not to stop for errors and to suppress all
further output to your terminal. Typing ‘`Q`’ or ‘`q`’ in response to an
error message puts you into batch mode.
]],
   begingroup = [[
`\begingroup`, `\endgroup`

These two commands begin and end a group. A `\begingroup` does not match
up with a right brace, nor an `\endgroup` with a left brace.

TeX treats `\begingroup` and `\endgroup` like any other control sequence
when it’s scanning its input. In particular, you can define a macro that
contains a `\begingroup` but not an `\endgroup`, and conversely.

This technique is often useful when you’re defining paired macros, one
of which establishes an environment and the other of which terminates
that environment. You can’t, however, use `\begingroup` and `\endgroup`
as substitutes for braces other than the ones that surround a group.
]],
   beginsection = [[
You can use this command to begin a major subdivision of your document.
`argument` is intended to serve as a section title. `\beginsection`
surrounds `argument` by extra vertical space and sets it in boldface,
left-justified. You can produce the `\par` that ends `argument` with a
blank line.
]],
   belowdisplayshortskip = [[
This parameter specifies the amount of vertical glue that TeX inserts
after a display when the display starts to the right of where the
previous line ends, i.e., when it does not visually overlap the previous
line. Plain TeX sets `\belowdisplayshortskip` to `7pt plus3pt minus4pt`.
See pages 188–189 of *The TeXbook* for a more detailed explanation of
this parameter.
]],
   belowdisplayskip = [[
This parameter specifies the amount of vertical glue that TeX inserts
after a display when the display starts to the left of where the
previous line ends, i.e., when it visually overlaps the previous line.
Plain TeX sets `\belowdisplayskip` to `12pt plus3pt minus9pt`. See
pages 188–189 of *The TeXbook* for a more detailed explanation of this
parameter.
]],
   bf = [[
`\bf`, `\it`, `\rm`, `\sl`, `\tt`

These commands select a type style without changing the typeface or the
point size.[1] Normally you would enclose one of these type style
commands in a group, together with the text to be set in the selected
font. Outside of a group a type style command is effective until the end
of the document (unless you override it with another such command).

[1] TeX does not provide predefined commands for changing just the point
size, e.g., `\eightpoint`. Supporting such commands would require a
great number of fonts, most of which would never be used. Such commands
were, however, used in typesetting *The TeXbook*.
]],
   bgroup = [[
`\bgroup`, `\egroup`

The left and right braces are commands that begin and end a group. The
`\bgroup` and `\egroup` control sequences are equivalent to ‘`{`’ and
‘`}`’, except that TeX treats `\bgroup` and `\egroup` like any other
control sequence when it’s scanning its input.

`\bgroup` and `\egroup` can be useful when you’re defining paired
macros, one of which starts a brace-delimited construct (not necessarily
a group) and the other one of which ends that construct.

You can’t define such macros using ordinary braces—if you try, your
macro definitions will contain unmatched braces and will therefore be
unacceptable to TeX. Usually you should use these commands only when you
can’t use ordinary braces.
]],
   big = [[
`\Biggl`, `\Biggm`, `\Biggr`, `\Bigg`, `\Bigl`, `\Bigm`, `\Bigr`,
`\Big`, `\biggl`, `\biggm`, `\biggr`, `\bigg`, `\bigl`, `\bigm`,
`\bigr`, `\big`

These commands make delimiters bigger than their normal size. The
commands in the four columns produce successively larger sizes. The
difference between `\big`, `\bigl`, `\bigr`, and `bigm` has to do with
the class of the enlarged delimiter:

-   `\big` produces an ordinary symbol.

-   `\bigl` produces an opening symbol.

-   `\bigr` produces a closing symbol.

-   `\bigm` produces a relation symbol.

TeX uses the class of a symbol in order to decide how much space to put
around that symbol.

These commands, unlike `\left` and `\right`, do *not* define a group.
]],
   binoppenalty = [[
This parameter specifies the penalty for breaking a math formula after a
binary operator when the formula appears in a paragraph. Plain TeX sets
`\binoppenalty` to $700$.
]],
   bmod = [[
This command produces a binary operation for indicating a modulus within
a formula.
]],
   boxmaxdepth = [[
This parameter contains a dimension $D$. TeX will not construct a box
whose depth exceeds $D$. If you produce a box whose depth $d$ would
exceed $D$, TeX will transfer the excess depth to the height of the box,
effectively moving the reference point of the box down by $d-D$. If you
set `\boxmaxdepth` to zero, TeX will line up a row of vboxes so that
their bottom boundaries all lie on the same horizontal line. Plain
TeX sets `\boxmaxdepth` to `\maxdimen` , so `\boxmaxdepth` won’t affect
your boxes unless you change it.
]],
   ["break"] = [[
This command forces a page break. Unless you do something to fill out
the page, you’re likely to get an underfull vbox. `\break` can also be
used in horizontal mode.
]],
   brokenpenalty = [[
This parameter specifies the penalty for breaking a page just after a
line that ends in a discretionary item (usually a hyphen).

`\brokenpenalty` applies to page breaking, while `\hyphenpenalty`
applies to line breaking. Plain TeX sets `\brokenpenalty` to $100$.
]],
   c = [[
`\H`, `\c`, `\d`, `\t`, `\u`, `\v`

These commands produce accent marks in ordinary text. You’ll usually
need to leave a space after the ones denoted by a single letter ).
]],
   cal = [[
`\cal`, `\mit`, `\oldstyle`

These commands cause TeX to typeset the following text in the specified
font. You can only use them in math mode. The `\mit` command is useful
for producing slanted capital Greek letters. You can also use the
commands given in “TeX for the Impatient”, Selecting fonts,selfont to
change fonts in math mode.
]],
   cases = [[
This command produces the mathematical form that denotes a choice among
several cases. Each case has two parts, separated by ‘`&`’. TeX treats
the first part as a math formula and the second part as ordinary text.
Each case must be followed by `\cr`.
]],
   catcode = [[
This table entry contains the category code of the character whose
ASCII code is `charcode`. The category codes are listed on in “TeX for
the Impatient”. By changing the category code of a character you can get
TeX to treat that character differently.
]],
   cdotp = [[
`\cdotp` (·), `\ldotp`

These two commands respectively produce a centered dot and a dot
positioned on the baseline. They are valid only in math mode. TeX treats
them as punctuation, putting no extra space in front of them but a
little extra space after them. In contrast, TeX puts an equal amount of
space on both sides of a centered dot generated by the `\cdot` command.
]],
   centerline = [[
`\centerline`, `\leftline`, `\rightline`

The `\centerline` command produces an hbox exactly as wide as the
current line and places `argument` at the center of the box. The
`\leftline` and `\rightline` commands are analogous; they place
`argument` at the left end or at the right end of the box. If you want
to apply one of these commands to several consecutive lines, you must
apply it to each one individually. See in “TeX for the Impatient” for an
alternate approach.

Don’t use these commands within a paragraph—if you do, TeX probably
won’t be able to break the paragraph into lines and will complain about
an overfull hbox.
]],
   char = [[
This command produces the character located at position `charcode` of
the current font.
]],
   chardef = [[
This command defines `control sequence` to be `charcode`. Although
`\chardef` is most often used to define characters, you can also use it
to give a name to a number in the range $0$–$255$ even when you aren’t
using that number as a character code.
]],
   cleartabs = [[
This command clears all the tabs to the right of the current column. Its
main use is in applications such as typesetting computer programs in
which the tab positions change from line to line.
]],
   closeout = [[
This command tells TeX to close the output stream numbered `number`.
i.e., end the association between the output stream and its file. The
output stream with this number then becomes available for use with a
different file. You should close an output stream once you’re finished
using its file.

A `\closeout` generates a whatsit that becomes part of a box. The
`\closeout` does not take effect until TeX ships out that box to the
.dvi file, unless you’ve preceded the `\closeout` with `\immediate`.
]],
   clubpenalty = [[
This parameter specifies the penalty for breaking a page just after the
first line of a paragraph. A line by itself at the bottom of a page is
called a “club line”. Plain TeX sets `\clubpenalty` to $150$.
]],
   colon = [[
This command produces a colon punctation symbol. It is valid only in
math mode. The difference between `\colon` and the colon character (`:`)
is that ‘`:`’ is an operator, so TeX puts extra space to the left of it
whereas it doesn’t put extra space to the left of `\colon`.
]],
   copy = [[
This command produces a copy of box register `register`. This command is
useful when you want to retrieve the contents of a box register but
don’t want to destroy the contents. (Retrieving the register contents
with `\box` makes the register void.)
]],
   cos = [[
`\Pr`, `\arccos`, `\arcsin`, `\arctan`, `\arg`, `\cosh`, `\cos`,
`\coth`, `\cot`, `\csc`, `\deg`, `\det`, `\dim`, `\exp`, `\gcd`, `\hom`,
`\inf`, `\ker`, `\lg`, `\liminf`, `\limsup`, `\lim`, `\ln`, `\log`,
`\max`, `\min`, `\sec`, `\sinh`, `\sin`, `\sup`, `\tanh`, `\tan`

These commands set the names of various mathematical functions in roman
type, as is customary. If you apply a superscript or subscript to one of
these commands, TeX will in most cases typeset it in the usual place. In
display style, TeX typesets superscripts and subscripts on `\det`,
`\gcd`, `\inf`, `\lim`, `\liminf`, `\limsup`, `\max`, `\min`, `\Pr`, and
`\sup` as though they were limits, i.e., directly above or directly
below the function name.
]],
   count = [[
`\count`, `\dimen`, `\muskip`, `\skip`, `\toks`

The first six commands listed here assign something to a register. The
`=`’s in the assignments are optional. The remaining five control
sequences are not true commands because they can only appear as part of
an argument. They yield the contents of the specified register. Although
you can’t use these control sequences by themselves as commands in text,
you can use `\the` to convert them to text so that you can typeset their
values.

You can name and reserve registers with the `\newcount` command and its
relatives . Using these commands is a safe way to obtain registers that
are known not to have any conflicting usage.

A `\count` register contains an integer, which can be either positive or
negative. Integers can be as large as you’re ever likely to need them to
be.[1] TeX uses count registers $0$–$9$ to keep track of the page number
(see page 119 of *The TeXbook*). `\count255` is the only count register
available for use without a reservation.

[1] Here’s the only exercise in this book: find out what’s the largest
integer that TeX will accept.
]],
   countdef = [[
`\countdef`, `\dimendef`, `\muskipdef`, `\skipdef`, `\toksdef`

These commands define `control sequence` to refer to the register of the
indicated category whose number is `register`. Normally you should use
the commands in the previous group (`\newcount`, etc.) in preference to
these commands in order to avoid numbering conflicts. The commands in
the previous group are defined in terms of the commands in this group.
]],
   cr = [[
This command ends the preamble of a horizontal or vertical alignment, a
row of a horizontal or tabbing alignment, or a column of a vertical
alignment. You can cause TeX to take certain actions whenever it sees a
`\cr` by setting the value of the `\everycr` parameter .
]],
   crcr = [[
This command behaves just like `\cr`, except that TeX ignores it if it
comes immediately after a `\cr` or a `\noalign`. Its main application is
as a safety measure to avoid a misleading error message caused by a
macro that expects an argument ending in `\cr`. If you put `\crcr` after
the ‘`#`$n$’ that denotes such an argument in the macro’s definition,
the macro will work properly whether or not the argument ends with
`\cr`.
]],
   csname = [[
This command produces a control sequence from `token list`. It provides
a way of synthesizing control sequences, including ones that you can’t
normally write. `token list` can itself include control sequences; it is
expanded in the same way as the replacement text of an `\edef`
definition. If the final expansion yields anything that isn’t a
character, TeX will complain. `\csname` goes from a list of tokens to a
control sequence; you can go the other way with `\string` .
]],
   day = [[
TeX sets this parameter to the current day of the month. It is a number
between $1$ and $31$. `\day` is set at the beginning of your run (see
the comments on `\time` above).
]],
   ddots = [[
This command produces three dots on a diagonal. Its most common use is
to indicate repetition along the diagonal of a matrix.
]],
   deadcycles = [[
This parameter contains the number of times that TeX has initiated the
output routine since the last time it did a `\shipout`.[1] If
`\deadcycles` gets too big, you’ve probably gotten TeX into a loop,
e.g., one where the page builder is trying the same page break over and
over again.

[1] More precisely, TeX sets `\deadcyles` to $0$ whenever it executes
`\shipout` and increments it by $1$ whenever it executes `\output`.
]],
   def = [[
This command defines `control sequence` as a macro with the specified
`parameter text` and `replacement text`. See in “TeX for the Impatient”
for a full explanation of how to write a macro definition.
]],
   defaulthyphenchar = [[
When TeX reads the metrics file

for a font in response to a `\font` command, it sets the font’s
`\hyphenchar` to `\defaulthyphenchar`. If the value of
`\defaulthyphenchar` is not in the range $0$–$255$ when you load a font,
TeX won’t hyphenate any words in that font unless you override the
decision by setting the font’s `\hyphenchar` later on. Plain TeX sets
`\defaulthyphenchar` to $45$, the ASCII code for ‘`-`’.
]],
   defaultskewchar = [[
When TeX reads the metrics file

for a font in response to a `\font` command, it sets the font’s
`\skewchar` to `\defaultskewchar`. If the value of `\defaultskewchar` is
not in the range $0$–$255$, TeX does not assign any skew characters by
default. Plain TeX sets `\defaultskewchar` to $-1$, and it’s usually
best to leave it there.
]],
   delcode = [[
This table entry specifies the delimiter code for the input character
whose ASCII code is `charcode`. The delimiter code tells TeX how to find
the best output character to use for typesetting the indicated input
character as a delimiter.

`number` is normally written in hexadecimal notation. Suppose that
`number` is the hexadecimal number $s_1s_2s_3\,
l_1l_2l_3$. Then when the character is used as a delimiter, TeX takes
the character to have small variant $s_1s_2s_3$ and large variant
$l_1l_2l_3$. Here $s_1s_2s_3$ indicates the math character found in
position $s_2s_3$ of family $s_1$, and similarly for $l_1l_2l_3$. This
is the same convention as the one used for `\mathcode`, except that
`\mathcode` also specifies a class.
]],
   delimiter = [[
This command produces a delimiter whose characteristics are given by
`number`. `number` is normally written in hexadecimal notation. You can
use the `\delimiter` command instead of a character in any context where
TeX expects a delimiter (although the command is rarely used outside of
a macro definition). Suppose that `number` is the hexadecimal number
$cs_1s_2s_3
l_1l_2l_3$. Then TeX takes the delimiter to have class $c$, small
variant $s_1s_2s_3$, and large variant $l_1l_2l_3$. Here $s_1s_2s_3$
indicates the math character found in position $s_2s_3$ of family $s_1$,
and similarly for $l_1l_2l_3$. This is the same convention as the one
used for `\mathcode`.
]],
   discretionary = [[
This command specifies a “discretionary break”, namely, a place where
TeX can break a line. It also tells TeX what text to put on either side
of the break.

-   If TeX does not break there, it uses the `no-break text`.

-   If TeX does break there, it puts the `pre-break text` just before
    the break and the `post-break text` just after the break.

Just as with ``, TeX isn’t obligated to break a line at a discretionary
break. In fact, `` is ordinarily equivalent to
`\discretionary!allowbreak{-}{}{}`.

TeX sometimes inserts discretionary breaks on its own. For example, it
inserts `\discretionary!allowbreak{}{}{}` after an explicit hyphen or
dash.
]],
   displayindent = [[
This parameter specifies the space by which TeX indents a math display.
TeX sets the value of `\displayindent` when it encounters the ‘`$$`’
that starts the display. Usually this initial value is zero, but if the
paragraph shape indicates that the display should be shifted by an
amount $s$, TeX will set `\displayindent` to $s$. See pages 188–189 of
*The TeXbook* for a more detailed explanation of this parameter.
]],
   displaylimits = [[
This command tells TeX to place limits above and below all operators
(including the integrals) if in display style, and after all operators
if in text style.

If you specify more than one of `\limits`, `\nolimits`, and
`\displaylimits`, the last command rules.
]],
   displaywidowpenalty = [[
This parameter specifies the penalty for breaking a page just before the
last line of a partial paragraph that immediately precedes a math
display. Plain TeX sets `\displaywidowpenalty` to $50$.
]],
   displaywidth = [[
This parameter specifies the maximum width that TeX allows for a math
display. If TeX cannot fit the display into a space of this width, it
sets an overfull hbox and complains. TeX sets the value of
`\displaywidth` when it encounters the ‘`$$`’ that starts the display.
This initial value is `\hsize` unless it’s overridden by changes to the
paragraph shape. See pages 188–189 of *The TeXbook* for a more detailed
explanation of this parameter.
]],
   dotfill = [[
`\dotfill`, `\hrulefill`

These commands respectively fill the enclosing horizontal space with a
row of dots on the baseline and with a horizontal line on the baseline.
It’s usually a good idea to leave a space between `\dotfill` or
`\hrulefill` and any text that precedes or follows it (see the example
below).
]],
   dots = [[
This command produces an ellipsis, i.e., three dots, in ordinary text.
It’s intended for use in mathematical writing; for an ellipsis between
ordinary words, you should use `$\ldots$` instead. Since `\dots`
includes its own space, you shouldn’t follow it by `\!vs`.
]],
   doublehyphendemerits = [[
 This parameter specifies additional demerits that TeX attaches to a
breakpoint when that breakpoint leads to two consecutive lines that end
in a hyphen. Increasing the value of this parameter has the effect of
discouraging TeX from hyphenating two lines in a row. Demerits are in
units of badness squared, so you need to assign a rather large value to
this parameter (in the thousands) for it to have any effect. Plain
TeX sets `\doublehyphendemerits` to $10000$.
]],
   downbracefill = [[
`\downbracefill`, `\upbracefill`

These commands respectively produce upward-pointing and
downward-pointing extensible horizontal braces. TeX will make the braces
as wide as necessary. These commands are used in the definitions of
`\overbrace` and `\underbrace` .
]],
   dump = [[
This command, which must not appear inside a group, dumps the contents
of TeX’s memory to a format file. By using `virtex`, a special “virgin”
form of TeX, you can then reload the format file at high speed and
continue in the same state that TeX was in at the time of the dump.
`\dump` also ends the run. Since `\dump` can only be used in `initex`,
not in production forms of TeX, it is only useful to people who are
installing TeX.
]],
   edef = [[
This command defines a macro in the same general way as `\def`. The
difference is that TeX expands the `replacement text` of an `\edef`
immediately (but still without executing anything). Thus any definitions
within the `replacement text` are expanded, but assignments and commands
that produce things such as boxes and glue are left as is. For example,
an `` command within the `replacement text` of an `\edef` remains as a
command and is not turned into a box as TeX is processing the
definition. It isn’t always obvious what’s expanded and what isn’t, but
you’ll find a complete list of expandable control sequences on
pages 212–215 of *The TeXbook*.

You can inhibit the expansion of a control sequence that would otherwise
be expanded by using `\noexpand`. You can postpone the expansion of a
control sequence by using ``.

The `\write`, `\message`, `\errmessage`, `\wlog`, and `\csname` commands
expand their token lists using the same rules that `\edef` uses to
expand its replacement text.
]],
   eject = [[
`\eject`, `\supereject`

These commands force a page break at the current position and end the
current paragraph. If you don’t precede them with `\vfil`,

TeX will try to stretch out the page contents (and will probably
complain about an underfull vbox). The `\supereject` command, in
addition, instructs the plain TeX output routine to force out any
leftover insertions,

such as long footnotes, so that they are produced before any more input
is processed. Thus `\supereject` is a good command to use at the end of
each chapter or other major unit of your document.
]],
   ["else"] = [[
This command introduces the “false” alternative of a conditional test.
]],
   emergencystretch = [[
By setting this parameter to be greater than zero, you can make it
easier for TeX to typeset your document without generating overfull
hboxes.

This is a better alternative than setting `\tolerance=10000`, since that
tends to produce really ugly lines. If TeX can’t typeset a paragraph
without exceeding `\tolerance`, it will try again, adding
`\emergencystretch` to the stretch of each line. The effect of the
change is to scale down the badness of each line, enabling TeX to make
spaces wider than they would otherwise be and thus choose line breaks
that are as good as possible under the circumstances.
]],
   empty = [[
This command expands to no tokens at all. It differs from `\relax` in
that it disappears after macro expansion.
]],
   ["end"] = [[
This command tells TeX to produce the last page and end the job. It does
not fill out the page, however, so it’s usually better to use `\bye`
rather than `\end`.
]],
   endgraf = [[
This command is a synonym for the `\par` primitive command. It is useful
when you’ve redefined `\par` but still want access to the original
definition of `\par`.
]],
   endinput = [[
This command tells TeX to stop reading input from the current file when
it next reaches the end of a line.
]],
   endinsert = [[
This command ends an insertion started by `\topinsert`, `\midinsert`, or
`\pageinsert`.
]],
   endline = [[
This command is a synonym for the `\cr` command. It is useful when
you’ve redefined `\cr` but still need access to the original definition.
]],
   endlinechar = [[
This parameter contains the character code for the character that
TeX appends to the end of each input line.

A value not in the range $0$–$255$ indicates that no character should be
appended. Plain TeX leaves `\endlinechar` at `` `\^^M `` (the ASCII code
for ⟨return⟩).
]],
   enskip = [[
`\enskip`, `\qquad`, `\quad`

Each of these commands produces a glob of horizontal glue that can
neither stretch nor shrink. TeX can break a line at such glue. The width
of these glues (which are relative to the current font) are as follows
for `cmr10`, the default plain TeX font:

3pc
]],
   enspace = [[
This command produces a kern whose width is one en (half of an em, see
in “TeX for the Impatient”). TeX won’t break a line at an `\enspace`
unless it’s followed by glue. In a bulleted list, the bullets are
usually separated from the following text by an `\enspace`.
]],
   eqalign = [[
`\eqalignno`, `\eqalign`, `\leqalignno`

These commands produce a multiline math display in which certain
corresponding parts of the lines are lined up vertically. The
`\eqalignno` and `\leqalignno` commands also let you provide equation
numbers for some or all of the lines. `\eqalignno` puts the equation
numbers on the right and `\leqalignno` puts them on the left.

Each line in the display is ended by `\cr`. Each of the parts to be
aligned (most often an equals sign) is preceded by ‘`&`’. An ‘`&`’ also
precedes each equation number, which comes at the end of a line. You can
put more than one of these commands in a single display in order to
produce several groups of equations. In this case, only the rightmost or
leftmost group can be produced by `\eqalignno` or `\leqalignno`.

You can use the `\noalign` command to change the amount of space between
two lines of a multiline display.
]],
   eqno = [[
`\eqno`, `\leqno`

These commands attach an equation number to a displayed formula. `\eqno`
puts the equation number on the right and `\leqno` puts it on the left.
The commands must be given at the end of the formula. If you have a
multiline display and you want to number more than one of the lines, use
the `\eqalignno` or `\leqalignno` command .

These commands are valid only in display math mode.
]],
   errhelp = [[
This parameter contains the token list that TeX displays when you ask
for help in response to an `\errmessage` command. We recommend that when
you’re generating an error message with `\errmessage`, you set
`\errhelp` to a string that describes the nature of the error and use
`\newhelp` to produce that string. You can use the `\newlinechar`
character to produce multiline messages.
]],
   errorcontextlines = [[
This parameter determines the number of pairs of context lines, not
counting the top and bottom pairs, that TeX prints when it encounters an
error. By setting it to $0$ you can get rid of long error messages. You
can still force out the full context by typing something like:

    I\errorcontextlines=100\oops

in response to an error, since the undefined control sequence `\oops`
will cause another error. Plain TeX sets `\errorcontextlines` to $5$.
]],
   errorstopmode = [[
This command tells TeX to stop for interaction whenever it finds an
error. This is the normal mode of operation.
]],
   escapechar = [[
This parameter specifies the ASCII code of the character that TeX uses
to represent the escape character

when it’s converting a control sequence name to a sequence of character
tokens. This conversion occurs when you use the `\string` command and
also when TeX is producing diagnostic messages. The default value of the
escape character is $92$, the ASCII character code for a backslash. If
`\escapechar` is not in the range $0$–$255$, TeX does not include an
escape character in the result of the conversion.
]],
   everycr = [[
TeX expands `token list` whenever it executes a `\cr`—at the end of
every preamble, at the end of every row of a horizontal alignment, and
at the end of every column of a vertical alignment. The `\everycr`
commands are expanded just after the `\cr`. Thus you can cause TeX to
execute certain commands at the end of a preamble, row, or column by
assigning a list of those commands to `\everycr`.

The `\everycr` tokens shouldn’t include any commands other than
`\noalign`. That’s because the `\everycr` tokens will reappear after the
last `\cr` of the alignment. A command other than `\noalign` will then
make TeX think that it’s starting a new row or column. TeX will complain
about a missing `\cr`, insert a `\cr`, insert the `\everycr` tokens
again, and repeat these actions indefinitely.
]],
   everyhbox = [[
`\everyhbox`, `\everyvbox`

These parameters contain token lists that TeX expands at the start of
every hbox or vbox that it constructs. Any items resulting from the
expansion then become the beginning of the list of items for the box. By
default these token lists are empty.
]],
   everyjob = [[
This parameter contains a token list that TeX expands at the start of
every job. Because an assignment to `\everyjob` cannot affect the
current run (by the time you’ve done the assignment it’s already too
late), it is only useful to people who are preparing format files.
]],
   everymath = [[
`\everydisplay`, `\everymath`

These parameters specify token lists that TeX inserts at the start of
every text math or display math formula, respectively. You can take
special actions at the start of each math formula by assigning those
actions to `\everymath` or `\everydisplay`. Don’t forget that if you
want both kinds of formulas to be affected, you need to set *both*
parameters.
]],
   everypar = [[
TeX performs the commands in `token list` whenever it enters horizontal
mode, e.g., when it starts a paragraph. By default `\everypar` is empty,
but you can take extra actions at the start of every paragraph by
putting the commands for those actions into a token list and assigning
that token list to `\everypar`.
]],
   exhyphenpenalty = [[
This parameter specifies the penalty that TeX attaches to a breakpoint
at an explicit hyphen such as the one in “helter-skelter”. Increasing
this parameter has the effect of discouraging TeX from ending a line at
an explicit hyphen. Plain TeX sets `\exhyphenpenalty` to $50$.
]],
   expandafter = [[
This command tells TeX to expand `token_1` according to its rules for
macro expansion *after* it has expanded `token_2` by one level. It’s
useful when `token_1` is something like ‘`{`’

or `\string` that inhibits expansion of `token_2`, but you want to
expand `token_2` nevertheless.
]],
   fam = [[
When TeX is in math mode, it ordinarily typesets a character using the
font family given in its mathcode.

However, when TeX is in math mode and encounters a character whose class
is $7$ (Variable), it typesets that character using the font family
given by the value of `\fam`, provided that the value of `\fam` is
between $0$ and $15$. If the value of `\fam` isn’t in that range,
TeX uses the family in the character’s mathcode as in the ordinary case.
TeX sets `\fam` to $-1$ whenever it enters math mode. Outside of math
mode, `\fam` has no effect.

By assigning a value to `\fam` you can change the way that TeX typesets
ordinary characters such as variables. For instance, by setting `\fam`
to `\ttfam`, you cause TeX to typeset variables using a typewriter font.
Plain TeX defines `` as a macro that, among other things, sets `\fam` to
`\ttfam`.
]],
   fi = [[
This command ends the text of a conditional test.
]],
   filbreak = [[
This command provides a kind of conditional page break. It tells TeX to
break the page—but not if the text up to a later `\filbreak` also fits
on the same page. By enclosing a paragraph in a pair of `\filbreak`s,
you can ensure that TeX will keep a paragraph on a single page if it
can. You should not use `\filbreak` within a paragraph, since it forces
TeX into vertical mode and thus ends the paragraph. See in “TeX for the
Impatient” for more advice on this subject.
]],
   finalhyphendemerits = [[
 This parameter specifies additional demerits that TeX attaches to a
breakpoint that causes the next to last line of a paragraph to end with
a hyphen. Such a hyphen is generally considered to be unaesthetic
because of the possible blank space from a short last line beneath it.
Increasing the value of this parameter has the effect of discouraging
TeX from ending the next to the last line with a hyphen. Demerits are in
units of badness squared, so you need to assign a rather large value to
this parameter (in the thousands) for it to have any effect. Plain
TeX sets `\finalhyphendemerits` to $5000$.
]],
   firstmark = [[
`\botmark`, `\firstmark`, `\topmark`

These commands expand to the mark text in an item generated by an
earlier `\mark` command. The mark text has the form of a token list.
TeX sets the values of these commands when it finishes placing the
contents of a page into `\box255`, just before calling the output
routine as part of its page breaking actions. TeX determines these
values as follows:

-   `\firstmark` contains the tokens of the first mark on the page.

-   `\botmark` contains the tokens of the last mark on the page.

-   `\topmark` contains the tokens of the mark that is in effect at the
    very top of the page. That mark is the last mark that *preceded* the
    page, i.e., the `\botmark` of the previous page. It is empty if no
    marks preceded the page.

If a page has no marks on it, TeX will set `\firstmark` and `\botmark`
to the same mark as `\topmark`, i.e., the most recent preceding mark.
The table at the bottom of page 258 of *The TeXbook* illustrates the
relation among `\firstmark`, `\botmark`, and `\topmark`.
]],
   fivebf = [[
`\fivebf`, `\fivei`, `\fiverm`, `\fivesy`, `\sevenbf`, `\seveni`,
`\sevenrm`, `\sevensy`, `\tenbf`, `\tenex`, `\tenit`, `\teni`, `\tenrm`,
`\tensl`, `\tensy`, `\tentt`

These commands cause TeX to typeset the following text in the specified
font. Normally you would enclose one of these font-selecting commands in
a group, together with the text to be set in the selected font. Outside
of a group a font-selecting command is effective until the end of the
document (unless you override it with another such command).
]],
   floatingpenalty = [[
This parameter specifies the penalty that TeX adds to `\insertpenalties`

when the page builder is adding an insertion to the current page and
discovers that a previous insertion of the same kind on this page has
been split, leaving part of it for subsequent pages. Plain TeX leaves
`\floatingpenalty` at $0$.
]],
   fmtname = [[
`\fmtname`, `\fmtversion`

These commands produce the name and version number of the TeX format,
e.g., plain TeX or LaTeX, that you’re using.
]],
   folio = [[
This command produces the current page number, whose value is the number
$n$ contained in `\pageno`. If $n\ge0$, TeX produces $n$ as a decimal
number, while if $n<0$, TeX produces $-n$ in lowercase roman numerals.
]],
   font = [[
Used alone, the `\font` control sequence designates the current font.
`\font` isn’t a true command when it’s used alone, since it then can
appear only as an argument to another command.

For the other three forms of `\font`, `fontname` names a set of files
that define a font. These forms of `\font` are commands. Each of these
forms has two effects:

1.  It defines `control sequence` as a name that selects the font
    `fontname`, possibly magnified (see below).

2.  It causes TeX to load the font metrics file (.tfm file) for
    `fontname`.

The name of a font file usually indicates its design size. For example,
`cmr10` indicates Computer Modern roman with a design size of $10$
points. The design size of a font is recorded in its metrics file.

If neither `scaled` `number` nor `at` `dimen` is present, the font is
used at its design size—the size at which it usually looks best.
Otherwise, a magnified version of the font is loaded:

-   If `scaled` `number` is present, the font is magnified by a factor
    of $\texttt{number}/1000$.

-   If `at` `dimen` is present, the font is scaled to `dimen` by
    magnifying it by $\texttt{dimen}/ds$, where $ds$ is the design size
    of `fontname`. `dimen` and $ds$ are nearly always given in points.

Magnifications of less than $1$ are possible; they reduce the size.

You usually need to provide a shape file for each magnification of a
font that you load. However, some device drivers can utilize fonts that
are resident in a printer. Such fonts don’t need shape files.

See “TeX for the Impatient” and “TeX for the Impatient” for further
information.
]],
   fontdimen = [[
These parameters specify various dimensions associated with the font
named by the control sequence `font` (as distinguished from the
`fontname` that names the font files). Values of these parameters are
specified in the metrics file for `font`, but you can retrieve or change
their values during a TeX run. The numbers and meanings of the
parameters are:

TeX uses the slant per point for positioning accents. It uses the
interword parameters for producing interword spaces ) and the extra
space parameter for the additional space after a period). The values of
these parameters for the plain TeX fonts are enumerated on page 433 of
*The TeXbook*. Math symbol fonts have $15$ additional parameters, which
we won’t discuss here.

Beware: assignments to these parameters are *not* undone at the end of a
group. If you want to change these parameters locally, you’ll need to
save and restore their original settings explicitly.
]],
   fontname = [[
This command produces the filename for `font`. The filename is the
`fontname` that was used to define `font`.
]],
   footnote = [[
`\footnote`, `\vfootnote`

These commands produce footnotes. `argument_1` is the “reference mark”
for the footnote and `argument_2` is its text. The text can be several
paragraphs long if necessary and can contain constructs such as math
displays, but it shouldn’t contain any insertions (such as other
footnotes).

You shouldn’t use these commands inside a subformula of a math formula,
in a box within a box being contributed to a page, or in an insertion of
any kind. If you’re unsure whether these restrictions apply, you can be
safe by only using `\footnote` and `\vfootnote` directly within a
paragraph or between paragraphs.

These restrictions aren’t as severe as they seem because you can use
`\vfootnote` to footnote most anything. Both `\footnote` and
`\vfootnote` insert the reference mark in front of the footnote itself,
but `\vfootnote` doesn’t insert the reference mark into the text. Thus,
when you use `\vfootnote` you can explicitly insert the reference mark
wherever it belongs without concern about the context and place the
`\vfootnote` in the next paragraph. If you find that the footnote lands
on the page following the one where it belongs, move the `\vfootnote`
back to the previous paragraph. There are rare circumstances where
you’ll need to alter the text of your document in order to get a
footnote to appear on the same page as its reference mark.
]],
   frenchspacing = [[
`\frenchspacing`, `\nonfrenchspacing`

TeX normally adjusts the spacing between words to account for
punctuation marks. For example, it inserts extra space at the end of a
sentence and adds some stretch to the glue following any punctuation
mark there. The `\frenchspacing` command tells TeX to make the interword
spacing independent of punctuation, while the `\nonfrenchspacing`
command tells TeX to use its normal spacing rules. If you don’t specify
`\frenchspacing`, you’ll get TeX’s normal spacing.

See in “TeX for the Impatient” for advice on how to control TeX’s
treatment of punctuation at the end of sentences.
]],
   futurelet = [[
This command tells TeX to make `token_2` the meaning of
`control sequence` (as would be done with `\let`), and then to process
`token_1` and `token_2` normally. `\futurelet` is useful at the end of
macro definitions because it gives you a way of looking beyond the token
that TeX is about to process before it processes it.
]],
   gdef = [[
This command is equivalent to `\global\def`.
]],
   global = [[
This command makes the following definition or assignment global so that
it becomes effective independent of group boundaries. You can apply a
`\global` prefix to any kind of definition or assignment, including a
macro definition or a register assignment.
]],
   globaldefs = [[
This parameter controls whether or not TeX takes definitions and other
assignments to be global:

-   If `\globaldefs` is zero (as it is by default), a definition is
    global if and only if it is preceded by `\global` either explicitly
    or implicitly. (The `\gdef` and `\xdef` commands have an implicit
    `\global` prefix).

-   If `\globaldefs` is greater than zero, all assignments and
    definitions are implicitly prefixed by `\global`.

-   If `\globaldefs` is less than zero, all `\global` prefixes are
    ignored.
]],
   goodbreak = [[
This command ends a paragraph and also indicates to TeX that this is a
good place to break the page.
]],
   halign = [[
This command produces a horizontal alignment consisting of a sequence of
rows, where each row in turn contains a sequence of column entries.
TeX adjusts the widths of the column entries to accommodate the widest
one in each column.

A horizontal alignment can only appear when TeX is in a vertical mode.
We recommend that you first study alignments in general before you
attempt to use this command.

An alignment consists of a preamble followed by the text to be aligned.
The preamble, which describes the layout of the rows that follow,
consists of a sequence of column templates, separated by ‘`&`’ and ended
by `\cr`. Each row consists of a sequence of column entries, also
separated by ‘`&`’ and ended by `\cr`. Within a template, ‘`#`’
indicates where TeX should insert the corresponding text of a column
entry. In contrast, `\settabs` uses a fixed implicit template of ‘`#`’,
i.e., it just inserts the text as is.

TeX typesets each column entry in restricted horizontal mode, i.e., as
the contents of an hbox, and implicitly encloses it in a group.

The `to` form of this command instructs TeX to make the width of the
alignment be `dimen`, adjusting the space between columns as necessary.
The `spread` form of this command instructs TeX to make the alignment
wider by `dimen` than its natural width. These forms are like the
corresponding forms of `` .

See `\tabskip` for an example using the `to` form.
]],
   hang = [[
This command indents the second and subsequent lines of a paragraph by
`\parindent`, the paragraph indentation . Since the first line is
already indented by `\parindent` (unless you’ve cancelled the
indentation with ``), the entire paragraph appears to be indented by
`\parindent`.
]],
   hangafter = [[
`\hangafter`, `\hangindent`

These two parameters jointly specify “hanging indentation” for a
paragraph. The hanging indentation indicates to TeX that certain lines
of the paragraph should be indented and the remaining lines should have
their normal width.

`\hangafter` determines which lines are indented, while `\hangindent`
determines the amount of indentation and whether it occurs on the left
or on the right:

-   Let $n$ be the value of `\hangafter`. If $n < 0$, the first $-n$
    lines of the paragraph will be indented. If $n\ge0$, all but the
    first $n$ lines of the paragraph will be indented.

-   Let $x$ be the value of `\hangindent`. If $x\ge0$, the lines will be
    indented by $x$ on the left. If $x<0$ the lines will be indented by
    $-x$ on the right.

When you specify hanging indentation, it applies only to the next
paragraph (if you’re in vertical mode) or to the current paragraph (if
you’re in horizontal mode). TeX uses the values of `\hangafter` and
`\hangindent` at the end of a paragraph, when it breaks that paragraph
into lines.

Unlike most of the other paragraph-shaping parameters, `\hangafter` and
`\hangindent` are reset to their default values at the start of each
paragraph, namely, $1$ for `\hangafter` and $0$ for `\hangindent`. If
you want to typeset a sequence of paragraphs with hanging indentation,
use `\everypar`.

If you specify `\hangafter` and `\hangindent` as well as `\parshape`,
TeX ignores the `\hangafter` and `\hangindent`.
]],
   hbadness = [[
`\hbadness`, `\vbadness`

These parameters specify the thresholds of horizontal and vertical
badness for reporting underfull or overfull boxes. `\hbadness` applies
to hboxes and `\vbadness` applies to vboxes. If the badness of a
constructed box exceeds the threshold, TeX will report an error. If you
raise the thresholds (the plain TeX defaults are both $1000$), TeX will
be less likely to complain. Note that the settings of `\hbadness` and
`\vbadness` have no effect on the appearance of your typeset document;
they only affect the error messages that you get. See page 302 of *The
TeXbook* for a precise description of how TeX decides when to complain
about an overfull or underfull box.
]],
   hbox = [[
This command produces an hbox (horizontal box) containing
`horizontal mode material`. The braces around `horizontal mode material`
define a group. TeX doesn’t break the `horizontal mode material` into
lines, since it’s in restricted horizontal mode when it’s assembling the
box. TeX won’t change the size of the box once it’s been produced.

`` is often useful when you want to keep some text all on one line. If
your use of `` prevents TeX from breaking lines in an acceptable way,
TeX will complain about an overfull hbox.

The width of the hbox depends on the arguments to ``:

-   If you specify only `horizontal mode material`, the hbox will have
    its natural width.

-   If you specify `to` `dimen`, the width of the hbox will be `dimen`.

-   If you specify `spread` `dimen`, the width of the hbox will be its
    natural width plus `dimen`, i.e., the hbox will be spread out by
    `dimen`.

The `\hfil` command is useful for filling out an hbox with empty space
when the material in the box isn’t as wide as the width of the box.
]],
   hfil = [[
`\hfill`, `\hfil`, `\vfill`, `\vfil`

These commands produce infinitely stretchable horizontal and vertical
glue that overwhelms any finite stretch that may be present. `\hfil` and
`\hfill` produce horizontal glue, while `\vfil` and `\vfill` produce
vertical glue.

`\hfill` is infinitely larger than `\hfil`. If both `\hfill` and `\hfil`
appear in the same box, the `\hfill` will consume all the available
extra space and the `\hfil` will be effectively ignored. `\hfill` can in
turn be overwhelmed by `\hskip 0pt plus 1filll`. The glue produced by
`\hfil` and `\hfill` never shrinks.

The behavior of `\vfil` and `\vfill` is analogous.
]],
   hfilneg = [[
`\hfilneg`, `\vfilneg`

These commands cancel the effect of a preceding `\hfil` or `\vfil`.
While `\hfil` and `\vfil` produce infinitely stretchable positive glue,
`\hfilneg` and `\vfilneg` produce infinitely stretchable negative glue.
(Thus, $n$ `\hfilneg`s cancel $n$ `\hfil`s, and similarly for
`\vfilneg`.) The main use of `\hfilneg` and `\vfilneg` is to counteract
the effect of an `\hfil` or `\vfil` inserted by a macro.

`\hfilneg` and `\vfilneg` have the curious property that if they are the
only infinitely stretchable glue in a box, they produce exactly the same
effect as `\hfil` and `\vfil`.
]],
   hfuzz = [[
`\hfuzz`, `\vfuzz`

These parameters specify the amount that a box can exceed its natural
size before TeX considers it to be overfull. `\hfuzz` applies to hboxes
and `\vfuzz` applies to vboxes. Plain TeX sets both parameters to
`0.1pt`.
]],
   hglue = [[
`\hglue`, `\vglue`

The `\hglue` command produces horizontal glue that won’t disappear at a
line break; the `\vglue` command produces vertical glue that won’t
disappear at a page break. In other respects these commands are just
like `\hskip` and `\vskip`. You can use `\vglue` to produce blank space
at the top of a page, e.g., above a title on the first page of a
document, but `\topglue` (next) is usually better for this purpose.
]],
   hidewidth = [[
This command tells TeX to ignore the width of the next column entry in a
horizontal alignment. It’s useful when you have an entry that is longer
than most of the others in the same column, and you’d rather have that
entry stick out of the column than make all the entries in the column
wider. If the `\hidewidth` is at the left of the entry, the entry sticks
out to the left; if the `\hidewidth` is at the right of the entry, the
entry sticks out to the right.
]],
   hoffset = [[
`\hoffset`, `\voffset`

TeX normally takes the “origin” of a page, that is, the point where it
starts printing, as being one inch down from the top of the page and one
inch to the right of the left end of the page.[1] The values of
`\hoffset` and `\voffset` give the horizontal and vertical offset of the
actual origin from this point. Thus if `\hoffset` and `\voffset` are
both zero, TeX uses its normal origin.

[1] TeX itself is indifferent to where the origin of the page is, but
this information has to be built into the device drivers

that convert .dvi files into printable form so that different devices
will yield the same results.
]],
   holdinginserts = [[
If this parameter is greater than $0$ when TeX is processing a page
break, TeX will refrain from processing insertions. Setting this
parameter to $1$ can be useful when you’re writing an output routine
that needs to reprocess the contents of the page, e.g., an output
routine that uses a value of `\vsize` different from the one used by the
page builder.
]],
   hphantom = [[
`\hphantom`, `\vphantom`

These commands produce phantom boxes that don’t print anything:

-   `\hphantom` produces a box with the same width as `argument` but
    zero height and depth.

-   `\vphantom` produces a box with the same height and depth as
    `argument` but zero width.

Their main purpose is to force a subformula to have a certain minimum
horizontal or vertical dimension.
]],
   hrule = [[
`\hrule`, `\vrule`

The `\hrule` command produces a horizontal rule; the `\vrule` command
produces a vertical rule. You can specify any or all of the width,
height, and depth of the rule—TeX supplies default values for those that
you omit. You can give the dimensions of the rule in any order; the
forms listed above show just two of the possible combinations. You can
even give a dimension of a given kind more than once—if you do, the last
one is the one that counts.

If you don’t specify the width of a horizontal rule, the rule is
extended horizontally to the boundaries of the innermost box or
alignment that contains the rule. If you don’t specify the height of a
horizontal rule, it defaults to `0.4pt`; if you don’t specify the depth
of a horizontal rule, it defaults to `0pt`.

If you don’t specify the width of a vertical rule, it defaults to
`0.4pt`. If you don’t specify the height or the depth of a vertical
rule, the rule is extended to the boundary of the innermost box or
alignment that contains the rule.

TeX treats a horizontal rule as an inherently vertical item and a
vertical rule as an inherently horizontal item. Thus a horizontal rule
is legal only in a vertical mode, while a vertical rule is legal only in
a horizontal mode.

If this seems surprising, visualize it—a horizontal rule runs from left
to right and separates vertical items in a sequence, while a vertical
rule runs up and down and separates horizontal items in a sequence.
]],
   hsize = [[
This parameter specifies the current line length. See in “TeX for the
Impatient” for a more complete explanation.
]],
   hskip = [[
`\hskip`, `\vskip`

These commands produce horizontal and vertical glue respectively. In the
simplest and most common case when only `dimen_1` is present, `\hskip`
skips to the right by `dimen_1` and `\vskip` skips down the page by
`dimen_1`. More generally, these commands produce glue whose natural
size is `dimen_1`, whose stretch is `dimen_2`, and whose shrink is
`dimen_3`. Either the `plus` `dimen_2`, the `minus ``dimen_3`, or both
can be omitted. If both are present, the `plus` must come before the
`minus`. An omitted value is taken to be zero. Any of the `dimen`s can
be negative.

You can use `\hskip` in math mode, but you can’t use `mu` units for any
of the dimensions. If you want `mu` units, use `\mskip` instead.
]],
   hss = [[
`\hss`, `\vss`

These commands produce horizontal and vertical glue that is both
infinitely stretchable and infinitely shrinkable. The glue can shrink to
a negative distance, producing the effect of backspacing along a line
(for `\hss`) or moving back up a page (for `\vss`).
]],
   ht = [[
`\dp`, `\ht`, `\wd`

These parameters refer to the height, depth, and width respectively of
box register `register`. You can use them to find out the dimensions of
a box. You can also change the dimensions of a box, but it’s a tricky
business; if you want to be adventurous you can learn all about it from
pages 388–389 of *The TeXbook*.
]],
   hyphenchar = [[
TeX doesn’t necessarily use the ‘-’ character at hyphenation points.
Instead, it uses the `\hyphenchar` of the current font, which is usually
‘-’ but need not be. If a font has a negative `\hyphenchar` value,
TeX won’t hyphenate words in that font.

Note that `font` is a control sequence that names a font, not a
`fontname` that names font files. Beware: an assignment to `\hyphenchar`
is *not* undone at the end of a group. If you want to change
`\hyphenchar` locally, you’ll need to save and restore its original
value explicitly.
]],
   hyphenpenalty = [[
This parameter specifies the penalty that TeX attaches to a breakpoint
at an implicit hyphen. Implicit hyphens can come from TeX’s hyphenation
dictionary or from discretionary hyphens that you’ve inserted with `` .

Increasing this parameter has the effect of discouraging TeX from
hyphenating words. Plain TeX sets `\hyphenpenalty` to $50$.
]],
   i = [[
`\i` (ı), `\j` (ȷ)

These commands produce dotless versions of the letters ‘i’ and ‘j’. You
should use them instead of the ordinary ‘i’ and ‘j’ when you are putting
an accent above those letters in ordinary text.

Use the `\imath` and `\jmath` commands for dotless ‘i’s and ‘j’s in math
formulas.
]],
   ialign = [[
This command behaves just like `\halign`, except that it first sets the
`\tabskip` glue to zero and sets `\everycr` empty.
]],
   ["if"] = [[
=1em This command tests if `token_1` and `token_2` have the same
character code, independent of their category codes. Before performing
the test, TeX expands tokens following the `\if` until it obtains two
tokens that can’t be expanded further. These two tokens become `token_1`
and `token_2`. The expansion includes replacing a control sequence
`\let` equal to a character token by that character token. A control
sequence that can’t be further expanded is considered to have character
code $256$.
]],
   ifcat = [[
This command tests if `token_1` and `token_2` have the same category
code. Before performing the test, TeX expands tokens following the
`\ifcat` until it obtains two tokens that can’t be expanded further.
These two tokens become `token_1` and `token_2`. The expansion includes
replacing a control sequence `\let` equal to a character token by that
character token. A control sequence that can’t be further expanded is
considered to have category code $16$.
]],
   ifdim = [[
This command tests if `dimen_1` and `dimen_2` satisfy `relation`, which
must be either ‘`<`’, ‘`=`’, or ‘`>`’. The dimensions can be constants
such as `1in`, dimension registers such as `\dimen6`, or dimension
parameters such as `\parindent`. Before performing the test, TeX expands
tokens following the `\ifdim` until it obtains a sequence of tokens
having the form `dimen_1` `relation` `dimen_2`, followed by a token that
can’t be part of `dimen_2`.
]],
   ifeof = [[
This command tests an input stream for end of file. It is true if input
stream `number` has not been opened, or has been opened and the
associated file has been entirely read in (or doesn’t exist).
]],
   ifhbox = [[
`\ifhbox`, `\ifvbox`, `\ifvoid`

These commands test the contents of the box register numbered
`register`. Let `register` be $n$. Then:

-   `\ifhbox` is true if `\box`$\,n$ is an hbox.

-   `\ifvbox` is true if `\box`$\,n$ is an vbox.

-   `\ifvoid` is true if `\box`$\,n$ is void, i.e, doesn’t have a box in
    it.
]],
   ifhmode = [[
`\ifhmode`, `\ifinner`, `\ifmmode`, `\ifvmode`

These commands test what mode TeX is in:

-   `\ifhmode` is true if TeX is in ordinary or restricted horizontal
    mode.

-   `\ifvmode` is true if TeX is in ordinary or internal vertical mode.

-   `\ifmmode` is true if TeX is in text math or display math mode.

-   `\ifinner` is true if TeX is in an “internal” mode: restricted
    horizontal, internal vertical, or text math.
]],
   ifnum = [[
This command tests if `number_1` and `number_2` satisfy `relation`,
which must be either ‘`<`’, ‘`=`’, or ‘`>`’. The numbers can be
constants such as `127`, count registers such as `\pageno` or
`\count22`, or numerical parameters such as `\hbadness`. Before
performing the test, TeX expands tokens following the `\ifnum` until it
obtains a sequence of tokens having the form `number_1` `relation`
`number_2`, followed by a token that can’t be part of `number_2`.
]],
   ifodd = [[
This command tests if `number` is odd. Before performing the test,
TeX expands tokens following the `\ifodd` until it obtains a sequence of
tokens having the form `number`, followed by a token that can’t be part
of `number`.
]],
   iftrue = [[
`\iffalse`, `\iftrue`

These commands are equivalent to tests that are always true or always
false. The main use of these commands is in defining macros that keep
track of the result of a test.
]],
   ifx = [[
This command tests if `token_1` and `token_2` agree. Unlike `\if` and
`\ifcat`, `\ifx` does *not* expand the tokens following `\ifx`, so
`token_1` and `token_2` are the two tokens immediately after `\ifx`.
There are three cases:

1.  If one token is a macro and the other one isn’t, the tokens don’t
    agree.

2.  If neither token is a macro, the tokens agree if:

    1.  both tokens are characters (or control sequences denoting
        characters) and their character codes and category codes agree,
        or

    2.  both tokens refer to the same TeX command, font, etc.

3.  If both tokens are macros, the tokens agree if:

    1.  their “first level” expansions, i.e., their replacement texts,
        are identical, and

    2.  they have the same status with respect to `\long` and `\outer`.

    Note in particular that *any two undefined control sequences agree*.

This test is generally more useful than `\if`.
]],
   ignorespaces = [[
This command tells TeX to read and expand tokens until it finds one that
is not a space token, ignoring any space tokens that it finds on the
way. `\ignorespaces` is often useful at the end of a macro as a way of
making the macro insensitive to any spaces or ends of line that might
follow calls on it. (An empty line after `\ignorespaces` still produces
a `\par` token, however.)
]],
   immediate = [[
This command should precede an `\openout`, `\closeout`, or `\write`.

It tells TeX to perform the specified file operation without delay.
]],
   indent = [[
If TeX is in vertical mode, as it is after ending a paragraph, this
command inserts the `\parskip` interparagraph glue, puts TeX into
horizontal mode, starts a paragraph, and indents that paragraph by
`\parindent`. If TeX is already in horizontal mode, this command merely
produces a blank space of width `\parindent`. Two ``s in a row produce
two indentations.

As the example below shows, an `` at a point where TeX would start a
paragraph anyway is redundant. When TeX is in vertical mode and sees a
letter or some other inherently horizontal command, it starts a
paragraph by switching to horizontal mode, doing an ``, and processing
the horizontal command.
]],
   infty = [[
`\Im` (ℑ), `\Re` (ℜ), `\Vert` (‖), `\aleph` (ℵ), `\angle` (∠),
`\backslash` (∖), `\bot` (⊥), `\clubsuit` (♣), `\diamondsuit` (♢),
`\ell` (ℓ), `\emptyset` (∅), `\exists` (∃), `\flat` (♭), `\forall` (∀),
`\hbar` (ℏ), `\heartsuit` (♡), `\imath` (𝚤), `\infty` (∞), `\jmath` (𝚥),
`\lnot` (¬), `\nabla` (∇), `\natural` (♮), `\neg` (¬), `\partial` (∂),
`\prime`, `\sharp` (♯), `\spadesuit` (♠), `\surd`, `\top` (⊤),
`\triangle` (△), `\vert`, `\wp` (℘)

These commands produce various symbols. They are called “ordinary
symbols” to distinguish them from other classes of symbols such as
relations. You can only use an ordinary symbol within a math formula, so
if you need an ordinary symbol within ordinary text you must enclose it
in dollar signs (`$`).

The commands `\imath` and `\jmath` are useful when you need to put an
accent on top of an ‘$i$’ or a ‘$j$’.

An apostrophe (`'`) is a short way of writing a superscript `\prime`.
(The `\prime` command by itself generates a big ugly prime.)

The `\|` and `\Vert` commands are synonymous, as are the `\neg` and
`\lnot` commands.

The `\vert` command produces the same result as ‘`|`’.

The symbols produced by `\backslash`, `\vert`, and `\Vert` are
delimiters. These symbols can be produced in larger sizes by using
`\bigm` et al.
]],
   input = [[
This command tells TeX to read its input from file `filename`. When that
file is exhausted, TeX returns to reading from its previous input
source. You can nest input files to any level you like (within reason).

When you’re typesetting a large document, it’s usually a good idea to
structure your main file as a sequence of `\input` commands that refer
to the subsidiary parts of the document. That way you can process the
individual parts easily as you’re working on drafts. It’s also a good
practice to put all of your macro definitions into a separate file and
summon that file with an `\input` command as the first action in your
main file.

TeX uses different rules for scanning file names than it does for
scanning tokens in general). If your implementation expects file names
to have extensions (usually indicated by a preceding dot), then
TeX provides a default extension of `.tex`.
]],
   inputlineno = [[
This command yields a number (not a string) giving the line number of
the current line, defined to be the number that would appear in an error
message if an error occurred at this point.
]],
   insert = [[
This primitive command provides the underlying mechanism for
constructing insertions, but it is hardly ever used outside of a macro
definition. The definitions of the `\footnote`, `\vfootnote`,
`\topinsert`, `\midinsert`, and `\pageinsert` commands are all built
around `\insert`.

When you design insertions for a document, you should assign a different
integer code[1] $n$ to each kind of insertion, using the `\newinsert`
command to obtain the integer codes. The `\insert` command itself
appends the `vertical mode material` to the current horizontal or
vertical list. Your output routine is responsible for moving the
inserted material from where it resides in `\box`$\,n$ to an output
page.

TeX groups together all insertions having the same code number. Each
insertion code $n$ has four registers associated with it:

-   `\box`$\,n$ is where TeX accumulates the material for insertions
    with code $n$. When TeX breaks a page, it puts into `\box`$\,n$ as
    much insertion $n$ material as will fit on the page. Your output
    routine should then move this material to the actual page. You can
    use `\ifvoid` to test if there is any material in `\box`$\,n$. If
    not all the material fits, TeX saves the leftovers for the next
    page.

-   `\count`$\,n$ is a magnification factor $f.$ When TeX is computing
    the vertical space occupied on the page by insertion $n$ material,
    it multiplies the vertical extent of this material by $f/1000$. Thus
    you would ordinarily set $f$ to $500$ for a double-column insertion
    and to $0$ for a marginal note.

-   `\dimen`$\,n$ specifies the maximum amount of insertion $n$ material
    that TeX will put on a single page.

-   `\skip`$\,n$ specifies extra space that TeX allocates on the page if
    the page contains any insertion $n$ material. This space is in
    addition to the space occupied by the insertion itself. For example,
    it would account for the space on a page above the footnotes (if
    there are any).

TeX sets `\box`$\,n$, and you should set the other three registers so
that TeX can correctly compute the vertical space required by the
insertion. See pages 122–125 of *The TeXbook* for further details of how
TeX processes this command and of how insertions interact with page
breaking.

[1] *The TeXbook* uses the term “class” for a code. We use a different
term to avoid confusion with the other meaning of “class”.
]],
   insertpenalties = [[
This parameter contains the sum of certain penalties that
TeX accumulates as it is placing insertions onto the current page. These
penalties are incurred when TeX is processing an `\insert` command and
discovers that a previous insertion of the same kind on this page has
been split, leaving part of it for subsequent pages. See pages 123–125
of *The TeXbook* for the details of this calculation.

`\insertpenalties` has an entirely different meaning during an

output routine—it’s the number of insertions that have been seen but
that don’t fit on the current page (see page 125 of *The TeXbook*).
]],
   interlinepenalty = [[
This parameter specifies the penalty for breaking a page between the
lines of a paragraph. By setting it to $10000$ you can force all page
breaks to occur between paragraphs, provided that the pages have enough
stretch so that TeX can still compose them decently. Plain TeX leaves
`\interlinepenalty` at $0$.
]],
   item = [[
`\itemitem`, `\item`

These commands are useful for creating itemized lists. The entire
paragraph following `argument` is indented by `\parindent`

(for `\item`) or by `2\parindent` (for `\itemitem`). (See in “TeX for
the Impatient” for an explanation of `\parindent`.) Then `argument`,
followed by an en space, is placed just to the left of the text of the
first line of the paragraph so that it falls within the paragraph
indentation as specified by `\parindent`.

If you want to include more than one paragraph in an item, put `\item{}`
in front of the additional paragraphs.
]],
   itfam = [[
`\bffam`, `\itfam`, `\slfam`, `\ttfam`

These commands define type families for use in math mode. Their
principal use is in defining the `\it`, `\bf`, `\sl`, and `` commands so
that they work in math mode.
]],
   jobname = [[
This command produces the base name of the file with which TeX was
invoked. For example, if your main input file is `hatter.tex`,
`\jobname` will expand to `hatter`. `\jobname` is most useful when
you’re creating an auxiliary file to be associated with a document.
]],
   jot = [[
This parameter defines a distance that is equal to three points (unless
you change it). The `\jot` is a convenient unit of measure for opening
up math displays.
]],
   kern = [[
The effect of this command depends on the mode that TeX is in when it
encounters it:

-   In a horizontal mode, TeX moves its position to the right (for a
    positive kern) or to the left (for a negative kern).

-   In a vertical mode, TeX moves its position down the page (for a
    positive kern) or up the page (for a negative kern).

Thus a positive kern produces empty space while a negative kern causes
TeX to back up over something that it’s already produced. This notion of
a kern is different from the notion of a kern in some computerized
typesetting systems—in TeX, positive kerns push two letters *apart*
instead of bringing them closer together.

A kern is similar to glue, except that (a) a kern can neither stretch
nor shrink, and (b) TeX will only break a line or a page at a kern if
the kern is followed by glue and is not part of a math formula. If
TeX finds a kern at the end of a line or a page, it discards the kern.
If you want to get the effect of a kern that never disappears, use
`\hglue` or `\vglue`.

You can use `\kern` in math mode, but you can’t use `mu` units for
`dimen`. If you want `mu` units, use `\mkern` instead.
]],
   language = [[
Different languages have different sets of hyphenation rules. This
parameter determines the set of hyphenation rules that TeX uses. By
changing `\language` you can get TeX to hyphenate portions of text or
entire documents according to the hyphenation rules appropriate to a
particular language.

Your local information about TeX will tell you if any additional sets of
hyphenation rules are available (besides the ones for English) and what
the appropriate values of `\language` are. The default value of
`\language` is $0$.

TeX sets the current language to $0$ at the start of every paragraph,
and compares `\language` to the current language whenever it adds a
character to the current paragraph. If they are not the same, TeX adds a
whatsit indicating the language change. This whatsit is the clue in
later processing that the language rules should change.
]],
   lastkern = [[
`\lastbox`, `\lastkern`, `\lastpenalty`, `\lastskip`

These control sequences yield the value of the last item on the current
list. They aren’t true commands because they can only appear as part of
an argument. If the last item on the list isn’t of the indicated type,
they yield a zero value (or an empty box, in the case of `\lastbox`).
For example, if the last item on the current list is a kern, `\lastkern`
yields the dimension of that kern; if it isn’t a kern, it yields a
dimension of $0$.

Using `\lastbox` has the additional effect of removing the last box from
the list. If you want the original `\lastbox` to remain on the list, you
have to add a copy of it to the list. `\lastbox` is not permitted in a
math list or in the main vertical list.

These control sequences are most useful after macro calls that might
have inserted entities of the indicated kinds.
]],
   lbrace = [[
`\langle` (⟨), `\lbrace`, `\lbrack`, `\lceil` (⌈), `\lfloor` (⌊),
`\rangle` (⟩), `\rbrace`, `\rbrack`, `\rceil` (⌉), `\rfloor` (⌋)

These commands produce left and right delimiters. Mathematicians use
delimiters to indicate the boundaries between parts of a formula. Left
delimiters are also called “openings”, and right delimiters are also
called “closings”. Openings and closings are two of TeX’s classes of
math symbols. TeX puts different amounts of space around different
classes of math symbols. You might expect the space that TeX puts around
openings and closings to be symmetrical, but in fact it isn’t.

Some left and right delimiters have more than one command that you can
use to produce them:

-   ‘$\{$’ (`\lbrace` and `\{`)

-   ‘$\}$’ (`\rbrace` and `\}`)

-   ‘$[$’ (`\lbrack` and ‘`[`’)

-   ‘$]$’ (`\rbrack` and ‘`]`’)

You can also use the left and right bracket characters (in either form)
outside of math mode.

In addition to these commands, TeX treats ‘`(`’ as a left delimiter and
‘`)`’ as a right delimiter.

You can have TeX choose the size for a delimiter by using `\left` and
`\right`. Alternatively, you can get a delimiter of a specific size by
using one of the `\big`$x$ commands).
]],
   lccode = [[
`\lccode`, `\uccode`

The `\lccode` and `\uccode` values for the $256$ possible input
characters specify the correspondence between the lowercase and
uppercase forms of letters. These values are used by the `\lowercase`
and `\uppercase` commands respectively and by TeX’s hyphenation
algorithm.

TeX initializes the values of `\lccode` and `\uccode` as follows:

-   The `\lccode` of a lowercase letter is the ASCII code for that
    letter.

-   The `\lccode` of an uppercase letter is the ASCII code for the
    corresponding lowercase letter.

-   The `\uccode` of an uppercase letter is the ASCII code for that
    letter.

-   The `\uccode` of a lowercase letter is the ASCII code for the
    corresponding uppercase letter.

-   The `\lccode` and `\uccode` of a nonletter are both zero.

Most of the time there’s no reason to change these values, but you might
want to change them if you’re using a language that has more letters
than English.
]],
   ldots = [[
`\cdots` (⋯), `\ldots` (…)

These commands produce three dots in a row. For `\ldots`, the dots are
on the baseline; for `\cdots`, the dots are centered with respect to the
axis).
]],
   leaders = [[
`\cleaders`, `\leaders`, `\xleaders`

These commands produce leaders, i.e., they fill a horizontal or vertical
space with copies of a pattern . The `box` or `rule` specifies a leader,
i.e., a single copy of the pattern, while the `skip command` specifies a
window to be filled with a row or a column of the leaders. The pattern
is repeated as many times as will fit into the window. If `skip command`
is a horizontal skip, the window contains a row of leaders and TeX must
be in a horizontal mode; if `skip command` is a vertical skip, the
window contains a column of leaders and TeX must be in a vertical mode.

The commands differ in how they arrange the repeated pattern in the
space and where they put any leftover space:

-   For `\leaders`, TeX aligns a row of leaders with the left end of the
    innermost box $B$ that is to contain the result of the `\leaders`
    command. It aligns a column of leaders with the top of $B$. Those
    leaders that fall entirely within the window are retained. Any
    leftover space at the top and bottom of the window is left empty.

-   For `\cleaders`, the leaders are centered within the window.

-   For `\xleaders` the pattern is uniformly distributed throughout the
    window. If the leftover space is $l$ and the leader is repeated $n$
    times, TeX puts space of width or height $l/(n+1)$ between adjacent
    leaders and at the two ends (left and right or top and bottom) of
    the leaders.
]],
   left = [[
`\left`, `\right`

These commands must be used together in the pattern:

This construct causes TeX to produce `subformula`, enclosed in the
delimiters `delim_1` and `delim_2`. The vertical size of the delimiter
is adjusted to fit the vertical size (height plus depth) of
`subformula`. `delim_1` and `delim_2` need not correspond. For instance,
you could use ‘`]`’ as a left delimiter and ‘`(`’ as a right delimiter
in a single use of `\left` and `\right`.

`\left` and `\right` have the important property that they define a
group, i.e., they act like left and right braces. This grouping property
is particularly useful when you put `\over` or a related command between
`\left` and `\right`, since you don’t need to put braces around the
fraction constructed by `\over`.

If you want a left delimiter but not a right delimiter, you can use
‘`.`’ in place of the delimiter you don’t want and it will turn into
empty space (of width `\nulldelimiterspace`).
]],
   leftarrow = [[
`\Downarrow` (⇓), `\Leftarrow` (⇐), `\Leftrightarrow` (⇔),
`\Longleftarrow` (⇐), `\Longleftrightarrow` (⇔), `\Longrightarrow` (⇒),
`\Rightarrow` (⇒), `\Uparrow` (⇑), `\Updownarrow` (⇕), `\downarrow` (↓),
`\gets` (←), `\hookleftarrow` (↩), `\hookrightarrow` (↪), `\leftarrow`
(←), `\leftharpoondown` (↽), `\leftharpoonup` (↼), `\leftrightarrow`
(↔), `\longleftarrow` (←), `\longleftrightarrow` (↔), `\longmapsto` (↦),
`\longrightarrow` (→), `\mapsto` (↦), `\nearrow` (↗), `\nwarrow` (↖),
`\rightarrow` (→), `\rightharpoondown` (⇁), `\rightharpoonup` (⇀),
`\rightleftharpoons` (⇌), `\searrow` (↘), `\swarrow` (↙), `\to` (→),
`\uparrow` (↑), `\updownarrow` (↕)

These commands provide arrows of different kinds. They are classified as
relations. The vertical arrows in the list are also delimiters, so you
can make them larger by using `\big` et al.

The command `\iff` differs from `\Longleftrightarrow` in that it
produces extra space to the left and right of the arrow.

You can place symbols or other legends on top of a left or right arrow
with `\buildrel`.
]],
   leftarrowfill = [[
`\leftarrowfill`, `\rightarrowfill`

These commands fill the enclosing horizontal space with left-pointing or
right-pointing arrows.
]],
   lefthyphenmin = [[
`\lefthyphenmin`, `\righthyphenmin`

These parameters specify the smallest word fragments that TeX allows at
the left and at the right end of a hyphenated word. Plain TeX defaults
them to $2$ and $3$ respectively; these are the recommended values for
English.
]],
   leftskip = [[
`\leftskip`, `\rightskip`

These parameters tell TeX how much glue to place at the left and at the
right end of each line of the current paragraph. We’ll just explain how
`\leftskip` works since `\rightskip` is analogous.

You can increase the left margin by setting `\leftskip` to a fixed
nonzero dimension. If you give `\leftskip` some stretch, you can produce
ragged left text, i.e., text that has an uneven left margin.

Ordinarily, you should enclose any assignment to `\leftskip` in a group
together with the affected text in order to keep its effect from
continuing to the end of your document. However, it’s pointless to
change `\leftskip`’s value inside a group that is in turn contained
within a paragraph—the value of `\leftskip` at the *end* of a paragraph
is what determines how TeX breaks the paragraph into lines.
]],
   let = [[
This command causes `control sequence` to acquire the current meaning of
`token`. Even if you redefine `token` later, the meaning of
`control sequence` will not change. Although `token` is most commonly a
control sequence, it can also be a character token.
]],
   lgroup = [[
`\lgroup`, `\rgroup`

These commands produce large left and right parentheses that are defined
as opening and closing delimiters. The smallest available size for these
delimiters is `\Big`. If you use smaller sizes, you’ll get weird
characters.
]],
   limits = [[
In text style, TeX normally places limits after a large operator. This
command tells TeX to place limits above and below a large operator
rather than after it.

If you specify more than one of `\limits`, `\nolimits`, and
`\displaylimits`, the last command rules.
]],
   line = [[
This command produces an hbox containing `argument`. The hbox is exactly
as wide as the current line, i.e., it extends from the right margin to
the left margin.
]],
   linepenalty = [[
This parameter specifies the penalty that TeX assesses for each line
break when it is breaking a paragraph into lines. The penalty is
independent of where the line break occurs. Increasing the value of this
parameter causes TeX to try harder to set a paragraph with a minimum
number of lines, even at the cost of other aesthetic considerations such
as avoiding overly tight interword spacing. Demerits are in units of
badness squared, so you need to assign a rather large value to this
parameter (in the thousands) for it to have any effect. Plain TeX sets
`\linepenalty` to $10$.
]],
   llap = [[
`\llap`, `\rlap`

These commands enable you to produce text that overlaps whatever happens
to be to the left or to the right of the current position. `\llap`
backspaces by the width of `argument` and then typesets `argument`.
`\rlap` is similar, except that it typesets `argument` first and then
backspaces. `\llap` and `\rlap` are useful for placing text outside of
the current margins. Both `\llap` and `\rlap` do their work by creating
a box of zero width.

You can also use `\llap` or `\rlap` to construct special characters by
overprinting, but don’t try it unless you’re sure that the characters
you’re using have the same width (which is the case for a monospaced
font such as `cmtt10`, the Computer Modern $10$-point typewriter font).
]],
   long = [[
This command is used as a prefix to a macro definition. It tells
TeX that the arguments to the macro are permitted to include `\par`
tokens, which normally indicate the end of a paragraph.

If TeX tries to expand a macro defined without `\long` and any of the
macro’s arguments include a `\par` token, TeX will complain about a
runaway argument. The purpose of this behavior is to provide you with
some protection against unterminated macro arguments. `\long` gives you
a way of bypassing the protection.
]],
   loop = [[
`\loop`, `\repeat`

These commands provide a looping construct for TeX. Here $\alpha$ and
$\beta$ are arbitrary sequences of commands and  is any of the
conditional tests described in “TeX for the Impatient”, Conditional
tests,conds. The `\repeat` replaces the `\fi` corresponding to the test,
so you must not write an explicit `\fi` to terminate the test. Nor,
unfortunately, can you associate an `\else` with the test. If you want
to use the test in the opposite sense, you need to rearrange the test or
define an auxiliary test with `\newif` (see above) and use that test in
the sense you want (see the second example below).

TeX expands `\loop` as follows:

1.  $\alpha$ is expanded.

2.  is performed. If the result is false, the loop is terminated.

3.  $\beta$ is expanded.

4.  The cycle is repeated.
]],
   looseness = [[
This parameter gives you a way to change the total number of lines in a
paragraph from what they optimally would be. `\looseness` is so named
because it’s a measure of how loose the paragraph is, i.e., how much
extra space there is in it.

Normally, `\looseness` is $0$ and TeX chooses line breaks in its usual
way. But if `\looseness` is, say, $3$, TeX does the following:

1.  It chooses line breaks normally, resulting in a paragraph of $n$
    lines.

2.  It discards these line breaks and tries to find a new set of line
    breaks that gives the paragraph $n+3$ lines. (Without the previous
    step, TeX wouldn’t know the value of $n$.)

3.  If the previous attempt results in lines whose badness exceeds
    `\tolerance`,

    it tries to get $n+2$ lines—and if that also fails, $n+1$ lines, and
    finally $n$ lines again.

Similarly, if looseness is $-n$, TeX attempts to set the paragraph with
$n$ fewer lines than normal. The easiest way for TeX to make a paragraph
one line longer is to put a single word on the excess line. You can
prevent this by putting a tie between the last two words of the
paragraph.

Setting `\looseness` is the best way to force a paragraph to occupy a
given number of lines. Setting it to a negative value is useful when
you’re trying to increase the amount of text you can fit on a page.
Similarly, setting it to a positive value is useful when you’re trying
to decrease the amount of text on a page.

TeX sets `\looseness` to $0$ when it ends a paragraph, after breaking
the paragraph into lines. If you want to change the looseness of several
paragraphs, you must do it individually for each one or put the change
into `\everypar` .
]],
   lower = [[
`\lower`, `\raise`

These commands move `box` up or down by `dimen` (which can be negative).
You can only apply `\raise` and `\lower` to a box that’s in a horizontal
list.
]],
   lowercase = [[
`\lowercase`, `\uppercase`

These commands convert the letters in `token list`, i.e., those tokens
with category code $11$, to their lowercase and uppercase forms. The
conversion of a letter is defined by its `\lccode` (for lowercase) or
`\uccode` (for uppercase) table value. Tokens in the list that are not
letters are not affected—even if the tokens are macro calls or other
commands that expand into letters.
]],
   lq = [[
`\P` (¶), `\S` (§), `\copyright` (©), `\dag`, `\ddag`, `\lbrack`, `\lq`
(‘), `\rbrack`, `\rq` (’)

These commands produce various special characters and marks. The first
five commands are necessary because TeX by default attaches special
meanings to the characters (`#`, `$`, `%`, `&`, `_`). You needn’t be in
math mode to use these commands.

You can use the dollar sign in the Computer Modern italic fonts to get
the pound sterling symbol, as shown in the example below.
]],
   magnification = [[
`\magnification`, `\mag`

An assignment to `\magnification` establishes the “scale factor” $f$
that determines the magnification ratio of your document . The
assignment to `\magnification` must occur before the first page of your
document has been shipped out.

The assignment sets $f$ to `number` and also sets `\hsize` and `\vsize`

respectively to `6.5true in` and `8.9true in`, the values appropriate
for an $8 \frac1/2$- by-$11$-inch page. $f$ must be between $0$ and
$32768$. The magnification ratio of the document is $f/1000$. A scale
factor of $1000$ provides unit magnification, i.e., it leaves the size
of your document unchanged. It’s customary to use powers of $1.2$ as
scale factors, and most libraries of fonts are based on such factors.
You can use the `\magstep` and `\magstephalf` commands to specify
magnifications by these factors.

`\magnification` is not a parameter. You can’t use it to *retrieve* the
scale factor. If you write something like `\dimen0 = \magnification`,
TeX will complain about it.

The `\mag` parameter contains the scale factor. Changing the value of
`\mag` rescales the page dimensions, which is not usually what you want.
Therefore it’s usually better to change the magnification by assigning
to `\magnification` rather than to `\mag`.
]],
   magstep = [[
This command expands to the magnification ratio needed to magnify
everything in your document (other than `true` dimensions) by $1.2^r$,
where $r$ is the value of `number`. `number` must be between $0$ and
$5$.
]],
   magstephalf = [[
This command expands to the magnification ratio needed to magnify
everything in your document (other than `true` dimensions) by
$\sqrt{1.2}$, i.e., halfway between $1$ and $1.2$.
]],
   mark = [[
This command causes TeX to append a mark containing `mark text` to
whatever list it is currently constructing. Generally you shouldn’t use
`\mark` within an “inner” construct such as a math formula or a box
you’ve built with an ``, `\vbox`, or `\vtop` command, because TeX won’t
see the mark when it’s constructing the main box of the page. But if you
use `\mark` in ordinary horizontal mode or directly in an hbox that’s
part of the main vertical list, the mark migrates out to the main
vertical list. See pages 259–260 of *The TeXbook* for examples showing
how `\mark` can be used.
]],
   mathaccent = [[
This command tells TeX to typeset a math accent whose family and
character code are given by `mathcode`. (TeX ignores the class of the
mathcode.) See Appendix G of *The TeXbook* for the details of how
TeX positions such an accent. The usual way to use `\mathaccent` is to
put it in a macro definition that gives a name to a math accent.
]],
   mathchar = [[
This command produces the math character whose class, family, and font
position are given by `mathcode`. It is only legal in math mode.
]],
   mathchardef = [[
This command defines `control sequence` as a math character with the
given `mathcode`. The control sequence will only be legal in math mode.
]],
   mathchoice = [[
This command tells TeX to typeset one of the subformulas `math_1`,
`math_2`, `math_3`, or `math_4`, making its choice according to the
current style. That is, if TeX is in display style it sets the
`\mathchoice` as `math_1`; in text style it sets it as `math_2`; in
script style it sets it as `math_3`; and in scriptscript style it sets
it as `math_4`.
]],
   mathcode = [[
This table entry contains the mathcode of the character whose ASCII code
is `charcode` . The mathcode specifies that character’s interpretation
in math mode.
]],
   mathinner = [[
This command tells TeX to treat the construct that follows as an “inner
formula”, e.g., a fraction, for spacing purposes. It resembles the class
commands given just above.
]],
   mathord = [[
`\mathbin`, `\mathclose`, `\mathopen`, `\mathop`, `\mathord`,
`\mathpunct`, `\mathrel`

These commands tell TeX to treat the construct that follows as belonging
to a particular class (see page 154 of *The TeXbook* for the definition
of the classes). They are listed here in the order of the class numbers,
from $0$ to $6$. Their primary effect is to adjust the spacing around
the construct to be whatever it is for the specified class.
]],
   mathpalette = [[
This command provides a convenient way of producing a math construct
that works in all four styles. To use it, you’ll normally need to define
an additional macro, which we’ll call `\build`. The call on
`\mathpalette` should then have the form `\mathpalette`||`argument`.

`\build` tests what style TeX is in and typesets `argument` accordingly.
It should be defined to have two parameters. When you call
`\mathpalette`, it will in turn call `\build`, with `#1` being a command
that selects the current style and `#2` being `argument`. Thus, within
the definition of `\build` you can typeset something in the current
style by preceding it with ‘`#1`’. See page 360 of *The TeXbook* for
examples of using `\mathpalette` and page 151 of *The TeXbook* for a
further explanation of how it works.
]],
   mathstrut = [[
This command produces a phantom formula whose width is zero and whose
height and depth are the same as those of a left parenthesis.
`\mathstrut` is in fact defined as ‘`\vphantom(`’. Its main use is for
getting radicals, underbars, and overbars to line up with other
radicals, underbars, and overbars in a formula. It is much like
`\strut`, except that it adjusts itself to the different styles that can
occur in math formulas.
]],
   mathsurround = [[
This parameter specifies the amount of space that TeX inserts before and
after a math formula in text mode (i.e., a formula surrounded by single
`$`’s). See page 162 of *The TeXbook* for further details about its
behavior. Plain TeX leaves `\mathsurround` at `0pt`.
]],
   maxdeadcycles = [[
If the value of `\deadcycles` exceeds the value of `\maxdeadcycles`,
TeX assumes that the output routine has gotten into a loop. TeX then
complains and runs its own simple output routine, equivalent to
`\shipout!allowbreak\box255`, that is likely to break the loop. Plain
TeX sets `\maxdeadcycles` to $25$.
]],
   maxdepth = [[
This parameter specifies the maximum depth of the bottom box on a page.
It is related to `\boxmaxdepth`. If the depth of the bottom box on a
page exceeds `\maxdepth`, TeX moves the box’s reference point down so
that it’s `\maxdepth` from the bottom of that box. Without this
adjustment, the bottom box on a page could extend well into the bottom
margin or even off the page entirely. Plain TeX sets `\maxdepth` to
`4pt`.
]],
   maxdimen = [[
This control sequence yields a `dimen` that is the largest dimension
acceptable to TeX (nearly 18 feet). It is not a true command because it
can only appear as part of an argument to another command.
]],
   meaning = [[
This command produces the meaning of `token`. It is useful for
diagnostic output. You can use the `\the` command in a similar way to
get information about the values of registers and other TeX entities.
]],
   message = [[
`\errmessage`, `\message`

These commands display the message given by `token list` on your
terminal and also enter it into the log. Any macros in the message are
expanded, but no commands are executed. This is the same rule that
TeX uses for `\edef`.

For `\errmessage`, TeX pauses in the same way that it does for one of
its own error messages and displays the `\errhelp` tokens if you ask for
help.

You can generate multiline messages by using the `\newlinechar`
character.
]],
   mkern = [[
This command produces a kern, i.e., blank space, of width `mudimen`. The
kern is measured in mathematical units, which vary according to the
style. Aside from its unit of measurement, this command behaves just
like `\kern` does in horizontal mode.
]],
   month = [[
TeX sets this parameter to the current month. It is a number between $1$
and $12$. `\month` is set at the beginning of your run (see the comments
on `\time` above).
]],
   moveleft = [[
`\moveleft`, `\moveright`

These commands move `box` left or right by `dimen` (which can be
negative). You can only apply `\moveleft` and `\moveright` to a box
that’s in a vertical list.
]],
   mskip = [[
This command produces horizontal glue that has natural width
`mudimen_1`, stretch `mudimen_2`, and shrink `mudimen_3`. The glue is
measured in mathematical units, which vary according to the style. Aside
from its units of measurement, this command behaves just like `\hskip`.
]],
   multiply = [[
`\divide`, `\multiply`

These commands multiply and divide the value in `register` by `number`
(which can be negative). The register can be a `\count`, `\dimen`,
`\skip`, or `\muskip` register. For a `\skip` or `\muskip` register, all
three components of the glue in the register are modified. You can omit
the word `by` in these commands—TeX will understand them anyway.

You can also obtain a multiple of a `dimen` by preceding it by a
`number`

or decimal constant, e.g., `-2.5\dimen2`. You can also use this notation
for `glue`, but watch out—the result is a `dimen`, not `glue`. Thus
`2\baselineskip` yields a `dimen` that is twice the natural size of
`\baselineskip`, with no stretch or shrink.
]],
   multispan = [[
This command tells TeX that the following `number` columns in a row of a
horizontal alignment, or `number` rows in a column of a vertical
alignment, should be combined into a single column or row (as with
`\span`) and that their templates should be omitted (as with `\omit`).
]],
   narrower = [[
This command makes paragraphs narrower, increasing the left and right
margins by `\parindent`, the current paragraph indentation. It achieves
this by increasing both `\leftskip` and `\rightskip` by `\parindent`.
Normally you place `\narrower` at the beginning of a group containing
the paragraphs that you want to make narrower. If you forget to enclose
`\narrower` within a group, you’ll find that all the rest of your
document will have narrow paragraphs.

`\narrower` affects just those paragraphs that end after you invoke it.
If you end a `\narrower` group before you’ve ended a paragraph,
TeX won’t make that paragraph narrower.
]],
   negthinspace = [[
This command produces a negative kern whose width is one-sixth of an em
, i.e., it causes TeX to move its position left by that amount. It is
useful for bringing together characters that are a little too far apart.
TeX won’t break a line at a `\negthinspace`.
]],
   newif = [[
This command names a trio of control sequences with names `\alphatrue`,
`\alphafalse`, and `\ifalpha`, where `alpha` is `test name`. You can use
them to define your own tests by creating a logical variable that
records true/false information:

-   `\alphatrue` sets the logical variable `alpha` true.

-   `\alphafalse` sets the logical variable `alpha` false

-   `\ifalpha` is a conditional test that is true if the logical
    variable `alpha` is true and false otherwise.

The logical variable `alpha` doesn’t really exist, but TeX behaves as
though it did. After `\newif\ifalpha`, the logical variable is initially
false.

`\newif` is an outer command, so you can’t use it inside a macro
definition.
]],
   newlinechar = [[
This parameter contains a character that indicates a new line on output.
When TeX encounters this character while reading the argument of a
`\write`, `\message`, or `\errmessage` command, it starts a new line. If
`\newlinechar` is not in the range $0$–$255$, there is no character that
indicates a new line on output. Plain TeX sets `\newlinechar` to $-1$.
]],
   noalign = [[
This command inserts `vertical mode material` after the current row of a
horizontal alignment or `horizontal mode material` after the current
column of a vertical alignment. The material can be text, glue, a rule,
or anything else.

The most common use of `\noalign` is to put extra space after a row or
column. If you want to put extra space after *every* row of a horizontal
alignment, use `\openup`.
]],
   noboundary = [[
You can defeat a ligature or kern that TeX applies to the first or last
character of a word by putting `\noboundary` just before or just after
the word. Certain fonts intended for languages other than English
contain a special boundary character that TeX puts at the beginning and
end of each word. The boundary character occupies no space and is
invisible when printed. It enables TeX to provide different
typographical treatment to characters at the beginning or end of a word,
since the boundary character can be part of a sequence of characters to
be kerned or replaced by a ligature. (None of the standard TeX fonts
contain this boundary character.) The effect of `\noboundary` is to
delete the boundary character if it’s there, thus preventing TeX from
recognizing the ligature or kern.
]],
   nobreak = [[
This command prevents a page break where it otherwise might occur.
`\nobreak` can also be used in horizontal mode.
]],
   noexpand = [[
This command tells TeX to suppress expansion of `token` if `token` is a
control sequence that can be expanded. If `token` can’t be expanded,
e.g., it’s a letter, TeX acts as though the `\noexpand` wasn’t there and
processes `token` normally. In other words the expansion of
‘`\noexpand``token`’ is simply `token` no matter what `token` happens to
be.
]],
   noindent = [[
If TeX is in vertical mode, as it is after ending a paragraph, this
command inserts the `\parskip` interparagraph glue, puts TeX into
horizontal mode, and starts an unindented paragraph. It has no effect in
horizontal mode, i.e., within a paragraph. Starting a paragraph with ``
thus cancels the indentation by `\parindent` that would normally occur
there.

A common use of `` is to cancel the indentation of the first line of a
paragraph when the paragraph follows some displayed material.
]],
   nointerlineskip = [[
This command tells TeX not to insert interline glue in front of the next
line. It has no effect on subsequent lines.
]],
   nolimits = [[
In display style, TeX normally places limits above and below a large
operator. This command tells TeX to place limits after a large operator
rather than above and below it.

The integral operators `\int` and `\oint` are exceptions—TeX places
limits after them in all cases, unless overridden, as in `\int\limits`.
(plain TeX defines `\int` and `\oint` as macros that specify the
operator symbol followed by `\nolimits`—this is what causes them to
behave differently by default.)

If you specify more than one of `\limits`, `\nolimits`, and
`\displaylimits`, the last command rules.
]],
   nonscript = [[
When TeX is currently typesetting in script or scriptscript style and
encounters this command immediately in front of glue or a kern, it
cancels the glue or kern. `\nonscript` has no effect in the other
styles.

This command provides a way of “tightening up” the spacing in script and
scriptscript styles, which generally are set in smaller type. It is of
little use outside of macro definitions.
]],
   nonstopmode = [[
This command tells TeX not to stop for errors, even those pertaining to
files that it can’t find, but to continue displaying the error messages
on your terminal. Typing ‘`R`’ or ‘`r`’ in response to an error message
puts you into nonstop mode.
]],
   nopagenumbers = [[
By default, plain TeX produces a footer containing a centered page
number. This command tells TeX to produce a blank footer instead.
]],
   normalbaselineskip = [[
`\normalbaselineskip`, `\normalbaselines`, `\normallineskiplimit`,
`\normallineskip`

The three parameters contain values for `\baselineskip`, `\lineskip`,
and `\lineskiplimit` respectively.

The `\normalbaselines` command sets `\baselineskip`, `\lineskip`, and
`\lineskiplimit` to the values contained in the three parameters.
]],
   null = [[
This command produces an empty hbox.
]],
   nulldelimiterspace = [[
This parameter specifies the width of the space produced by a null
delimiter. Plain TeX sets `\nulldelimiterspace` to `1.2pt`.
]],
   nullfont = [[
This command selects a font, built into TeX, that has no characters in
it. TeX uses it as a replacement for an undefined font in a family of
math fonts.
]],
   number = [[
This command produces the representation of a number as a sequence of
character tokens. The number can be either an explicit integer, a
`number` parameter, or a `number` register.
]],
   obeylines = [[
TeX normally treats an end of line as a space. `\obeylines` instructs
TeX to treat each end of line as an end of paragraph, thus forcing a
line break. `\obeylines` is often useful when you’re typesetting verse
or computer programs.

If any of your lines are longer than the effective line length (`\hsize`
- `\parindent`), however, you may get an extra line break within those
lines.

Because TeX inserts the `\parskip` glue between lines controlled by
`\obeylines` (since it thinks each line is a paragraph), you should
normally set `\parskip` to zero when you’re using `\obeylines`.

You can use the `\obeyspaces` command to get TeX to take spaces within a
line literally. `\obeylines` and `\obeyspaces` are often used together.
]],
   obeyspaces = [[
TeX normally condenses a sequence of several spaces to a single space.
`\obeyspaces` instructs TeX to produce a space in the output for each
space in the input. `\obeyspaces` does not cause spaces at the beginning
of a line to show up, however; for that we recommend the
`\obeywhitespace` command defined in `eplain.tex` .

`\obeyspaces` is often useful when you’re typesetting something,
computer input for example, in a monospaced font (one in which each
character takes up the same amount of space) and you want to show
exactly what each line of input looks like.

You can use the `\obeylines` command to get TeX to follow the line
boundaries of your input. `\obeylines` is often used in combination with
`\obeyspaces`.
]],
   offinterlineskip = [[
This command tells TeX to stop inserting interline glue from now on.
Unless you want it to be in effect for the rest of the document (which
you probably don’t), you should enclose it in a group together with the
text you want it to affect. Its main purpose is to let you do interline
spacing yourself, e.g., using struts, without interference from TeX’s
normal interline glue. `\offinterlineskip` is often useful when you’re
constructing a horizontal alignment.
]],
   omit = [[
This command tells TeX to ignore a template in a horizontal or vertical
alignment while processing a particular column or row entry
respectively. `\omit` must appear as the first item in a column or row
entry; in effect, it overrides the template from the preamble with the
simple template ‘`#`’.
]],
   openin = [[
This command tells TeX to open the file named `filename` and make it
available for reading via the input stream designated by `number`.

`number` must be between $0$ and $15$. Once you’ve opened a file and
connected it to an input stream, you can read from the file using the
`\read` command with the input stream’s number.

You can associate more than one input stream with the same file. You can
then read from several different positions within the file, one for each
input stream.

You should allocate stream numbers for `\openin` using `\newread`.
]],
   openout = [[
This command tells TeX to open the file named `filename` and make it
available for writing via the output stream designated by `number`.
`number` must be between $0$ and $15$. Once you’ve opened a file and
connected it to an output stream, you can write to the file using the
`\write` command with the output stream’s number.

An `\openout` generates a whatsit that becomes part of a box. The
`\openout` does not take effect until TeX ships out that box to the .dvi
file, unless you’ve preceded the `\openout` with `\immediate`.

TeX won’t complain if you associate more than one output stream with the
same file, but you’ll get garbage in the file if you try it!

You should allocate stream numbers for `\openout` using `\newwrite`.
]],
   openup = [[
This command increases `\baselineskip` by `dimen`. An `\openup` command
before the end of a paragraph affects the entire paragraph, so you
shouldn’t use `\openup` to change `\baselineskip` within a paragraph.
`\openup` is most useful for typesetting tables and math displays—a
little extra space between rows often makes them more readable.
]],
   outer = [[
This command is used as a prefix to a macro definition. It tells
TeX that the macro is outer and cannot be used in certain contexts. If
the macro is used in a forbidden context, TeX will complain.
]],
   output = [[
This parameter contains the current output routine, i.e., the token list
that TeX expands when it finds a page break. TeX puts the page into
`\box255`, so `\output` is responsible for doing something with
`\box255`—either shipping it out or putting it somewhere else. The
output routine is also responsible for attaching things such as headers
and footers.
]],
   outputpenalty = [[
TeX sets this parameter when it breaks a page. If the breakpoint was at
a penalty item, TeX removes the penalty item and sets `\outputpenalty`
to the penalty value at the breakpoint; otherwise it sets
`\outputpenalty` to $0$.

Suppose that you are undoing a page break in order to break the page at
a different place than the one that TeX has just chosen. In order to
reconstruct the page, you need to recreate the penalty at TeX’s chosen
breakpoint. You can accomplish this with the command
`\penalty!allowbreak\outputpenalty`.
]],
   over = [[
`\above`, `\atop`, `\brace`, `\brack`, `\choose`, `\over`

These commands stack one subformula on top of another one. We will
explain how `\over` works, and then relate the other commands to it.

`\over` is the command that you’d normally use to produce a fraction.

If you write something in one of the following forms:

    $$⟨formula1⟩\over⟨formula2⟩$$
    $⟨formula1⟩\over⟨formula2⟩$
    \left!<delim>⟨formula1⟩\over⟨formula2⟩\right!<delim>
    {⟨formula1⟩\over⟨formula2⟩}

you’ll get a fraction with numerator `formula_1` and denominator
`formula_2`, i.e., `formula_1` over `formula_2`. In the first three of
these forms the `\over` is not implicitly contained in a group; it
absorbs everything to its left and to its right until it comes to a
boundary, namely, the beginning or end of a group.

You can’t use `\over` or any of the other commands in this group more
than once in a formula. Thus a formula such as:

    $$a \over n \choose k$$

isn’t legal. This is not a severe restriction because you can always
enclose one of the commands in braces. The reason for the restriction is
that if you had two of these commands in a single formula, TeX wouldn’t
know how to group them.

The other commands are similar to `\over`, with the following
exceptions:

-   `\atop` leaves out the fraction bar.

-   `\above` provides a fraction bar of thickness `dimen`.

-   `\choose` leaves out the fraction bar and encloses the construct in
    parentheses. (It’s called “choose” because $n \choose k$ is the
    notation for the number of ways of choosing $k$ things out of $n$
    things.)

-   `\brace` leaves out the fraction bar and encloses the construct in
    braces.

-   `\brack` leaves out the fraction bar and encloses the construct in
    brackets.
]],
   overfullrule = [[
This parameter specifies the width of the rule that TeX appends to an
overfull hbox. Plain TeX sets it to `5pt`.
]],
   overwithdelims = [[
`\abovewithdelims`, `\atopwithdelims`, `\overwithdelims`

Each of these commands stacks one subformula on top of another one and
surrounds the entire construct with `delim_1` on the left and `delim_2`
on the right. These commands follow the same rules as `\over`, `\atop`,
and `\above`. The `dimen` in `\abovewithdelims` specifies the thickness
of the fraction bar.
]],
   pagedepth = [[
This parameter specifies the depth of the current page. TeX updates
`\pagedepth` as it adds items to the main vertical list.
]],
   pagegoal = [[
This parameter specifies the desired height for the current page.
TeX sets `\pagegoal` to the current value of `\vsize` when it first puts
a box or an insertion on the current page. You can shorten a page while
TeX is working on it by changing the value of `\pagegoal`—even if the
new value is less than the height of the material already on that page.
TeX will just put the extra material on the next page. But
remember—`\pagegoal` is reset to `\vsize` again when TeX starts the next
page.
]],
   pageno = [[
This parameter contains the current page number as an integer. The page
number is normally negative for front-matter pages that are numbered
with small roman numerals instead of arabic numerals. If you change the
page number within a page, the changed number will be used in any
headers or footers that appear on that page. The actual printing of page
numbers is handled by TeX’s output routine, which you can modify.

Plain TeX keeps the page number in the register `\count0`. (`\pageno`
is, in fact, a synonym for `\count0`.) Whenever it ships out a page to
the .dvi file,

TeX displays the current value of `\count0` on your terminal so that you
can tell which page it is working on. It’s possible to use registers
`\count1`–`\count9` for nested levels of page numbers (you must program
this yourself). If any of these registers are nonzero, TeX displays them
on your terminal also.[1]

[1] More precisely, it displays all registers in sequence from `\count0`
to `\count9`, but omits trailing zero registers. For instance, if the
values of `\count0`–`\count3` are $(17, 0 , 0, 7)$ and the others are
$0$, TeX displays the page number as \[17.0.0.7\].
]],
   pageshrink = [[
This parameter specifies the amount of shrink in the accumulated glue on
the current page. TeX updates `\pageshrink` as it adds items to the main
vertical list.
]],
   pagestretch = [[
`\pagefilllstretch`, `\pagefillstretch`, `\pagefilstretch`,
`\pagestretch`

These four parameters together specify the amount of stretch in the glue
on the current page. The amount of stretch has the form

$n_0 + n_1fil + n_2fill + n_3filll$

, with the four parameters giving the values of the four $n_i$.
TeX updates these parameters as it adds items to the main vertical list.
]],
   pagetotal = [[
This parameter specifies the accumulated natural height of the current
page. TeX updates `\pagetotal` as it adds items to the main vertical
list.
]],
   parfillskip = [[
This parameter specifies the horizontal glue that TeX inserts at the end
of a paragraph. The default value of `\parfillskip` is `0pt plus 1fil`,
which causes the last line of a paragraph to be filled out with blank
space. A value of `0pt` forces TeX to end the last line of a paragraph
at the right margin.
]],
   parindent = [[
This parameter specifies the amount by which the first line of each
paragraph is to be indented. As the example below shows, it’s a bad idea
to set both `\parindent` and `\parskip` to zero since then the paragraph
breaks are no longer apparent.
]],
   parshape = [[
This command specifies the shape of the first $n$ lines of a paragraph—
the next paragraph if you’re in vertical mode and the current paragraph
if you’re in horizontal mode. The $i$’s and $l$’s are all dimensions.
The first line is indented by $i_1$ and has length $l_1$, the second
line is indented by $i_2$ and has length $l_2$, and so forth. If the
paragraph has more than $n$ lines, the last indentation/ length pair is
used for the extra lines. To achieve special effects such as the one
shown here, you usually have to experiment a lot, insert kerns here and
there, and choose your words to fit the shape.

`\parshape`, like `\hangafter` and `\hangindent`, is effective only for
one paragraph. If you specify `\hangafter` and `\hangindent` as well as
`\parshape`, TeX ignores the `\hangafter` and `\hangindent`.
]],
   parskip = [[
This parameter specifies the “paragraph skip”, i.e., the vertical glue
that TeX inserts at the start of a paragraph.

See `\par` for more information about what happens when TeX starts a
paragraph. Plain TeX sets `\parskip` to `0pt plus 0.1pt`.
]],
   pausing = [[
If this parameter is greater than zero, TeX will pause at each line of
input to give you an opportunity to replace it with a different line. If
you type in a replacement, TeX will use that line instead of the
original one; if you respond with ⟨return⟩, TeX will use the original
line.

Setting `\pausing` to $1$ can be useful as a way of patching a document
as TeX is processing it. For example, you can use this facility to
insert `\show` commands (see below).
]],
   penalty = [[
This command produces a penalty item. The penalty item makes TeX more or
less willing to break a page at the point where that item occurs. A
negative penalty, i.e., a bonus, encourages a page break; a positive
penalty discourages a page break. A penalty of $10000$ or more prevents
a break altogether, while a penalty of $-10000$ or less forces a break.
`\penalty` can also be used in horizontal mode.
]],
   phantom = [[
This command produces an empty box having the same size and placement
that `argument` would have were it typeset. One use of `\phantom` is for
reserving space for a symbol that for some reason needs to be drawn in
by hand.
]],
   plainoutput = [[
This command invokes plain TeX’s output routine.

Plain TeX defines `\output` as a token list containing the single token
`\plainoutput`.
]],
   pmod = [[
This command provides a notation for indicating a modulus in parentheses
at the end of a formula.
]],
   postdisplaypenalty = [[
This parameter specifies the penalty for breaking a page just after a
math display. Plain TeX leaves `\postdisplaypenalty` at $0$.
]],
   predisplaypenalty = [[
This parameter specifies the penalty for breaking a page just before a
math display. Plain TeX sets `\predisplaypenalty` to $10000$.
]],
   predisplaysize = [[
TeX sets this parameter to the width of the line preceding a math
display. TeX uses `\predisplaysize` to determine whether or not the
display starts to the left of where the previous line ends, i.e.,
whether or not it visually overlaps the previous line. If there is
overlap, it uses the `\abovedisplayskip` and `\belowdisplayskip` glue in
setting the display; otherwise it uses the `\abovedisplayshortskip` and
`\belowdisplayshortskip` glue. See pages 188–189 of *The TeXbook* for a
more detailed explanation of this parameter.
]],
   pretolerance = [[
`\pretolerance`, `\tolerance`

These parameters determine the badness that TeX will tolerate on each
line when it is choosing line breaks for a paragraph. The badness is a
measure of how far the interword spacing deviates from the ideal.
`\pretolerance` specifies the tolerable badness for line breaks without
hyphenation; `\tolerance` specifies the tolerable badness for line
breaks with hyphenation. The tolerable badness can be exceeded in either
of two ways: a line is too tight (the interword spaces are too small) or
it is too loose (the interword spaces are too big).

-   If TeX must set a line too loosely, it complains about an “underfull
    hbox”.

-   If TeX must set a line too rightly, it lets the line run past the
    right margin and complains about an “overfull hbox”.

TeX chooses line breaks in the following steps:

1.  It attempts to choose line breaks without hyphenating. If none of
    the resulting lines have a badness exceeding `\pretolerance`, the
    line breaks are acceptable and the paragraph can now be set.

2.  Otherwise, it tries another set of line breaks, this time allowing
    hyphenation. If none of the resulting lines have a badness exceeding
    `\tolerance`, the new set of line breaks is acceptable and the
    paragraph can now be set.

3.  Otherwise, it adds `\emergencystretch` (see below) to the stretch of
    each line and tries again.

4.  If none of these attempts have produced an acceptable set of line
    breaks, it sets the paragraph with one or more overfull hboxes and
    complains about them.

Plain TeX sets `\tolerance` to $200$ and `\pretolerance` to $100$. If
you set `\tolerance` to $10000$, TeX becomes infinitely tolerant and
accepts any spacing, no matter how bad (unless it encounters a word that
won’t fit on a line, even with hyphenation). Thus by changing
`\tolerance` you can avoid overfull and underfull hboxes, but at the
cost of making the spacing worse. By making `\pretolerance` larger you
can get TeX to avoid hyphenation (and also run faster), again at the
cost of possibly worse spacing. If you set `\pretolerance` to $-1$,
TeX will not even try to set the paragraph without hyphenation.

The `\hbadness` parameter determines the level of badness that TeX will
tolerate before it complains, but `\hbadness` does not affect the way
that TeX typesets your document. The `\hfuzz` parameter determines the
amount that an hbox can exceed its specified width before TeX considers
it to be erroneous.
]],
   prevdepth = [[
When TeX adds a box to a vertical list, it sets `\prevdepth` to the
depth of that box. TeX sets `\prevdepth` to $-1000$pt at the start of a
vertical list, indicating that the usual interline glue should be
suppressed.
]],
   prevgraf = [[
In horizontal mode, this parameter specifies the number of lines in the
paragraph so far; in vertical mode, it specifies the number of lines in
the previous paragraph. TeX only sets `\prevgraf` after it has finished
breaking some text into lines, i.e., at a math display or at the end of
a paragraph. See page 103 of *The TeXbook* for more details about it.
]],
   proclaim = [[
This command “proclaims” a theorem, lemma, hypothesis, etc. It sets
`argument` in boldface type and the following paragraph in italics.
`argument` must be followed by a period and a space token, which serve
to set off `argument` from `general text`. `general text` consists of
the text up to the next paragraph boundary. You can include multiple
paragraphs by using `\endgraf` instead of a blank line or `\par`.
]],
   radical = [[
This command produces a radical sign whose characteristics are given by
`number`. It uses the same representation as the delimiter code

in the `\delcode` command.
]],
   raggedbottom = [[
`\normalbottom`, `\raggedbottom`

Normally TeX tries hard to ensure that all pages have the same depth,
i.e., that their bottom margins are equal. The `\raggedbottom` command
tells TeX to allow some variability among the bottom margins on
different pages. It’s often appropriate to use `\raggedbottom` when you
have material that contains large blocks of material that should not be
split across pages. The `\normalbottom` command cancels the effect of
`\raggedbottom`.
]],
   raggedright = [[
`\raggedright`, `\ttraggedright`

These commands cause TeX to typeset your document “ragged right”.
Interword spaces all have their natural size, i.e., they all have the
same width and don’t stretch or shrink. Consequently the right margin is
generally not even. The alternative, which is TeX’s default, is to
typeset your document justified,

i.e., with uniform left and right margins. In justified text, interword
spaces are stretched in order to make the right margin even. Some
typographers prefer ragged right because it avoids distracting “rivers”
of white space on the printed page.

You should use the `\ttraggedright` command when typesetting text in a
monospaced font and the `\raggedright` command when typesetting text in
any other font.

Most of the time you’ll want to apply these commands to an entire
document, but you can limit their effects by enclosing them in a group.
]],
   relax = [[
This command tells TeX to do nothing. It’s useful in a context where you
need to provide a command but there’s nothing that you want TeX to do.
]],
   relpenalty = [[
This parameter specifies the penalty for breaking a math formula after a
relation when the formula appears in a paragraph. Plain TeX sets
`\relpenalty` to $500$.
]],
   romannumeral = [[
This command produces the roman numeral representation of a number as a
sequence of character tokens. The number can be either an explicit
integer, a `number` parameter, or a `number` register. If the number is
zero or negative, `\romannumeral` produces no tokens.
]],
   root = [[
This command produces the notation for a root of `argument_2`, where the
root is given by `argument_1`.
]],
   sb = [[
`\sb`, `\sp`

The commands in each column are equivalent. The commands in the first
column typeset `argument` as a subscript, and those in the second column
typeset `argument` as a superscript. The `\sb` and `\sp` commands are
mainly useful if you’re working on a terminal that lacks an underscore
or caret, or if you’ve redefined ‘`_`’ or ‘`\verb`’ and need access to
the original definition. These commands are also used for setting lower
and upper limits on summations and integrals.

If a subscript or superscript is not a single token, you need to enclose
it in a group. TeX does not prioritize subscripts or superscripts, so it
will reject formulas such as `a_i_j`, `a^i^j`, or `a^i_j`.

Subscripts and superscripts are normally typeset in script style, or in
scriptscript style if they are second-order, e.g., a subscript on a
subscript or a superscript on a a subscript. You can set *any* text in a
math formula in a script or scriptscript style with the `\scriptstyle`
and `\scriptscriptstyle` commands.

You can apply a subscript or superscript to any of the commands that
produce named mathematical functions in roman type). In certain cases
(again, see ) the subscript or superscript appears directly above or
under the function name as shown in the examples of `\lim` and `\det`
below.
]],
   scriptspace = [[
This parameter specifies the amount of space that TeX inserts before and
after a subscript or superscript. The `\nonscript` command after a
subscript or superscript cancels this space. Plain TeX sets
`\scriptspace` to `0.5pt`.
]],
   scrollmode = [[
This command tells TeX not to stop for most errors, but to continue
displaying the error messages on your terminal. Typing ‘`S`’ or ‘`s`’ in
response to an error message puts you into scroll mode.
]],
   setbox = [[
`\box`, `\setbox`

These commands respectively set and retrieve the contents of the box
register whose number is `register`. Note that you set a box register a
little differently than you set the other kinds of registers: you use
`\setbox`$\,n$ `=` rather than `\box`$\,n$ `=`.

*Retrieving the contents of a box register with these commands has the
side effect of emptying it, so that the box register become void.* If
you don’t want that to happen, you can use `\copy` (see below) to
retrieve the contents. You should use `\box` in preference to `\copy`
when you don’t care about what’s in a box register after you’ve used it,
so as not to exhaust TeX’s memory by filling it with obsolete boxes.
]],
   setlanguage = [[
This command sets the current language to `number` by inserting the same
whatsit that you’d get by changing `\language`. However, it does not
change the value of `\language`.
]],
   settabs = [[
The first form of this command defines a set of tab stops for a tabbing
alignment. It tells TeX to set the tab stops so as to divide each line
into `number` equal parts. TeX takes the length of a line to be
`\hsize`, as usual. You can make the alignment narrower by decreasing
`\hsize`.
]],
   shipout = [[
This command instructs TeX to send `box` to the .dvi file.

TeX expands any `\write` command in `box` as part of `\shipout`.

The principal use of `\shipout` is in the output routine, but you can
use it anywhere.
]],
   show = [[
`\showbox`, `\showlists`, `\showthe`, `\show`

These commands record information in the log of your TeX run:

-   `\show` records the meaning of `token`.

-   `\showthe` records whatever tokens would be produced by `\the`
    `argument`).

-   `\showbox` records the contents of the box register numbered
    `number`. The number of leading dots in the log indicates the number
    of levels of nesting of inner boxes.

-   `\showlists` records the contents of each list that TeX is currently
    constructing. (These lists are nested one within another.) See
    pages 88–89 of *The TeXbook* for further information about
    interpreting the output of `\showlists`.

For `\show` and `\showthe`, TeX also displays the information at your
terminal. For `\showbox` and `\showlists`, TeX displays the information
at your terminal only if `\tracingonline` is greater than zero; if
`\tracingonline` is zero or less (the default case), the information is
not displayed.

Whenever TeX encounters a `\show`-type command it stops for interaction.
The request for interaction does *not* indicate an error, but it does
give you an opportunity to ask TeX to show you something else. If you
don’t want to see anything else, just press ⟨return⟩.

You can control the amount of output produced by `\showbox` by setting
`\showboxbreadth` and `\showboxdepth`.

These parameters respectively have default values of $5$ and $3$, which
is why just five items appear for each box described in the log output
below. (The ‘`..etc.`’ indicates additional items within the boxes that
aren’t displayed.)
]],
   showboxbreadth = [[
This parameter specifies the maximum number of list items that
TeX displays for one level of one box when it is producing the output
for `\showbox` or `\tracingoutput`. Plain TeX sets `\showboxbreadth` to
$5$.
]],
   showboxdepth = [[
This parameter specifies the level of the deepest list that TeX displays
when it is producing the output for `\showbox` or `\showlists`. Plain
TeX sets `\showboxdepth` is $3$.
]],
   skew = [[
This command shifts the accent `argument_1` by `number` mathematical
units to the right of its normal position with respect to `argument_2`.
The most common use of this command is for modifying the position of an
accent that’s over another accent.
]],
   skewchar = [[
The `\skewchar` of a font is the character in the font whose kerns, as
defined in the font’s metrics file, determine the positions of math
accents. That is, suppose that TeX is applying a math accent to the
character ‘`x`’. TeX checks if the character pair ‘`x\skewchar`’ has a
kern; if so, it moves the accent by the amount of that kern. The
complete algorithm that TeX uses to position math accents (which
involves many more things) is in Appendix G of *The TeXbook*.

If the value of `\skewchar` is not in the range $0$–$255$, TeX takes the
kern value to be zero.

Note that `font` is a control sequence that names a font, not a
`fontname` that names font files. Beware: an assignment to `\skewchar`
is *not* undone at the end of a group. If you want to change `\skewchar`
locally, you’ll need to save and restore its original value explicitly.
]],
   slash = [[
This command produces a solidus (/) and also tells TeX that it can break
the line after the solidus, if necessary.
]],
   smallbreak = [[
`\bigbreak`, `\medbreak`, `\smallbreak`

These commands indicate increasingly desirable places for TeX to break a
page. They also cause TeX to insert a `\smallskip`, `\medskip`, or
`\bigskip` if the page break doesn’t actually happen. TeX suppresses
this skip if it occurs just after an equal or larger skip.
]],
   smallskip = [[
`\bigskip`, `\medskip`, `\smallskip`

These commands produce successively larger amounts of vertical space:

`\smallskip` skips by $3$ points and can stretch or shrink by $1$ point.
`\medskip` is equivalent to two `\smallskip`s and `\bigskip` is
equivalent to two `\medskip`s.

These commands end a paragraph since they are inherently vertical. The
skips that they produce are in addition to the normal interparagraph
skip.
]],
   smallskipamount = [[
`\bigskipamount`, `\medskipamount`, `\smallskipamount`

These parameters specify the amounts of glue produced by the
`\smallskip`, `\medskip`, and `\bigskip` commands. By changing these
parameters you change the effect of the commands. The default values
(for plain TeX) correspond to a quarter of a linespace, half a
linespace, and a full linespace. We recommend that you maintain this
ratio by changing these values whenever you change `\baselineskip`.
]],
   smash = [[
This command typesets `argument`, but forces the height and depth of its
containing box to be zero. You can use `\smash` and `\vphantom` in
combination to give a subformula any height and depth that you wish.
]],
   space = [[
This command is equivalent to an input space character. It differs from
`\ ` in that its width *can* be affected by preceding punctuation.
]],
   spacefactor = [[
`\sfcode`, `\spacefactor`, `\spaceskip`, `\xspaceskip`

These primitive parameters affect how much space TeX puts between two
adjacent words, i.e., the interword spacing. The normal interword
spacing is supplied by the current font. As TeX is processing a
horizontal list, it keeps track of the space factor $f$ in
`\spacefactor`. As it processes each input character $c$, it updates $f$
according to the value of $f_c$, the space factor code of $c$ (see
below). For most characters, $f_c$ is $1000$ and TeX sets $f$ to $1000$.
(The initial value of $f$ is also $1000$.) When TeX sees an interword
space, it adjusts the size of that space by multiplying the stretch and
shrink of that space by $f/1000$ and $1000/f$ respectively. Thus:

1.  If $f=1000$, the interword space keeps its normal value.

2.  If $f<1000$, the interword space gets less stretch and more shrink.

3.  If $f>1000$, the interword space gets more stretch and less shrink.

In addition, if $f\ge2000$ the interword space is further increased by
the “extra space” parameter associated with the current font.

Each input character $c$ has an entry in the `\sfcode` (space factor
code) table. The `\sfcode` table entry is independent of the font.
Usually TeX just sets $f$ to $f_c$ after it processes $c$. However:

-   If $f_c$ is zero, TeX leaves $f$ unchanged. Thus a character such as
    ‘`)`’ in plain TeX, for which $f_c$ is zero, is essentially
    transparent to the interword space calculation.

-   If $f<1000<f_c$, TeX sets $f$ to $1000$ rather than to $f_c$, i.e.,
    it refuses to raise $f$ very rapidly.

The `\sfcode` value for a period is normally $3000$, which is why
TeX usually puts extra space after a period (see the rule above for the
case $f\ge2000$). Noncharacter items in a horizontal list, e.g.,
vertical rules, generally act like characters with a space factor of
$1000$.

You can change the space factor explicitly by assigning a different
numerical value to `\spacefactor`. You can also override the normal
interword spacing by assigning a different numerical value to
`\xspaceskip` or to `\spaceskip`:

-   `\xspaceskip` specifies the glue to be used when $f\ge2000$; in the
    case where `\xspaceskip` is zero, the normal rules apply.

-   `\spaceskip` specifies the glue to be used when $f<2000$ or when
    `\xspaceskip` is zero; if `\spaceskip` is zero, the normal rules
    apply. The stretch and shrink of the `\spaceskip` glue, like that of
    the ordinary interword glue, is modified according to the value of
    $f$.

See page 76 of *The TeXbook* for the precise rules that TeX uses in
calculating interword glue, and pages 285–287 of *The TeXbook* for the
adjustments made to `\spacefactor` after various items in a horizontal
list.
]],
   span = [[
The meaning of this command depends on whether it appears in a preamble
or in an alignment entry.

-   Normally, TeX does not expand tokens in the preamble when it reads
    them. Putting `\span` in front of a token in the preamble causes
    that token to be expanded immediately according to TeX’s usual rules
    of macro expansion.

-   Putting `\span` instead of ‘`&`’ between two column or row entries
    causes those columns or rows to be combined. For a horizontal
    alignment, the width of the combined column is the sum of the widths
    of the component columns. For a vertical alignment, the height of
    the combined row is the sum of the heights of the component rows.
    The template of the combined column or combined row forms a single
    group, so font-setting commands preceding a `\span` affect
    everything up to the next ‘`&`’.

`\span` is rarely useful by itself outside of a template, but it
provides the basic mechanism for defining `\multispan`.
]],
   special = [[
This command tells TeX to write `token list` directly to the .dvi
file when it next ships out a page. A typical use of `\special` would be
to tell the device driver to incorporate the contents of a named
graphics file into the output page.

The `\special` command produces a whatsit that associates `token list`
with a particular position on the page, namely, the position that a
zero-size box would have had if such a box had appeared instead of the
`\special` command. Any use you might make of `\special` depends
strictly on the device drivers that you have available.
]],
   splitfirstmark = [[
`\splitbotmark`, `\splitfirstmark`

These commands expand to the mark text generated by an earlier `\mark`
command that produced an item in the item list of a vbox $V$. The mark
text has the form of a token list. When TeX splits $V$ in response to a
`\vsplit` command, it sets the values of these commands as follows:

-   `\splitfirstmark` contains the tokens of the first mark in the item
    list of $V$.

-   `\splitbotmark` contains the tokens of the last mark in the item
    list of $V$.

These commands produce no tokens if there was no preceding `\vsplit`, or
if the most recent preceding `\vsplit` didn’t contain any marks.
]],
   splitmaxdepth = [[
This parameter specifies the maximum allowable depth of a box resulting
from a `\vsplit`. `\splitmaxdepth` plays the same role that `\maxdepth`
plays for a page.
]],
   splittopskip = [[
This parameter specifies the glue that TeX inserts at the top of a box
resulting from a `\vsplit`. `\splittopskip` plays the same role that
`\topskip` plays for a page.
]],
   sqrt = [[
This command produces the notation for the square root of `argument`.
]],
   string = [[
This command produces the characters that form the name of
`control sequence`, including the escape character. The escape character
is represented by the current value of `\escapechar`.

TeX gives the characters in the list a category code of $12$ (other).

You can perform the reverse operation with the `\csname` command, which
turns a string into a control sequence.
]],
   strut = [[
This command produces a box whose width is zero and whose height
(`8.5pt`) and depth (`3.5pt`) are those of a more or less typical line
of type in `cmr10`, the default plain TeX font. Its main use is in
forcing lines to have the same height when you’ve disabled TeX’s
interline glue with `\offinterlineskip` or a similar command, e.g., when
you’re constructing an alignment. If the natural height of a line is too
short, you can bring it up to standard by including a `\strut` in the
line. The strut will force the height and depth of the line to be
larger, but it won’t print anything or consume any horizontal space.

If you’re setting type in a font that’s bigger or smaller than `cmr10`,
you should redefine `\strut` for that context.
]],
   tabskip = [[
This parameter specifies the amount of horizontal or vertical glue that
TeX puts between the columns of a horizontal alignment or between the
rows of a vertical alignment. TeX also puts the `\tabskip` glue to the
left of the first column and to the right of the last column of a
horizontal alignment, and above the first row and below the last row of
a vertical alignment. You can change `\tabskip` within a template— the
change will affect the glue associated with all the following `&`’s as
well as the glue after the last row or column.
]],
   textfont = [[
`\scriptfont`, `\scriptscriptfont`, `\textfont`

Each of these parameters specifies the font that TeX is to use for
typesetting the indicated style in the indicated family. These choices
have no effect outside of math mode.
]],
   textindent = [[
This command tells TeX to start a paragraph and indent it by
`\parindent`, as usual. TeX then right-justifies `argument` within the
indentation and follows it with an en space (half an em). Plain TeX uses
this command to typeset footnotes

and items in lists).
]],
   textstyle = [[
`\displaystyle`, `\scriptscriptstyle`, `\scriptstyle`, `\textstyle`

These commands override the normal style and hence the font that
TeX uses in setting a formula. Like font-setting commands such as `\it`,
they are in effect until the end of the group containing them. They are
useful when TeX’s choice of style is inappropriate for the formula you
happen to be setting.
]],
   the = [[
This command generally expands to a list of character tokens that
represents `token`. `token` can be any of the following:

-   a TeX parameter, e.g., `\parindent` or `\deadcycles`

-   a register, e.g., `\count0`

-   a code associated with an input character, e.g., `` \catcode`( ``

-   a font parameter, e.g., `\fontdimen3\sevenbf`

-   the `\hyphenchar` or `\skewchar` of a font, e.g., `\skewchar\teni`

-   `\lastpenalty`, `\lastskip`, or `\lastkern` (values derived from the
    last item on the current horizontal or vertical list)

-   a control sequence defined by `\chardef` or `\mathchardef`

In addition, `\the` can expand to noncharacter tokens in the following
two cases:

-   `\the` `font`, which expands to the most recently defined control
    sequence that selects the same font as the control sequence `font`

-   `\the` `token variable`, which expands to a copy of the value of the
    variable, e.g., `\the\everypar`

See pages 214–215 of *The TeXbook* for a more detailed description of
what `\the` does in various cases.
]],
   thinmuskip = [[
`\medmuskip`, `\thickmuskip`, `\thinmuskip`

These parameters define thin, medium, and thick spaces in math mode.
]],
   thinspace = [[
This command produces a positive kern whose width is one-sixth of an em

i.e., it causes TeX to move its position right by that amount. It is
useful when you have a nested quotation, for example, and you want to
separate the levels of quotes. TeX won’t break a line at a ``.
]],
   time = [[
TeX sets this parameter to the number of minutes that have elapsed since
midnight (of the current day). At noon, for instance, `\time` is $720$.
This command and the next three make use of the time and date as
recorded in your computer. TeX retrieves them just once, at the
beginning of your run, so `\time` at the end of the run always has the
same value as `\time` at the beginning of the run (unless you’ve
explicitly changed it).
]],
   topglue = [[
This command[1] causes the space from the top of the page to the top of
the first box on the page to be `glue` precisely. The top of the page is
considered to be at the baseline of an imaginary line of text just above
the top line of the page. More precisely, it’s a distance `\topskip`
above the origin as given by `\hoffset` and `\voffset`.

This command is useful because TeX ordinarily adjusts the glue produced
by `\topskip` in a complex way. By using `\topglue` you can control the
position of the first box on the page without worrying about those
adjustments.

[1] `\topglue` was added to TeX in version 3.0, later than the other
enhancements introduced by new TeX . It is first described in the
*eighteenth* edition of *The TeXbook*.
]],
   topinsert = [[
`\midinsert`, `\pageinsert`, `\topinsert`

These commands produce different forms of insertions that instruct (or
allow) TeX to relocate the `vertical mode material`:

-   `\topinsert` attempts to put the material at the top of the current
    page. If it won’t fit there, `\topinsert` will move the material to
    the next available top of page.

-   `\midinsert` attempts to put the material at the current position.
    If it won’t fit there, `\midinsert` will move the material to the
    next available top of page.

-   `\pageinsert` puts the material by itself on the next page. To avoid
    an underfull page, be sure to end the inserted material with `\vfil`
    or fill out the excess space some other way.

The `vertical mode material` is said to be “floating” because TeX can
move it from one place to another. Insertions are very useful for
material such as figures and tables because you can position such
material where you want it without knowing where the page breaks will
fall.

Each of these commands implicitly ends the current paragraph, so you
should use them only between paragraphs. You should not use them within
a box or within another insertion. If you have several insertions
competing for the same space, TeX will retain their relative order.
]],
   topskip = [[
TeX inserts glue at the top of each page in order to ensure that the
baseline of the first box on the page always is the same distance $d$
from the top of the page. `\topskip` determines the amount of that glue,
called the “`\topskip` glue”, by specifying what $d$ should be (provided
that the first box on the page isn’t too tall). $d$ is given by the
natural size of the `\topskip` glue. If the height of the first box on
the page exceeds $d$, so that the glue would be negative, TeX simply
inserts no `\topskip` glue at all on that page.

To understand better the effect of these rules, assume that `\topskip`
has no stretch or shrink and that the first item on the page is indeed a
box. Then if the height of that box is no greater than `\topskip`, its
baseline will be `\topskip` from the top of the page independently of
its height. On the other hand, if the height of the box is $e$ greater
than `\topskip`, its baseline will be `\topskip` + $e$ from the top of
the page. See pages 113–114 of *The TeXbook* for the remaining details
of how `\topskip` works. Plain TeX sets `\topskip` to `10pt`.
]],
   tracingall = [[
This command tells TeX to turn on every available form of tracing. It
also sets `\tracingonline` to $1$ so that the trace output will appear
at your terminal.
]],
   tracingcommands = [[
If this parameter is $1$ or greater, TeX will record in the log file
most commands that it executes. If `\tracingonline` is greater than
zero, this information will also appear at your terminal. Typesetting
the first character of a word counts as a command, but (for the purposes
of the trace only) the actions of typesetting the subsequent characters
and any punctuation following them do not count as commands. If
`\tracingcommands` is $2$ or greater, TeX will also record commands that
are expanded rather than executed, e.g., conditional tests and their
outcomes.
]],
   tracinglostchars = [[
If this parameter is greater than zero, TeX will record an indication in
the log file of each time that it drops an output character because that
character does not exist in the current font. If `\tracingonline` is
greater than zero, this information will also appear at your terminal.
Plain TeX defaults it to $1$ (unlike the others).
]],
   tracingmacros = [[
If this parameter is $1$ or greater, TeX will record in the log file the
expansion and arguments of every macro that it executes.

If `\tracingmacros` is $2$ or greater, TeX will record, in addition,
every expansion of a token list such as `\output` or `\everycr`. If
`\tracingonline` is greater than zero, this information will also appear
at your terminal.
]],
   tracingonline = [[
If this parameter is greater than zero, TeX will display the results of
tracing (including `\showbox` and `\showlists`) at your terminal in
addition to recording them in the log file.
]],
   tracingoutput = [[
If this parameter is greater than zero, TeX will record in the log file
the contents of every box that it sends to the .dvi file.

If `\tracingonline` is greater than zero, this information will also
appear at your terminal. The number of leading dots in each line of the
trace output indicates the nesting level of the box at that line. You
can control the amount of tracing by setting `\showboxbreadth` and
`\showboxdepth`.

Setting `\tracingoutput` to $1$ can be particularly helpful when you’re
trying to determine why you’ve gotten extra space on a page.
]],
   tracingpages = [[
If this parameter is greater than zero, TeX will record in the log file
its calculations of the cost of various page breaks that it tries.

If `\tracingonline` is greater than zero, this information will also
appear at your terminal. TeX produces a line of this output whenever it
first places a box or insertion on the current page list, and also
whenever it processes a potential break point for the page. Examining
this output can be helpful when you’re trying to determine the cause of
a bad page break. See pages 112–114 of *The TeXbook* for an illustration
and explanation of this output.

Some production forms of TeX ignore the value of `\tracingpages` so that
they can run faster. If you need to use this parameter, be sure to use a
form that responds to it.
]],
   tracingparagraphs = [[
If this parameter is greater than zero, TeX will record in the log file
its calculations of the cost of various line breaks that it tries.

If `\tracingonline` is greater than zero, this information will also
appear at your terminal. TeX produces this output when it reaches the
end of each paragraph. See pages 98–99 of *The TeXbook* for an
illustration and explanation of this output.

Some production forms of TeX ignore the value of `\tracingparagraphs` so
that they can run faster. If you need to use this parameter, be sure to
use a form that responds to it.
]],
   tracingrestores = [[
If this parameter is greater than zero, TeX will record in the log file
the values that it restores when it encounters the end of a group. If
`\tracingonline` is greater than zero, this information will also appear
at your terminal.

Some production forms of TeX ignore the value of `\tracingrestores` so
that they can run faster. If you need to use this parameter, be sure to
use a form that responds to it.
]],
   tracingstats = [[
If this parameter is $1$ or greater, TeX will include a report on the
resources that it used to run your job (see page 300 of *The TeXbook*
for a list and explanation of these resources). Moreover, if
`\tracingstats` is $2$ or greater, TeX will report on its memory usage
whenever it does a `\shipout` for a page. The report appears at the end
of the log file.

If `\tracingonline` is greater than zero, the information will also
appear at your terminal. If you’re having trouble with TeX exceeding one
of its capacities, the information provided by `\tracingstats` may help
you pinpoint the cause of the difficulty.

Some production forms of TeX ignore the value of `\tracingstats` so that
they can run faster. If you need to use this parameter, be sure to use a
form that responds to it.

The following example shows a sample of the tracing output you’d get on
one implementation of TeX. It may be different on other implementations.
]],
   uchyph = [[
A positive value of `\uchyph` (uppercase hyphenation) permits
hyphenation of words, such as proper names, that start with a capital
letter. A zero or negative value inhibits such hyphenation. Plain
TeX sets `\uchyph` to $1$, so TeX normally tries to hyphenate words that
start with a capital letter.
]],
   underbar = [[
This command puts `argument` into an hbox and underlines it without
regard to anything that protrudes below the baseline of the box.
]],
   underbrace = [[
`\overbrace`, `\overleftarrow`, `\overline`, `\overrightarrow`,
`\underbrace`, `\underline`

These commands place extensible braces, lines, or arrows over or under
the subformula given by `argument`. TeX will make these constructs as
wide as they need to be for the context. When TeX produces the extended
braces, lines, or arrows, it considers only the dimensions of the box
containing `argument`. If you use more than one of these commands in a
single formula, the braces, lines, or arrows they produce may not line
up properly with each other. You can use the `\mathstrut` command to
overcome this difficulty.
]],
   unhbox = [[
`\unhbox`, `\unvbox`

These commands produce the list contained in box register `register` and
make that box register void. `\unhbox` applies to box registers
containing hboxes and `\unvbox` applies to box registers containing
vboxes. You should use these commands in preference to `\unhcopy` and
`\unvcopy` (see below) when you don’t care about what’s in the box
register after you’ve used it, so as not to exhaust TeX’s memory by
filling it with obsolete boxes.
]],
   unhcopy = [[
`\unhcopy`, `\unvcopy`

These commands produce the list contained in box register `register` and
leave the contents of the register undisturbed. `\unhcopy` applies to
box registers containing hboxes and `\unvcopy` applies to box registers
containing vboxes.
]],
   unkern = [[
`\unkern`, `\unpenalty`, `\unskip`

If the last item on the current list is of type kern, glue, or penalty
respectively, these commands remove it from that list. If the item isn’t
of the right type, these commands have no effect. Like `\lastbox`, you
can’t apply them to lists in math mode or to the main vertical list.
These commands are most useful after a macro call that is known to have
inserted a specific item that you don’t want there. TeX doesn’t provide
an `\unbox` command because `\lastbox` produces nearly the same effect.
]],
   vadjust = [[
This command inserts the specified `vertical mode material` just after
the output line containing the position where the command occurs.

You can use it, for instance, to cause a page eject or to insert extra
space after a certain line.
]],
   valign = [[
This command produces a vertical alignment consisting of a sequence of
columns, where each column in turn contains a sequence of row entries.
TeX adjusts the heights of the row entries to accommodate the tallest
one in each row.

A vertical alignment can only appear when TeX is in a horizontal mode.
Because vertical alignments are (a) conceptually somewhat difficult and
(b) not often used, we recommend that you learn about alignments in
general and the `\halign` command (see above) before you attempt to use
the `\valign` command.

An alignment consists of a preamble followed by the text to be aligned.
The preamble, which describes the layout of the columns that follow,
consists of a sequence of row templates, separated by ‘`&`’ and ended by
`\cr`. Each column consists of a sequence of row entries, also separated
by ‘`&`’ and ended by `\cr`. Within a template, ‘`#`’ indicates where
TeX should insert the corresponding text of a row entry.

TeX typesets each row entry in internal vertical mode, i.e., as the
contents of a vbox, and implicitly encloses the entry in a group. It
always gives the vbox zero depth. Any text or other horizontal mode
material in a row entry then puts TeX into ordinary horizontal mode.
(This is just an application of the general rules for TeX’s behavior in
internal vertical mode.) The usual paragraphing parameters apply in this
case: the row entry has an initial indentation of `\parindent` and its
lines have the `\leftskip` and `\rightskip` glue appended to them.

Note in particular that a row entry containing text has a width of
`\hsize`. Unless you reset `\hsize` to the row width that you want,
you’re likely to encounter overfull hboxes, or find that the first
column takes up the width of the entire page, or both.

Normally, you need to include a strut

in each template so that the rows don’t come out crooked as a result of
the varying heights of the entries in the alignment. You can produce a
strut with the `\strut` command.

The `to` form of this command instructs TeX to make the vertical extent
of the alignment be `dimen`, adjusting the space between rows as
necessary. The `spread` form of this command instructs TeX to make the
alignment taller by `dimen` than its natural height. These forms are
like the corresponding forms of `\vbox` .
]],
   vcenter = [[
Every math formula has an invisible “axis” that TeX treats as a kind of
horizontal centering line for that formula. For instance, the axis of a
formula consisting of a fraction is at the center of the fraction bar.
The `\vcenter` command tells TeX to place the `vertical mode material`
in a vbox and to center the vbox with respect to the axis of the formula
it is currently constructing.

The first form of the command centers the material as given. The second
and third forms expand or shrink the material vertically as in the
`\vbox` command .
]],
   vdots = [[
This command produces three vertical dots.
]],
   vee = [[
`\amalg` (∐), `\ast`, `\bigcirc` (○), `\bigtriangledown` (▽),
`\bigtriangleup` (△), `\bullet` (•), `\cap` (∩), `\cdot` (⋅), `\circ`
(∘), `\cup` (∪), `\dagger` (†), `\ddagger` (‡), `\diamond` (⋄), `\div`
(÷), `\land` (∧), `\lor` (∨), `\mp` (∓), `\odot` (⊙), `\ominus` (⊖),
`\oplus` (⊕), `\oslash` (⊘), `\otimes` (⊗), `\pm` (±), `\setminus`,
`\sqcap` (⊓), `\sqcup` (⊔), `\star` (⋆), `\times` (×), `\triangleleft`
(⊲), `\triangleright` (⊳), `\uplus` (⊎), `\vee` (∨), `\wedge` (∧), `\wr`
(≀)

These commands produce the symbols for various binary operations. Binary
operations are one of TeX’s classes of math symbols. TeX puts different
amounts of space around different classes of math symbols. When
TeX needs to break a line of text within a math formula, it will
consider placing the break after a binary operation—but only if the
operation is at the outermost level of the formula, i.e., not enclosed
in a group.

In addition to these commands, TeX also treats ‘`+`’ and ‘`-`’ as binary
operations. It considers ‘`/`’ to be an ordinary symbol, despite the
fact that mathematically it is a binary operation, because it looks
better with less space around it.
]],
   vsize = [[
This parameter specifies the current vertical extent of a page.
TeX examines it only when it is starting a page. Thus if you change
`\vsize` in the middle of a page, your change won’t affect anything
until the following page. If you want to change the vertical extent of a
page when you’re in the middle of it, you should assign the new height
to `\pagegoal` instead. (If you want the change to affect the following
pages too, you should change *both* `\vsize` and `\pagegoal`.) Plain
TeX sets `\vsize` to `8.9in`.
]],
   vsplit = [[
This command causes TeX to split the box numbered `number`, which we’ll
call $B_2$, into two parts. It uses the same algorithm that it would use
if $B_2$ was a page and it was breaking that page; the division point
then corresponds to the page break that it would find. The box $B_2$
must be a vbox, not an hbox. TeX puts the material preceding the
division point into another box $B_1$ and leaves the material after the
division point in $B_2$. The `\vsplit` command then produces $B_1$.
Normally you’d assign $B_1$ to a different box register, as in the
example below. If the division point is at the end of $B_2$, $B_2$ will
be empty after the `\vsplit`.

TeX employs its usual page-breaking algorithm

for the split. It uses `dimen` for `\pagegoal`, the desired height of
$B_1$. The vertical extent of $B_1$ may not be exactly `dimen` because
TeX may not be able to achieve its page goal perfectly. TeX does not
consider insertions in calculating the split, so insertions in the
original vertical list of $B_2$ will be retained but won’t affect the
split point.
]],
   vtop = [[
`\vbox`, `\vtop`

These commands produce a vbox (vertical box) containing
`vertical mode material`. The braces around `vertical mode material`
define a group. TeX is in internal vertical mode when it’s assembling
the box. TeX won’t change the size of the box once it’s been produced.

The difference between `\vtop` and `\vbox` lies in where TeX puts the
reference point of the constructed vbox. Ordinarily, the reference point
gotten from `\vtop` tends to be at or near the top of the constructed
vbox, while the reference point gotten from `\vbox` tends to be at or
near the bottom of the constructed vbox. Thus a row of vboxes all
constructed with `\vtop` will tend to have their tops nearly in a line,
while a row of vboxes all constructed with `\vbox` will tend to have
their bottoms nearly in a line.

`\vtop` and `\vbox` are often useful when you want to keep some text
together on a single page. (For this purpose, it usually doesn’t matter
which command you use.) If your use of these commands prevents TeX from
breaking pages in an acceptable way, TeX will complain that it’s found
an overfull or underfull vbox while `\output` is active.

The height of a vbox depends on the arguments to `\vtop` or `\vbox`. For
`\vbox`, TeX determines the height as follows:

-   If you specify only `vertical mode material`, the vbox will have its
    natural height.

-   If you specify `to` `dimen`, the height of the vbox will be `dimen`.

-   If you specify `spread` `dimen`, the height of the vbox will be its
    natural height plus `dimen`, i.e., the height of the vbox will be
    stretched vertically by `dimen`.

For `\vtop`, TeX constructs the box using its rules for `\vbox` and then
apportions the vertical extent between the height and the depth as
described below.

Ordinarily, the width of a constructed vbox is the width of the widest
item inside it.[1] The rules for apportioning the vertical extent
between the height and the depth are more complicated:

-   For `\vtop`, the height is the height of its first item, if that
    item is a box or rule. Otherwise the height is zero. The depth is
    whatever vertical extent remains after the height is subtracted.

-   For `\vbox`, the depth is the depth of its last item, if that item
    is a box or rule. Otherwise the depth is zero. The height is
    whatever vertical extent remains after the depth is subtracted. [2]

The `\vfil` command is useful for filling out a vbox

with empty space when the material in the box isn’t as tall as the
vertical extent of the box.

[1] More precisely, it’s the distance from the reference point to the
rightmost edge of the constructed vbox. Therefore, if you move any of
the items right using `\moveright` or `\moveleft` (with a negative
distance), the constructed vbox might be wider.

[2] In fact, there’s a further complication. Suppose that after the
depth has been determined using the two rules just given, the depth
turns out to be greater than `\boxmaxdepth`. Then the depth is reduced
to `\boxmaxdepth` and the height is adjusted accordingly.
]],
   widowpenalty = [[
This parameter specifies the penalty for breaking a page just before the
last line of a paragraph. A line by itself at the top of a page is
called a “widow line”. Plain TeX sets `\widowpenalty` to $150$.
]],
   wlog = [[
This command writes `token list` on the log file.

TeX expands `token list` according to the same rules that it uses for
`\edef`.
]],
   write = [[
This command tells TeX to write `token list` to the file associated with
the output stream designated by `number`. It generates a whatsit that
becomes part of a box. The actual writing does not take place until
TeX ships out that box to the .dvi file, unless you’ve preceded the
`\write` with `\immediate`.

For a `\write` that is not immediate, TeX does not expand macros in
`token list` until the token list is actually written to the file. The
macro expansions follow the same rules as `\edef`. In particular, any
control sequence that is not the name of a macro is written as
`\escapechar` followed by the control sequence name and a space. Any
‘`#`’ tokens in `token list` are doubled, i.e., written as ‘`##`’.

If `number` is not in the range from $0$ to $15$, TeX writes
`token list` to the log file.

If `number` is greater than $15$ or isn’t associated with an output
stream, TeX also writes `token list` to the terminal.
]],
   xdef = [[
This command is equivalent to `\global\edef`.
]],
   year = [[
TeX sets this parameter to the current year (A.D.). It is a number such
as $1991$. `\year` is set at the beginning of your run (see the comments
on `\time` above).
]]
}
