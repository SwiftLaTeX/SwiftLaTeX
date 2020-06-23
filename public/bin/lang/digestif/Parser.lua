--- Parser class
-- @classmod digestif.Parser

local lpeg = require "lpeg"
local util = require "digestif.util"

local C = lpeg.C
local Cc = lpeg.Cc
local Cg = lpeg.Cg
local Cs = lpeg.Cs
local Ct = lpeg.Ct
local P = lpeg.P
local R = lpeg.R
local S = lpeg.S
local V = lpeg.V
local match = lpeg.match

local I = lpeg.Cp()
local Ipos = Cg(I, "pos")
local Icont = Cg(I, "cont")
local Iouter_pos = Cg(I, "outer_pos")
local Iouter_cont = Cg(I, "outer_cont")
local Kcs = Cc("cs")
local Kmathshift = Cc("mathshift")
local Knil = Cc(nil)
local Kpar = Cc("par")
local Pend = P(-1)
local is_incomplete = Cg(Cc(true), "incomplete")
local is_omitted = Cg(Cc(true), "omitted")

local choice = util.choice
local gobble_until = util.gobble_until
local many = util.many
local matcher = util.matcher
local search = util.search
local sequence = util.sequence
local trim = util.trim

local weak_keys = {__mode = "k"}

local default_catcodes = {
  escape = P("\\"),
  bgroup = P("{"),
  egroup = P("}"),
  mathshift = P("$"),
  eol = P("\n"),
  letter = R("az", "AZ"),
  comment = P("%"),
  char = P(1), -- are we utf-8 agnostic?
  space = S(" \t"),
  listsep = P(","),
  valsep = P("=")
}

local Parser = util.class()

