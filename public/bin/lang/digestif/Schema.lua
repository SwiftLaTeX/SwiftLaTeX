--- A simple validation library, loosely inspired by JSON Schema

local Schema = {}
Schema.__index = Schema

local function is_schema(tbl)
  return getmetatable(tbl) == Schema
end
Schema.is_schema = is_schema

local validators, initializers = {}, {}

local function to_schema(tbl)
  if is_schema(tbl) then return tbl end
  for i = 1, #tbl do
    tbl[i] = to_schema(tbl[i])
  end
  -- local fields, items = tbl.fields, tbl.items
  -- if fields then
  --   for k, v in pairs(fields) do
  --     fields[k] = to_schema(v)
  --   end
  -- end
  -- if items then
  --   tbl.items = to_schema(items)
  -- end
  for k, fun in pairs(initializers) do
    if tbl[k] then tbl[k] = fun(tbl[k]) end
  end
  return setmetatable(tbl, Schema)
end
setmetatable(Schema, {__call = function(_, tbl) return to_schema(tbl) end})

local function msg_type(expect, got)
  return ("Type: expected %q, got %q"):format(expect, got)
end

local function msg_where(msg, loc_type, loc_name)
  return ("%s in %s %q"):format(msg, loc_type, loc_name)
end

initializers.items = to_schema
initializers.keys = to_schema
initializers.values = to_schema

function initializers.enum(tbl)
  local set = {}
  for i = 1, #tbl do
    set[tbl[i]] = true
  end
  return set
end

function initializers.fields(fields)
  for k, v in pairs(fields) do
    fields[k] = to_schema(v)
  end
  return fields
end

function validators.type(s, obj)
  local ok = (type(obj) == s)
  return ok, ok or msg_type(s, type(obj))
end

function validators.predicate(f, obj)
  return f(obj)
end

function validators.keys(schema, obj)
  if type(obj) ~= "table" then
    return false, msg_type("table", type(obj))
  end
  for k in pairs(obj) do
    local ok, msg = schema:validate(k)
    if not ok then
      return false, msg_where(msg, "key", k)
    end
  end
  return true
end

function validators.values(schema, obj)
  if type(obj) ~= "table" then
    return false, msg_type("table", type(obj))
  end
  for k, v in pairs(obj) do
    local ok, msg = schema:validate(v)
    if not ok then
      return false, msg_where(msg, "field", k)
    end
  end
  return true
end

function validators.fields(fields, obj)
  if type(obj) ~= "table" then
    return false, msg_type("table", type(obj))
  end
  for k, schema in pairs(fields) do
    local ok, msg = schema:validate(obj[k])
    if not ok then
      return false, msg_where(msg, "field", k)
    end
  end
  return true
end

function validators.items(schema, obj)
  if type(obj) ~= "table" then
    return false, msg_type("table", type(obj))
  end
  for i, item in ipairs(obj) do
    local ok, msg = schema:validate(item)
    if not ok then
      return false, msg_where(msg, "item", i)
    end
  end
  return true
end

function validators.enum(set, obj)
  local ok = set[obj]
  return ok, ok or ('Enum: %q not allowed'):format(obj)
end

function Schema:validate(obj)
  if self.optional and obj == nil then return true end
  for k, v in pairs(self) do
    local fun = validators[k]
    if fun then
      local ok, msg = fun(v, obj)
      if not ok then return false, msg end
    end
  end
  local len = #self
  if len > 0 then
    for i = 1, len do
      if self[i]:validate(obj) then break end
      if i == len then return false, "Alternatives: no match" end
    end
  end
  return true    
end

function Schema:assert(obj)
  return assert(self:validate(obj))
end

function Schema:assert_fail(obj)
  assert(not self:validate(obj))
end

schema_of_schema = Schema {
  fields = {
    description = {
      type = "string",
      optional = true,
      description = "A docstring for the schema"
    },
    fields = {
      type = "table",
      values = {predicate = is_schema}
    },
    enum = {
      type = "table"
    },
    items = {
      predicate = is_schema
    },
    optional = {
      type = "boolean",
      optional = true,
      description = "Whether nil values are accepted"
    },
    predicate = {
      type = "function",
      optional = true,
      description = "A function to test on the object"
    },
    type = {
      type = "string",
      optional = true,
      description = "Check if the object is of a given type"
    },
  }
}

return Schema
