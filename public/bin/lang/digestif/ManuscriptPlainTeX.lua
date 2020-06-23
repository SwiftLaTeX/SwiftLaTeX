local lpeg = require "lpeg"
local util = require "digestif.util"
local Manuscript = require "digestif.Manuscript"
local Parser = require "digestif.Parser"

local path_join, path_split = util.path_join, util.path_split
local nested_get, nested_put = util.nested_get, util.nested_put
local map, update, merge = util.map, util.update, util.merge
local format_filename_template = util.format_filename_template

local ManuscriptPlainTeX = util.class(Manuscript)

ManuscriptPlainTeX.parser = Parser()
ManuscriptPlainTeX.format = "plain"

ManuscriptPlainTeX.init_callbacks =
   setmetatable({}, {__index = Manuscript.init_callbacks})
ManuscriptPlainTeX.scan_references_callbacks = {}

return ManuscriptPlainTeX
