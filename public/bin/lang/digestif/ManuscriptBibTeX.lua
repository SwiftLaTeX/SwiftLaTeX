local lpeg = require "lpeg"
local util = require "digestif.util"
local Manuscript = require "digestif.Manuscript"
local Parser = require "digestif.Parser"
local bibtex = require "digestif.bibtex"

local path_join, path_split = util.path_join, util.path_split
local nested_get, nested_put = util.nested_get, util.nested_put
local map, update, merge = util.map, util.update, util.merge

local ManuscriptBibTeX = util.class(Manuscript)

ManuscriptBibTeX.format = "bibtex"
ManuscriptBibTeX.parser = Parser()
ManuscriptBibTeX.init_callbacks = false

function ManuscriptBibTeX:__init(args)
  Manuscript.__init(self, args)
  local bibitems = bibtex.parse(self.src)
  local idx = {}
  self.bib_index = idx
  for i, item in ipairs(bibitems) do
    idx[i] = {
      name = item.id,
      pos = item.pos,
      cont = item.cont,
      manuscript = self,
      text = item:pretty_print(),
      bibitem = item,
    }
  end
end

return ManuscriptBibTeX
