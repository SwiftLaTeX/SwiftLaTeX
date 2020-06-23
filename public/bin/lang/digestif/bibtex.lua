local lpeg = require "lpeg"
local util = require "digestif.util"

local B, P, R, S, V
   = lpeg.B, lpeg.P, lpeg.R, lpeg.S, lpeg.V
local C, Cc, Cp, Ct, Cmt, Cg
   = lpeg.C, lpeg.Cc, lpeg.Cp, lpeg.Ct, lpeg.Cmt, lpeg.Cg
local concat = table.concat
local merge = util.merge
local search, gobble_until, case_fold = util.search, util.gobble_until, util.case_fold
local split, replace = util.split, util.replace

local bibtex = {}

local function ipairs_from(t, i)
  return ipairs(t), t, i - 1
end

-- Parser
-- ======

local char = P(1)
local at_sign = P"@"
local newline = P"\n"
local space = S" \r\n\t"
local whitespace = space^0
local comment = P"%" * (1 - newline)^0 * newline
local junk = (comment + 1 - at_sign)^0
local number = C(R"09"^1) * whitespace
local name = C((R"az" + R"AZ" + R"09" + S"!$&*+-./:;<>?[]^_`|")^1) * whitespace
local lbrace = P"{" * whitespace
local rbrace = P"}" * whitespace
local lparen = P"(" * whitespace
local rparen = P")" * whitespace
local equals = P"=" * whitespace
local hash = P"#" * whitespace
local comma = P"," * whitespace
local quote = P'"' * whitespace
local lit_string = C(case_fold "string") * whitespace
local lit_comment = C(case_fold "comment") * whitespace
local lit_preamble = C(case_fold "preamble") * whitespace

local Cstart = Cg(Cp(), "start")
local Cstop = Cg(Cp(), "stop")

local curly_braced_string = util.between_balanced("{", "}")
local round_braced_string = util.between_balanced("(", ")")
local braced_string = (curly_braced_string + round_braced_string) * whitespace
local quoted_string = '"' * C(gobble_until('"')) * '"' * whitespace
local simple_value = quoted_string + braced_string + number + Ct(name)
local value = simple_value * (hash * simple_value)^0
local field = Ct(name * equals * value) + whitespace
local fields = field * (comma * field)^0

local token = curly_braced_string/0 + char
local nonspace = token - space
local author_sep = space * "and" * space
local etal_marker = P("et al") * whitespace * P(-1)

-- either curly or round braced
local function braced(patt)
  return lbrace * patt * rbrace + lparen * patt * rparen
end

local string_entry = at_sign * lit_string * braced(fields)
local comment_entry = at_sign * lit_comment * braced_string
local preamble_entry = at_sign * lit_preamble * braced(value)
local regular_entry = at_sign * name * braced(name * comma * fields)
local entry = string_entry + comment_entry + preamble_entry + regular_entry

-- this pattern produces the parse tree
-- TODO: catch premature end on invalid entry
local all_entries = Ct((junk * Ct(Cstart * entry * Cstop))^0)

-- Translate parse tree
-- ====================

-- BibItem class
local BibItem = {}
local mt = {__index = BibItem}
setmetatable(
  BibItem, {
    __call = function(_,t) return setmetatable(t, mt) end
})

