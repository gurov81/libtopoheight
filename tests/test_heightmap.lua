local libtopoheight = require 'libtopoheight'

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

  --local x,y,dx,dy = 39,56,2,2
  local x,y,dx,dy = 40,40,20,20
  local x,y,dx,dy = 40,43,2,2
  local rc = obj:get_heightmap({x-dx,y-dy,x+dx,y+dy},1024,768,"1.png")
  assertEquals(rc,0)

  obj:destroy()
end
