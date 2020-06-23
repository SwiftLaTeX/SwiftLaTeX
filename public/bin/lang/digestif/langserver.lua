local Cache = require "digestif.Cache"
local config = require "digestif.config"
local json = require "dkjson"
local util = require "digestif.util"

local log, nested_get = util.log, util.nested_get

local cache = Cache()
local null = json.null

-- a place to store the tex_format/languageId of open files
local tex_format_table = setmetatable({}, {
  __index = function(_, k)
    error(("Trying to access unopened file %s"):format(k))
  end
})

-- ¶ Convert LSP API objects to/from internal representations

-- TODO: deal with weird path separators
local function from_DocumentUri(str)
  str = str:match("^file://([^#%?]*)$")
    or error("Invalid or unsupported URI: " .. str)
  str = str:gsub('%%(%x%x)',
                 function(x)
                   return string.char(tonumber(x, 16))
                 end)
  return str
end

local function to_DocumentUri(str)
  str = str:gsub("[^0-9A-Za-z%-._~/]",
                 function(s)
                   return string.format("%%%X", string.byte(s))
                 end)
  return "file://" .. str
end

-- p0 is the position of a line l0, provided as a hint for the search
-- returns a position in btyes, and a new hint p0, l0
local function from_Position(str, position, p0, l0)
  local l, c = position.line + 1, position.character + 1
  if l0 and l0 > l then p0, l0 = nil, nil end
  for n, i in util.lines(str, p0, l0) do
    if n == l then
      return utf8.offset(str, c, i), i, l
    end
  end
end

local function from_Range(str, range, p0, l0)
  local pos, p1, l1 = from_Position(str, range.start, p0, l0) -- inclusive
  local cont = from_Position(str, range['end'], p1, l1) -- exclusive
  if not (pos and cont) then error("Position out of bounds") end
  return pos, cont, p1, l1
end

local function from_TextDocumentPositionParams(arg)
  local filename = from_DocumentUri(arg.textDocument.uri)
  local format = tex_format_table[filename]
  local script = cache:manuscript(filename, format)
  local l, c = arg.position.line + 1, arg.position.character + 1
  return script, script:position_at(l, c)
end

local function to_Range(item)
  local script = item.manuscript
  local l1, c1 = script:line_column_at(item.pos)
  local l2, c2 = script:line_column_at(item.cont)
  return {
    start = {line = l1 - 1, character = c1 - 1},
    ["end"] = {line = l2 - 1, character = c2 - 1},
  }
end

local function to_Location(item)
  return {
    uri = to_DocumentUri(item.manuscript.filename),
    range = to_Range(item)
  }
end

local function to_MarkupContent(str)
  return {kind = "markdown", value = str}
end

local function to_TextEdit(script, pos, old, new)
  local l, c_start = script:line_column_at(pos)
  local c_end = c_start + utf8.len(old)
  return {
    range = {
      start = {line = l - 1, character = c_start - 1},
      ["end"] = {line = l - 1, character = c_end - 1},
    },
    newText = new
  }
end

local languageId_translation_table = {
  bibtex = "bibtex",
  context = "context",
  latex = "latex",
  plain = "plain",
  plaintex = "plain",
  ["plain-tex"] = "plain",
  tex = "latex", -- this is for vim; maybe "tex" should mean "tex file, undecided format"
  texinfo = "texinfo"
}

-- ¶ LSP methods

local methods = {}

methods["initialize"] = function(params)
  if params.trace == "verbose" then config.verbose = true end
  config.provide_snippets = nested_get(params.capabilities,
    "textDocument", "completion", "completionItem", "snippetSupport")
  return {
    capabilities = {
      textDocumentSync = {
        openClose = true,
        change = 2
      },
      completionProvider = {
        triggerCharacters = {"\\", "{", "[", ",", "="},
      },
      signatureHelpProvider = {
        triggerCharacters = {"{", "[", "="},
      },
      hoverProvider = true,
      definitionProvider = true,
      referencesProvider = true
    }
  }
end

methods["initialized"] = function() end
methods["shutdown"] = function() return null end
methods["exit"] = function() os.exit() end
methods["workspace/didChangeConfiguration"] = function() end
methods["textDocument/willSave"] = function() end
methods["textDocument/didSave"] = function() end

methods["textDocument/didOpen"] = function(params)
  local filename = from_DocumentUri(params.textDocument.uri)
  local format = languageId_translation_table[params.textDocument.languageId]
  if not format then
    error(("Invalid languageId %q"):format(params.textDocument.languageId))
  end
  tex_format_table[filename] = format
  cache:put(filename, params.textDocument.text)
end

