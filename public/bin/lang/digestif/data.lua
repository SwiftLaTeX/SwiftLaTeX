local lpeg = require "lpeg"
local util = require "digestif.util"
local config = require "digestif.config"

local P = lpeg.P
local C, Cc = lpeg.C, lpeg.Cc

local data = {loaded = {}}

local token = P(1)
local xparse_patt = (P" "^0 * P"+"^-1 *
   (  "m" * Cc{optional = false}
    + "r" * C(token) * C(token)
          / function (tok1, tok2) return {delimiters = {tok1, tok2}} end
    + "u{"* C((token - "}")^0) * "}"
          / function (toks) return {delimiters = {"", toks}} end
    + "v" * Cc{type = "verbatim"}
    + "o" * Cc{optional = true, delimiters = {"[", "]"}}
    + "d" * C(token) * C(token)
          / function (tok1, tok2) return {optonal = true, delimiters = {tok1, tok2}} end
    + "s" * Cc{type = "literal", literal = "*", optional = true, delimiters = false}
    + "t" * C(token)
          / function (tok) return {type = "literal", literal = tok, optional = true} end
    + "g" * Cc{optional = true, delimiters = {"{", "}"}}))^0

function data.signature(sig, ...)
   local args = {...}
   for i, arg in ipairs({xparse_patt:match(sig)}) do
      if type(args[i]) ~= "table" then
         args[i] = {meta = args[i] or "#" .. i}
      end
      util.update(args[i], arg)
   end
   return args
end

local ref_patt = P"$DIGESTIFDATA/" * C(P(1)^0)
local ref_split = util.split("/")

local function resolve_refs(tbl)
  for k, v in pairs(tbl) do
    if type(v) == "string" then
      local path = ref_patt:match(v)
      if path then
        local t = ref_split(path)
        tbl[k] = util.nested_get(data.require(t[1]), table.unpack(t, 2))
      end
    elseif type(v) == "table" then
      resolve_refs(v) -- TODO: prevent infinite loops
    end
  end
end

local load_data_mt = {
   __index = {
      merge = util.merge,
      data = data.require,
      signature = data.signature
   }
}

function data.load(name)
  if name:find('..', 1, true) then return nil end -- unreasonable file name
  local src = util.try_read_file(config.data_dirs, name .. ".tags")
  if not src then return nil end
  local result, err = util.eval_with_env(src, load_data_mt)
  if not result and config.verbose then
    util.log("Error loading %s.tags: %s", name, err)
    return -- TODO: should throw an error?
  end
  data.loaded[name] = result
  resolve_refs(result)
  return result
end

function data.require(name)
   return data.loaded[name] or data.load(name)
end

---
-- Load all data files, and return them in a table.  This is intended
-- for debugging and testing only, and depends on luafilesystem.
function data.load_all()
  local ok, lfs = pcall(require, "lfs")
  assert(ok, "Function data.load_all() need the luafilesystem library.")
  for _, data_dir in ipairs(config.data_dirs) do
    for path in lfs.dir(data_dir) do
      local pkg = path:match("(.*)%.tags")
      if pkg then
        assert(data.require(pkg), "Couldn't load data file " .. path)
      end
    end
  end
  return data.loaded
end

return data
