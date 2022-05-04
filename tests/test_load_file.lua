local libtopoheight = require 'libtopoheight'

TestLoadFile = {}

function TestLoadFile:testReadFile()
  local obj = libtopoheight.new()
  assertNotIsNil(obj)
  local rc = obj:load_file("examples/LAYER6.geojson")
  assertEquals(rc,0)
end