methods["textDocument/didChange"] = function(params)
  local filename = from_DocumentUri(params.textDocument.uri)
  local p0, l0, src = 1, 1, cache(filename)
  for _, change in ipairs(params.contentChanges) do
    if change.range then
      local pos, cont
      pos, cont, p0, l0 = from_Range(src, change.range, p0, l0)
      if change.rangeLength ~= utf8.len(src, pos, cont - 1) then
        error("Range length mismatch in textdocument/didChange operation")
      end
      src = src:sub(1, pos - 1) .. change.text .. src:sub(cont)
    else
      src = change.text
    end
  end
  globalSrcVariable = src
  globalSrcUriVariable = params.textDocument.uri
  cache:put(filename, src)
end

methods["textDocument/didClose"] = function(params)
  local filename = from_DocumentUri(params.textDocument.uri)
  cache:forget(filename)
end

methods["textDocument/signatureHelp"] = function(params)
  local script, pos = from_TextDocumentPositionParams(params)
  local help = script:describe(pos)
  if not nested_get(help, "data", "arguments") then return null end
  local parameters = {}
  for i, arg in ipairs(help.data.arguments) do
    parameters[i] = {
      label = arg.meta,
      documentation = arg.summary
    }
  end
  return {
    signatures = {
      [1] = {
        label = help.label,
        documentation = help.summary,
        parameters = parameters
      }
    },
    activeSignature = 0,
    activeParameter = help.arg and help.arg - 1
  }
end

methods["textDocument/hover"] = function(params)
  local script, pos = from_TextDocumentPositionParams(params)
  local help = script:describe(pos)
  if (not help) or help.arg then return null end
  local contents = help.details or help.summary or "???"
  return {contents = to_MarkupContent(contents)}
end

methods["textDocument/completion"] = function(params)
  local script, pos = from_TextDocumentPositionParams(params)
  local candidates = script:complete(pos)
  if not candidates then return null end
  local result = {}
  for i, cand in ipairs(candidates) do
    local snippet = config.provide_snippets and cand.snippet
    result[i] = {
      label = cand.text,
      filterText = candidates.prefix, -- Workaround to allow “flex matching”
      sortText=("%05x"):format(i), -- Workaround to avoid client re-sorting
      documentation = cand.summary,
      detail = cand.annotation,
      insertTextFormat = snippet and 2 or 1,
      textEdit = to_TextEdit(script,
                             candidates.pos,
                             candidates.prefix,
                             snippet or cand.text)
    }
  end
  return result
end

methods["textDocument/definition"] = function(params)
  local script, pos = from_TextDocumentPositionParams(params)
  local definition = script:find_definition(pos)
  return definition and to_Location(definition) or null
end

methods["textDocument/references"] = function(params)
  local script, pos = from_TextDocumentPositionParams(params)
  local result = {}
  if params.context and params.context.includeDeclaration then
    local definition = script:find_definition(pos)
    if definition then
      result[#result + 1] = to_Location(definition)
    end
  end
  local references = script:find_references(pos)
  if references then
    for _, ref in ipairs(references) do
      result[#result + 1] = to_Location(ref)
    end
  end
  if #result > 0 then
    return result
  else
    return null
  end
end

-- ¶ RPC functions and the main loop

local function log_error(err)
  if config.verbose then
    log("Error: %s", err)
    log(debug.traceback())
  end
  return err
end

local function write_msg(msg)
  io.write(msg, '\n')
  io.flush()
end

local function read_msg()
  msg = globalInputVariable
  return msg
end

local function rpc_send(id, result, error_code)
  write_msg(
    json.encode({
        jsonrpc = "2.0",
        id = id,
        result = not error_code and result,
        error = error_code and {code = error_code, message = result}
  }))
end

local function rpc_receive()
  local msg = read_msg()
  local success, request = xpcall(json.decode, log_error, msg)
  if not success then
    rpc_send(json.null, request, -32700)
    return
  end
  return request.id, request.method, request.params
end

local is_optional = util.matcher("$/")

local function process_request()
  local clock = config.verbose and os.clock()
  local id, method_name, params = rpc_receive()
  local method = methods[method_name]
  if method then
    local success, result = xpcall(method, log_error, params)
    if success then
      if id then rpc_send(id, result) end
    else
      rpc_send(id, result, 1)
    end
  elseif not is_optional(method_name) then
    rpc_send(id, "Unknown method " .. method_name, -32601)
  end
  if clock then
    log("Request: %4s %-40s %6.2f ms",
        id or "*", method_name, 1000 * (os.clock() - clock))
  end
end

local function main()
  config.verbose = false

  -- Set up data path and check if it worked
  config.data_dirs = "data/"
  if not require("digestif.data").require("primitives") then
    error("Could not find data files at the following locations:\n- "
            .. table.concat(config.data_dirs, "\n- ")
            .. "\nSet the environment variable DIGESTIFDATA to fix this.")
  end

  if config.verbose then
    log("━━━━━━ digestif started! ━━━━━━━━━━━━━━ %s", os.date())
  end

end

return {
  main = main,
  process_request = process_request,
  methods = methods
}

