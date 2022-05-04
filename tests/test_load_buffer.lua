local libtopoheight = require 'libtopoheight'
local helpers = require 'helpers'

local EPSILON = 0.000001

local Layer, Polygon, MultiPoint = helpers.Layer, helpers.Polygon, helpers.MultiPoint

TestLoadBuffer = {}


function TestLoadBuffer:testPolygon_Doubles_3points()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local rc = obj:load_buffer( Layer {
    Polygon { {55.01,33.01,0.01}, {60.01,30.01,0.02}, {52.01,32.01,0.03} }
  })
  assertEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,6)
  assertEquals(altitudes*2,coords)
  assertEquals(triangles,0)

  local coord_x,coord_y = obj:debug_get_coords(1)
  local coord_alt = obj:debug_get_altitude(1)
  assertAlmostEquals(coord_x, 60.01, EPSILON)
  assertAlmostEquals(coord_y, 30.01, EPSILON)
  assertAlmostEquals(coord_alt, 0.02, EPSILON)

  local rc = obj:triangulate()
  assertEquals(rc,0)
  local _,_, triangles = obj:debug_get_counts()
  assertEquals(triangles / 3,1)
  assertEquals(triangles % 3,0)

  obj:destroy()
end

function TestLoadBuffer:testPolygon_Doubles_4points()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local rc = obj:load_buffer( Layer {
    Polygon { {55.01,33.01,0.01}, {60.01,30.01,0.02}, {52.01,32.01,0.03}, {51.01,31.01,0.04} }
  })
  assertEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,8)
  assertEquals(altitudes*2,coords)
  assertEquals(triangles,0)

  local rc = obj:triangulate()
  assertEquals(rc,0)
  local _,_, triangles = obj:debug_get_counts()
  assertEquals(triangles / 3,2)
  assertEquals(triangles % 3,0)

  obj:destroy()
end


function TestLoadBuffer:testPolygon_Doubles_5points()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local rc = obj:load_buffer( Layer {
    Polygon { {55.01,33.01,0.01}, {60.01,30.01,0.02}, {52.01,32.01,0.03}, {51.01,31.01,0.04}, {50.02,30.02,0.05} }
  })
  assertEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,10)
  assertEquals(altitudes*2,coords)
  assertEquals(triangles,0)

  local rc = obj:triangulate()
  assertEquals(rc,0)
  local _,_, triangles = obj:debug_get_counts()
  assertEquals(triangles / 3,3)
  assertEquals(triangles % 3,0)

  obj:destroy()
end


function TestLoadBuffer:testPolygon_Integers()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local rc = obj:load_buffer( Layer {
    Polygon { {55,33,0}, {60,30,0}, {52,32,0} }
  })
  assertEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,6)
  assertEquals(coords,altitudes*2)
  assertEquals(triangles,0)

  obj:destroy()
end

function TestLoadBuffer:testMultiPoint_Doubles()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local rc = obj:load_buffer( Layer {
    MultiPoint {55.01,33.01,0.01}
  })
  assertEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,2)
  assertEquals(coords,altitudes*2)
  assertEquals(triangles,0)

  obj:destroy()
end


function TestLoadBuffer:testMalformed_JSON()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local rc = obj:load_buffer [[
  a=123
  c:"aaa"
  ]]
  assertNotEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,0)
  assertEquals(altitudes,0)
  assertEquals(triangles,0)

  obj:destroy()
end
