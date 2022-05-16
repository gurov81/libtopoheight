local libtopoheight = require 'libtopoheight'
local helpers = require 'helpers'

local EPSILON = 0.000001

local Layer, Polygon, MultiPoint = helpers.Layer, helpers.Polygon, helpers.MultiPoint

TestHeightProperty = {}

function TestHeightProperty:test1()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)
  local rc = obj:load_buffer(
    Layer { Polygon( { {55,33,10}, {60,30,11}, {52,32,12} }, {SC_4=123} ) },
    nil,
    "SC_4"
  )
  assertEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,6)
  assertEquals(coords,altitudes*2)
  assertEquals(triangles,0)

  local rc = obj:triangulate()
  assertEquals(rc,0)

  local rc,alt = obj:get_alt(55,33)
  assertEquals(rc, 0)
  assertAlmostEquals(alt, 123)
  obj:destroy()
end