function Parser:__init(catcodes)
  catcodes = catcodes or default_catcodes

  -- Single characters
  local escape    = P(catcodes.escape    or default_catcodes.escape)
  local bgroup    = P(catcodes.bgroup    or default_catcodes.bgroup)
  local egroup    = P(catcodes.egroup    or default_catcodes.egroup)
  local mathshift = P(catcodes.mathshift or default_catcodes.mathshift)
  local eol       = P(catcodes.eol       or default_catcodes.eol)
  local letter    = P(catcodes.letter    or default_catcodes.letter)
  local comment   = P(catcodes.comment   or default_catcodes.comment)
  local char      = P(catcodes.char      or default_catcodes.char)
  local space     = P(catcodes.space     or default_catcodes.space)
  local listsep   = P(catcodes.listsep   or default_catcodes.listsep)
  local valsep    = P(catcodes.valsep    or default_catcodes.valsep)

  -- Basic elements
  local blank = space + eol
  local cs = escape * (letter^1 + char)
  local csname = escape * C(letter^1 + char)
  local token = cs + char
  local blank_line = eol * space^0 * eol
  local single_eol = eol * space^0 * -eol
  local comment_line = comment * gobble_until(eol, char)
  local par = blank_line * (space + eol + comment_line)^0
  local skipped = (space + single_eol + comment_line)^0

  -- These patterns match text delimited by braces.  They succeed on a
  -- incomplete subject string (with the additional incomplete = true
  -- named capture).  The short variant does not cross paragraph
  -- boundaries.
  local group = sequence(
    Iouter_pos,
    P{sequence(bgroup,
               Ipos,
               many(choice(comment_line,
                           V(1)/0,
                           token - egroup)),
               Icont,
               egroup + is_incomplete)},
    Iouter_cont)
  local short_group = sequence(
    Iouter_pos,
    P{sequence(bgroup,
               Ipos,
               many(choice(comment_line,
                           V(1)/0,
                           token - egroup)
                      - blank_line),
               Icont,
               egroup + is_incomplete)},
    Iouter_cont)

  -- One thing to which the scanner reacts.
  local thing = choice(
    Kcs * csname,
    mathshift * Kmathshift * (mathshift * Cc"$$" + Cc"$"),
    par * Kpar * Knil)

  -- Patterns that search ahead
  local next_thing = search(I * thing * I, comment_line + char)
  local next_par = search(blank_line, comment + char)
    * (eol + space + comment_line)^0 * I * char -- need this char at the end?

  -- Trimming, cleaning, and cropping
  local trimmer = blank^0 * C((blank^0 * (char - blank)^1)^0)
  local cleaner = blank^0 * Cs(((blank^1 / " " + true) * (char - blank)^1)^0)
  local comment_block = ((eol * space^0)^-1 * comment_line)^1 -- use the one below?
  -- local comment_block = (comment_line * (Pend + eol * space^0))^0
  local comment_stripper = Cs((comment_block / "" + char)^0)

  -- Parsing lists
  local skim_unit = comment_line + group/0 + token
  local skipped_long = (space + eol + comment_line)^0
  local listsep_skip = listsep * skipped_long

  local list_item = Ct(Ipos * (skim_unit - listsep)^1 * Icont)
  local list_parser = skipped_long * Ct((listsep_skip^0 * list_item)^0)

  local list_reader = skipped_long * Ct(
    (listsep_skip^0 * C((skim_unit - listsep)^1)
       / trim(space + eol + comment_line, char))^0)

  local key = Ct(Ipos * (skim_unit - listsep - valsep)^1 * Icont)
  local value = Ct(valsep * skipped_long * Ipos * (skim_unit - listsep)^0 * Icont)
  local kvlist_item = Ct(Ipos * Cg(key, "key") * Cg(value, "value")^-1 * Icont)
  local kvlist_parser = skipped_long * Ct((listsep_skip^0 * kvlist_item)^0)

  local patt_from_arg = function(arg)
    local ret = skipped
    if arg.delimiters then
      local l, r = arg.delimiters[1], arg.delimiters[2]
      ret = sequence(
        ret, Iouter_pos, l, Ipos,
        many(
          choice(comment_line,
                 short_group/0,
                 token)
            - blank_line - r),
        Icont,
        r + is_incomplete,
        Iouter_cont)
    elseif arg.literal then
      ret = ret * Ipos * P(arg.literal) * Icont
    else -- plain argument
      ret = ret * (short_group + Ipos * token * Icont)
    end
    if arg.optional then
      ret = ret + is_omitted
    end
    return Ct(ret)
  end

  local patt_from_arglist = function(args)
    local ret = Icont
    for i = #args, 1, -1 do
      ret = (patt_from_arg(args[i]) + is_incomplete) * ret
    end
    return Ct(Ipos * ret)
  end

  local patt_store = setmetatable({}, weak_keys)

  self.parse_args = function(arglist, str, pos)
    local patt = patt_store[arglist]
    if not patt then
      patt = patt_from_arglist(arglist)
      patt_store[arglist] = patt
    end
    return match(patt, str, pos)
  end

  -- public patterns
  self.group = group
  self.short_group = short_group
  self.next_par = next_par
  self.comment_line = comment_line
  self.cs = cs
  self.csname = csname
  self.skipped = skipped
  self.blank_line = blank_line
  self.next_thing = next_thing

  function self.cs_matcher(prefix)
    local patt = P(prefix) * letter^0 * Pend
    return matcher(patt)
  end

  -- public functions from patterns
  self.is_blank_line = matcher(space^0 * eol)
  self.next_nonblank = matcher(skipped * I)
  self.trim = matcher(trimmer) -- replace def of trimmer directly in here?
  self.clean = matcher(cleaner)
  self.strip_comments = matcher(comment_stripper)
  self.skip_to_bracketed = matcher( -- for tikz paths
    search(
      patt_from_arg{delimiters = {"[", "]"}},
      skim_unit - blank_line)) -- also exclude ";"?

  --- Parse a list.
  -- @function Parser.parse_list
  self.parse_list = matcher(list_parser)
  self.read_list = matcher(list_reader)
  self.parse_kvlist = matcher(kvlist_parser)

end

return Parser
