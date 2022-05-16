local libtopoheight = require 'libtopoheight'

TestLoadFile = {}

function TestLoadFile:testRead_LAYER6()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local rc = obj:load_file("examples/LAYER6.geojson")
  assertEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,187250)
  assertEquals(altitudes*2,coords)
  assertEquals(triangles,0)

  local rc = obj:triangulate()
  assertEquals(rc,0)

  local _,_, triangles = obj:debug_get_counts()
  --assertEquals(triangles / 3,coords)
  assertEquals(triangles % 3,0)

  obj:destroy()
end

function TestLoadFile:testRead_LAYER13()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)

  local rc = obj:load_file("examples/LAYER13.geojson")
  assertEquals(rc,0)

  local coords, altitudes, triangles = obj:debug_get_counts()
  assertEquals(coords,19186)
  assertEquals(altitudes*2,coords)
  assertEquals(triangles,0)

  local rc = obj:triangulate()
  assertEquals(rc,0)

  local _,_, triangles = obj:debug_get_counts()
  --assertEquals(triangles / 3,coords)
  assertEquals(triangles % 3,0)

  obj:destroy()
end
