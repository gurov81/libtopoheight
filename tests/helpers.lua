local dkjson = require 'dkjson'

local function Layer(args)
  return dkjson.encode {
    type = "FeatureCollection", name = "LAYER1", features = args
  }
end

local function Polygon(args)
  return {
    type = "Feature", geometry = { type = "Polygon", coordinates = { args } }
  }
end

local function MultiPoint(args)
  return {
    type = "Feature", geometry = { type = "MultiPoint", coordinates = { args } }
  }
end

return {
  Layer = Layer,
  Polygon = Polygon,
  MultiPoint = MultiPoint,
}