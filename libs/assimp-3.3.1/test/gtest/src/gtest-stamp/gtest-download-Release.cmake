

set(command "C:/Program Files/CMake/bin/cmake.exe;-P;E:/mattt/Documents/assimp-3.3.1/test/gtest/tmp/gtest-gitclone.cmake")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "E:/mattt/Documents/assimp-3.3.1/test/gtest/src/gtest-stamp/gtest-download-out.log"
  ERROR_FILE "E:/mattt/Documents/assimp-3.3.1/test/gtest/src/gtest-stamp/gtest-download-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  E:/mattt/Documents/assimp-3.3.1/test/gtest/src/gtest-stamp/gtest-download-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "gtest download command succeeded.  See also E:/mattt/Documents/assimp-3.3.1/test/gtest/src/gtest-stamp/gtest-download-*.log")
  message(STATUS "${msg}")
endif()
