local util = require "digestif.util"
local Manuscript = require "digestif.Manuscript"
local Parser = require "digestif.Parser"

local ManuscriptTexinfo = util.class(Manuscript)

ManuscriptTexinfo.parser = Parser({
  escape = "@"
})

ManuscriptTexinfo.format = "texinfo"

ManuscriptTexinfo.init_callbacks =
   setmetatable({}, {__index = Manuscript.init_callbacks})
ManuscriptTexinfo.scan_references_callbacks = {}

--- Make a snippet for an environment.
--
-- @param cs The command name
-- @param args An argument list
-- @return The snippet string
function ManuscriptTexinfo:snippet_env(cs, args)
  local argsnippet = args and self:snippet_args(args) or ""
  return cs .. argsnippet .. "\n\t$0\n@end " .. cs
end

--- Pretty-print a command signature
--
-- @param cs The command name
-- @param args An argument list, or nil
-- @return A string
--
function Manuscript:signature_cmd(cs, args)
  return "@" .. cs .. (args and self:signature_arg(args) or "")
end

return ManuscriptTexinfo