-- replace user-defined strings and concatenate
local function process_value(val, strings, i)
  i = i or 2
  local t = {}
  for _, v in ipairs_from(val, i) do
    if type(v) == "table" then
      t[#t+1] = strings[v[1]] or ""
    else
      t[#t+1] = v
    end
  end
  return concat(t)
end

local default_options = {
  with_authors = true,
  with_title = true
}

--- Parse a bibtex file.
function bibtex.parse(src, options)
  options = merge(default_options, options)
  local entries = all_entries:match(src)
  local strings = merge(options.strings)
  local preambles = {}
  local ids = {}
  local items = {
    strings = strings,
    preambles = preambles,
    ids = ids
  }
  for _, t in ipairs(entries) do
    local entry_type = t[1]:lower()
    if entry_type == "comment" then
      -- pass
    elseif entry_type == "preamble" then
      preambles[#preambles + 1] = t[2]
    elseif entry_type == "string" then
      for _, u in ipairs_from(t, 2) do
        local key = u[1]:lower()
        local val = process_value(u, strings)
        strings[key] = val
      end
    else
      local id = t[2]
      local fields = {}
      for _, u in ipairs_from(t, 3) do
        local key = u[1]:lower()
        local val = process_value(u, strings)
        fields[key] = val
      end
      local item = BibItem {
        id = id,
        type = entry_type,
        fields = fields,
        pos = t.start,
        cont = t.stop
      }
      ids[id] = item
      items[#items + 1] = item
    end
  end
  return items
end

-- Deuglify strings
-- ================

local tex_symbols = {
  oe = "œ",
  OE = "Œ",
  ae = "ӕ",
  AE = "Ӕ",
  aa = "å",
  AA = "Å",
  ss = "ß",
   o = "ø",
   O = "Ø",
   l = "ł",
   L = "Ł",
   -- corner cases
   i = "{i}", -- not "ı"
   j = "j", -- not "ȷ"
   [" "] = " ",
}

local tex_accents = util.map(
  replace("◌", ""),
  {
    ['"'] = "◌̈",
    ["'"] = "◌́",
    ["."] = "◌̇",
    ["="] = "◌̄",
    ["^"] = "◌̂",
    ["`"] = "◌̀",
    ["~"] = "◌̃",
    ["c"] = "◌̧",
    ["d"] = "◌̣",
    ["H"] = "◌̋",
    ["u"] = "◌̆",
    ["b"] = "◌̱",
    ["v"] = "◌̌",
    ["t"] = "◌͡"
  }
)

local tex_letter = (R"AZ" + R"az")
local tex_char_or_math =  "$" * gobble_until("$") * "$" + char -- for deuglification purposes
local tex_cs_patt = "\\" * C(tex_letter^1 + char)* whitespace
local tex_accent_patt = tex_cs_patt * (curly_braced_string + C(char))
local function repl_accents_fun(cs, arg)
  local acc = tex_accents[cs]
  if not acc then
    return -- must return 0 values!
  else
    return arg .. acc
  end
end

local detexify_symbols = replace(tex_cs_patt, tex_symbols, tex_char_or_math)
local detexify_accents = replace(tex_accent_patt, repl_accents_fun, tex_char_or_math)
local debracify = replace(curly_braced_string, 1, tex_char_or_math)
local detitlify = replace(B(space) * C(tex_letter), string.lower, tex_char_or_math)
local trim = util.trim(space)
local clean = util.clean(space)

local function deuglify_name (s)
  return
    clean(
      debracify(
        detexify_accents(
          detexify_symbols(s))))
end

local function deuglify_title (s)
  return
    clean(
      debracify(
        detitlify(
          detexify_accents(
            detexify_symbols(s)))))
end

-- Pretty-printing
-- ===============

local split_authors = split(author_sep, token)
local split_name = split(comma, token)
local split_last = search(Cp() * C(nonspace^1) * whitespace * P(-1))

function BibItem:authors()
  local t = {}
  local author = self.fields.author
  if not author then return {} end
  for _, name in ipairs(split_authors(author)) do
    local u = {}
    local parts = split_name(name)
    if #parts == 3 then
      u.first = parts[3]
      u.last = parts[1]
      u.suffix = parts[2]
    elseif #parts == 2 then
      u.first = parts[2]
      u.last = parts[1]
    else
      local p, l = split_last:match(name)
      if p then
        u.first = name:sub(1, p - 1)
        u.last = l
      else
        u.last = name
      end
    end
    t[#t + 1] = u
  end
  return t
end

function BibItem:pretty_print()
  local t, a = {}, {}
  for _, name in ipairs(self:authors()) do
    a[#a + 1] = deuglify_name(name.last)
  end
  t[#t + 1] = concat(a, ", ")
  t[#t + 1] = (self.fields.year or self.fields.date or "(n.d.)") .. ";"
  t[#t + 1] = deuglify_title(self.fields.title or "")
  return concat(t, " ")
end

return bibtex
