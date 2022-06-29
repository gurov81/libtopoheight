local libtopoheight = require 'libtopoheight'
local helpers = require 'helpers'

local Layer, Polygon, MultiPoint = helpers.Layer, helpers.Polygon, helpers.MultiPoint
local LineString = helpers.LineString

TestHeightmap = {}

--[[function TestHeightmap:testRead_LAYER6()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)
  local rc = obj:load_file("examples/LAYER6.geojson","SC_4")
  assertEquals(rc,0)
  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,2128)
  assertEquals(altitudes*2,coords)
  assertEquals(triangles,0)
  local rc = obj:triangulate()
  assertEquals(rc,0)
  local rc,alt = obj:get_alt(39.7799788, 56.4060258)
  assertEquals(rc,0)
  assertEquals(alt,121.0)
  --local x,y,dx,dy = 39,56,2,2
  local x,y,dx,dy = 40,40,20,20
  local x,y,dx,dy = 40,43,2,2
  local rc = obj:get_heightmap({x-dx,y-dy,x+dx,y+dy},1024,768,"1.png")
  assertEquals(rc,0)
  obj:destroy()
end--]]

function TestHeightmap:testPolygon_WithColorCallback()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local src_data = {
    Polygon { {0,0,200}, {10,0,200}, {10,10,200}, {0,10,200} },
    LineString({ {2, 2, 0}, {8, 8, 0} }),
    MultiPoint({ {4, 6, 400} }),
    MultiPoint({ {6, 4, 400} }),
  }

  local rc = obj:load_buffer(Layer(src_data))

  --[[local rc = obj:load_buffer( Layer {
    --Polygon { {0,0,100}, {10,0,200}, {10,10,300}, {0,10,400} }
  })--]]
  assertEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  --assertEquals(coords,8)
  assertEquals(altitudes*2,coords)
  assertEquals(triangles,0)

  local rc = obj:triangulate()
  assertEquals(rc,0)

  local _,_, triangles = obj:debug_get_counts()
  --assertEquals(triangles,6)

  local function get_altitude_color(alt) --0xAARRGGBB
    if alt<100 or alt>400 then return 0 end
    local blue = 0xff*(alt-100)/300
    return blue
  end
  local function get_altitude_color_v2(alt) --0xAARRGGBB
    local red,green,blue,white = 0x00FF0000,0x0000FF00,0x000000FF,0x00FFFFFF
    if     math.abs(alt-100)<50 then return red
    elseif math.abs(alt-200)<50 then return green
    elseif math.abs(alt-300)<50 then return blue
    elseif math.abs(alt-400)<50 then return white end
    return 0
  end

  local rc = obj:get_heightmap({0,0,10,10},1024,1024,"2.png",get_altitude_color)
  --assertEquals(rc,0)

  local str = helpers.dump_altitude_matrix(obj,{0,10,1},{0,10,1})
  --print(str)
--[[  assertEquals(str,[[
400 400 400 400 400 400 400 400 400 400 300
400 352 334 319 308 300 296 294 294 300 300
400 329 319 308 300 294 291 290 300 256 300
400 306 300 295 290 287 285 300 236 242 300
400 283 281 279 278 277 300 220 226 230 300
400 260 261 263 265 300 206 212 217 220 300
400 236 240 245 300 194 200 205 209 212 300
400 210 218 300 180 188 195 200 204 206 300
400 182 300 164 174 183 191 196 200 202 300
400 300 144 158 170 180 188 194 198 200 300--]]
--100 200 200 200 200 200 200 200 200 200 200]])



  obj:destroy()
end
