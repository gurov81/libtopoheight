local dkjson = require 'dkjson'

local function Layer(args)
  return dkjson.encode {
    type = "FeatureCollection", name = "LAYER1", features = args
  }
end

local function Polygon(args,properties)
  return {
    type = "Feature", properties=properties, geometry = { type = "Polygon", coordinates = { args } }
  }
end

local function LineString(args,properties)
  return {
    type = "Feature", properties=properties, geometry = { type = "LineString", coordinates = { args } }
  }
end

local function MultiPoint(args,properties)
  return {
    type = "Feature", properties=properties, geometry = { type = "MultiPoint", coordinates = { args } }
  }
end

local function dump_altitude_matrix(obj,X,Y)
  --X = {Xmin,Xmax,delta_x}
  --Y = {Ymin,Ymax,delta_y}
  local t = {}
  for y=Y[2],Y[1],-Y[3] do --выводим сверху-вниз
    local line = {}
    for x=X[1],X[2],X[3] do
      local rc,alt = obj:get_alt(x,y)
      assertEquals(rc,0)
      line[#line+1] = string.format("%3.0f",alt)
    end
    t[#t+1] = table.concat(line,' ')
  end
  return table.concat(t,'\n')
end

return {
  Layer = Layer,
  Polygon = Polygon,
  LineString = LineString,
  MultiPoint = MultiPoint,
  dump_altitude_matrix = dump_altitude_matrix,
}
