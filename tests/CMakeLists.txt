# https://stackoverflow.com/questions/43875499/do-post-processing-after-make-install-in-cmake
# https://stackoverflow.com/questions/20792802/why-variables-are-not-accessed-inside-script-in-cmake
#install(CODE "set(SCRIPT_PATH \"${CMAKE_CURRENT_SOURCE_DIR}\")")

#set(R_PATH "${CMAKE_SOURCE_DIR}/source/uts-800/unit_tests")

# https://cmake.org/cmake/help/v3.7/command/install.html
#install(CODE "set(ROOT_PATH \"${R_PATH}\")")
#install(SCRIPT luaunit.cmake)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test_stamp
    #DEPENDS ${RDP}
    COMMAND ./run.lua -v
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #RESULT_VARIABLE rc
    COMMENT "testing libtopoheight"
  )
add_custom_target( topoheight-test ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/test_stamp )
