#!/usr/bin/luajit

io.stdout:setvbuf("no")

package.path = package.path .. ';../build/?.lua;lib/?.lua'
package.cpath = package.cpath .. ';../build/?.so'

EXPORT_ASSERT_TO_GLOBALS = true
_G.real_print = print

require('luaunit')

require 'test_load_buffer'
require 'test_load_file'

os.exit(LuaUnit.run())
