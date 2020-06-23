local lpeg = require "lpeg"
local util = require "digestif.util"
local Manuscript = require "digestif.Manuscript"
local Parser = require "digestif.Parser"

local path_join, path_split = util.path_join, util.path_split
local nested_get, nested_put = util.nested_get, util.nested_put
local map, update, merge = util.map, util.update, util.merge
local format_filename_template = util.format_filename_template

local ManuscriptLaTeX = util.class(Manuscript)

ManuscriptLaTeX.parser = Parser()
ManuscriptLaTeX.format = "latex"

ManuscriptLaTeX.init_callbacks =
   setmetatable({}, {__index = Manuscript.init_callbacks})
ManuscriptLaTeX.scan_references_callbacks = {}

-- ¶ Snippets

--- Make a snippet for an environment.
--
-- @param cs The command name
-- @param args An argument list
-- @return The snippet string
function ManuscriptLaTeX:snippet_env(cs, args)
  local argsnippet = args and self:snippet_args(args) or ""
  return "begin{" .. cs .. "}" .. argsnippet .. "\n\t$0\n\\end{" .. cs .. "}"
end

--- Pretty-print an environment signature
--
-- @param cs The command name
-- @param args An argument list, or nil
-- @return A string
--
function ManuscriptLaTeX:signature_env(cs, args)
  return "\\begin{" .. cs .. "}" .. (args and self:signature_arg(args) or "")
end

-- ¶ Helper functions

--@param meta the desired meta string
--@param args a list of arg tables
local function find_by_meta(meta, args)
   for i, v in ipairs(args) do
      if meta == v.meta then return i end
   end
end

local function first_mand(args)
   for i, v in ipairs(args) do
      if not v.optional then return i end
   end
end

local function first_opt(args)
   for i, v in ipairs(args) do
      if v.optional  then return i end
   end
end

-- ¶ Global scan callbacks

function ManuscriptLaTeX.init_callbacks.label (m, pos, cs)
   local idx = m.label_index
   local args = m.commands[cs].arguments
   local r = m:parse_command(pos, cs)
   local i = first_mand(args)
   if r[i] then
      local l = m:substring_stripped(r[i])
      idx[#idx + 1] = {
         name = l,
         pos = r[i].pos,
         cont = r[i].cont,
         outer_pos = r.pos,
         outer_cont = r.cont,
         manuscript = m
      }
   end
   return r.cont
end

function ManuscriptLaTeX.init_callbacks.heading (m, pos, cs)
  local idx = m.heading_index
  local args = m.commands[cs].arguments
  local r = m:parse_command(pos, cs)
  local i = first_mand(args)
  if r[i] then
    idx[#idx + 1] = {
      title = m:substring_stripped(r[i]),
      pos = r[i].pos,
      cont = r[i].cont,
      outer_pos = r.pos,
      outer_cont = r.cont,
      level = m.commands[cs].heading_level,
      manuscript = m
    }
  end
  return r.cont
end

function ManuscriptLaTeX.init_callbacks.bibitem (m, pos, cs)
  local idx = m.bib_index
  local args = m.commands[cs].arguments
  local r = m:parse_command(pos, cs)
  local i = first_mand(args)
  if r[i] then
    idx[#idx + 1] = {
      name = m:substring_stripped(r[i]),
      pos = r[i].pos,
      cont = r[i].cont,
      outer_pos = r.pos,
      outer_cont = r.cont,
      manuscript = m
    }
  end
  return r.cont
end

function ManuscriptLaTeX.init_callbacks.amsrefs_bib(self, pos, cs)
  local idx = self.bib_index
  local r = self:parse_command(pos, cs)
  if r.incomplete then return r.cont end
  local keys = self:read_keys(r[3])
  local authors, title, date = {}, "", "(n.d.)"
  for i = 1, #keys do
    local k, v = keys[i].key, keys[i].value
    if k == "author" then
      authors[#authors+1] = self.parser.clean(v:match("[^,]+", 2))
    elseif k == "title" then
      title = self.parser.clean(v:sub(2, -2))
    elseif k == "date" then
      date = self.parser.clean(v:match("(%d+)"))
    end
  end
  idx[#idx + 1] = {
    name = self:substring_stripped(r[1]),
    text = string.format(
      "%s %s; %s",
      table.concat(authors, ", "),
      date,
      title),
    pos = r[1].pos,
    cont = r[1].cont,
    outer_pos = r.pos,
    outer_cont = r.cont,
    manuscript = self
  }
  return r.cont
end

-- function ManuscriptLaTeX.init_callbacks.begin(m, pos, cs)
--    local r = m:parse_command(pos, cs)
--    if r[1] then
--       m:add_outline({
--             env = m:substring_stripped(r[1]),
--             pos = r[1].pos,
--             level = math.huge})
--    end
--    return r.cont
-- end

function ManuscriptLaTeX.init_callbacks.input(m, pos, cs)
  local idx = m.child_index -- should this include the known modules, or just the filenames not know in /data?
  local args = m.commands[cs].arguments
  local filename = m.commands[cs].filename
  local r = m:parse_command(pos, cs)
  local i = first_mand(args)
  if r[i] then
    for _, k in ipairs(m:read_list(r[i])) do
      local f = format_filename_template(filename, k)
      m:add_module(f)
      if not m.modules[f] then
        f = path_join(path_split(m.filename), f)
        idx[#idx + 1] = {
          name = f,
          pos = r[i].pos,
          cont = r[i].cont,
          filename = m.filename
        }
      end
    end
  end
  return r.cont
end

-- ¶ Scan reference callbacks

function ManuscriptLaTeX.scan_references_callbacks.ref(self, pos, cs)
  local idx = self.ref_index
  local args = self.commands[cs].arguments
  local r = self:parse_command(pos, cs)
  local i = first_mand(args)
  if r[i] then
    local l = self:substring_stripped(r[i])
    idx[#idx + 1] = {
      name = l,
      pos = r[i].pos,
      cont = r[i].cont,
      outer_pos = r.pos,
      outer_cont = r.cont,
      manuscript = self
    }
  end
  return r.cont
end

function ManuscriptLaTeX.scan_references_callbacks.cite(self, pos, cs)
  -- TODO: allow lists using Manuscript:argument_items
  local idx = self.cite_index
  local args = self.commands[cs].arguments
  local r = self:parse_command(pos, cs)
  local i = first_mand(args)
  if r[i] then
    local l = self:substring_stripped(r[i])
    idx[#idx + 1] = {
      name = l,
      pos = r[i].pos,
      cont = r[i].cont,
      outer_pos = r.pos,
      outer_cont = r.cont,
      manuscript = self,
    }
  end
  return r.cont
end

return ManuscriptLaTeX
