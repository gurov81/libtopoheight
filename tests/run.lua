#!/usr/bin/luajit

io.stdout:setvbuf("no")

package.path = package.path .. ';../build/?.lua;lib/?.lua'
package.cpath = package.cpath .. ';../build/?.so'

EXPORT_ASSERT_TO_GLOBALS = true
_G.real_print = print

require('luaunit')

--require 'test_load_buffer'
--require 'test_load_file'
--require 'test_height'
--require 'test_height_property'
require 'test_heightmap'

os.exit(LuaUnit.run())
