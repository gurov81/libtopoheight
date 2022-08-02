local libtopoheight = require 'libtopoheight'
local helpers = require 'helpers'

local Layer, Polygon, MultiPoint = helpers.Layer, helpers.Polygon, helpers.MultiPoint
local LineString = helpers.LineString

TestHeightmap = {}

function TestHeightmap:testRead_LAYER6()
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
  local x,y,dx,dy = 40,40,20,20
  local x,y,dx,dy = 40,43,2,2
  local rc = obj:get_heightmap({x-dx,y-dy,x+dx,y+dy},1024,768,"1.png")
  assertEquals(rc,0)
  obj:destroy()
end

--[[
function TestHeightmap:testMap_LAYER6()
  local obj = libtopoheight.new()
  local x,y,x2,y2 = 40,43,40.5,43.333
  local rc = obj:load_file("examples/LAYER6.geojson","SC_7")
  local rc = obj:triangulate()  
  local rc = obj:get_heightmap({x,y,x2,y2},2500,2500,"LAYER6.png")
  obj:destroy()
end
]]

function TestHeightmap:testPolygon_WithColorCallback()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)
  local rc = obj:load_buffer( Layer {
    Polygon { {0,0,100}, {10,0,200}, {10,10,300}, {0,10,400} }
  })
  assertEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,8)
  assertEquals(altitudes*2,coords)
  assertEquals(triangles,0)

  local rc = obj:triangulate()
  assertEquals(rc,0)

  local _,_, triangles = obj:debug_get_counts()
  assertEquals(triangles,6)

  --local trajectory = {{0, 5},{10, 5}}
  --local trajectory = {{0, 2},{5, 2},{5, 7},{10, 10}}
  local trajectory = {{0, 0},{10, 0},{10, 10},{0, 10}}
  local accuracy = 10
  local deviation = 0.1
  local rc = obj:get_vertical_profile(trajectory,accuracy,deviation)
  --local rc = obj:get_vertical_profile(trajectory)
  assertEquals(rc,0)

  --local rc = obj:get_heightmap({0,0,10,10},1024,1024,"2.png")
  --assertEquals(rc,0)

  --local str = helpers.dump_altitude_matrix(obj,{0,10,1},{0,10,1})
  --print("\n",str)

  obj:destroy()
end
