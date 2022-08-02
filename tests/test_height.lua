local libtopoheight = require 'libtopoheight'
local helpers = require 'helpers'

local EPSILON = 0.000001

local Layer, Polygon, MultiPoint = helpers.Layer, helpers.Polygon, helpers.MultiPoint
local LineString = helpers.LineString

TestHeight = {}

local polygon = Polygon { {0,0,100}, {1,0,200}, {1,1,300}, {0,1,400} }

local test_cases = {
  --on edges  
  [{0.5,0.0}] = 150,
  [{1.0,0.5}] = 250,
  [{0.5,1.0}] = 350,
  [{0.0,0.5}] = 250,
}

for k,v in ipairs( polygon.geometry.coordinates[1] ) do
  local x,y,alt = v[1],v[2],v[3]
  TestHeight['testInPoints_'..x..'_'..y] = function()
    local obj = libtopoheight.new()
    assertNotIsNil(obj)
    local rc = obj:load_buffer( Layer { polygon } )
    assertEquals(rc,0)
    local rc = obj:triangulate()
    assertEquals(rc,0)
    local rc,h = obj:get_alt(x,y)
    assertEquals(rc, 0)
    assertAlmostEquals(h, alt)
    obj:destroy()
  end
end

for k,v in pairs(test_cases) do
  local x,y,alt = k[1],k[2],v
  TestHeight['testCase_'..x..'_'..y] = function()
    local obj = libtopoheight.new()
    assertNotIsNil(obj)
    local rc = obj:load_buffer( Layer { polygon } )
    assertEquals(rc,0)
    local rc = obj:triangulate()
    assertEquals(rc,0)
    local rc,h = obj:get_alt(x,y)
    assertEquals(rc, 0)
    assertAlmostEquals(h, alt)
    obj:destroy()
  end
end

TestHeight['testCase_break_line'] = function()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local src_data = {
    LineString({ {0, 0, 0}, {1000, 0, 0} }),
    MultiPoint({ {500, -10, 222} }),
    MultiPoint({ {500,  10, 222} }),
  }

  local rc = obj:load_buffer(Layer(src_data))
  assertEquals(rc,0)
  local rc = obj:triangulate()
  assertEquals(rc,0)

  local rc,h = obj:get_alt(500,0)
  assertEquals(rc, 0)
  assertEquals(h, 0)

  obj:destroy()
end

local checkPoint = function(obj, x, y, expected)
  local rc,h = obj:get_alt(x, y)
  assertEquals(rc, 0)
  assertEquals(h, expected)
end

TestHeight['testCase_interpolation'] = function()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local src_data = {
    LineString({ {0, 0, 0}, {100, 0, 0} }),
    LineString({ {0, 100, 20}, {100, 100, 20} }),
    MultiPoint({ {50, 50, 30} }),
  }

  local rc = obj:load_buffer(Layer(src_data))
  assertEquals(rc,0)
  local rc = obj:triangulate()
  assertEquals(rc,0)

  -- corners
  checkPoint(obj,   0,   0,  0)
  checkPoint(obj, 100,   0,  0)
  checkPoint(obj,   0, 100, 20)
  checkPoint(obj, 100, 100, 20)
  -- mid vert
  checkPoint(obj, 50,   0,  0)
  checkPoint(obj, 50,  25, 15)
  checkPoint(obj, 50,  50, 30)
  checkPoint(obj, 50,  75, 25)
  checkPoint(obj, 50, 100, 20)
  -- mid hor
  checkPoint(obj,   0, 50, 10)
  checkPoint(obj,  25, 50, 20)
  checkPoint(obj,  50, 50, 30)
  checkPoint(obj,  75, 50, 20)
  checkPoint(obj, 100, 50, 10)

  obj:destroy()
end
