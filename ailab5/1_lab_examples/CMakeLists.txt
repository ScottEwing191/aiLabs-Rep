cmake_minimum_required(VERSION 3.0.0)
project(gecode-lab-examples)
include_directories("../Gecode/include")
link_directories("../Gecode/lib")

set(examples lab1 lab2 lab3 lab4 lab5 lab6 lab7)
set(dbg-sfx -6-2-0-d-x64)

set(GECODE_LIBS GecodeMinimodel${dbg-sfx} GecodeDriver${dbg-sfx} GecodeSet${dbg-sfx} GecodeSearch${dbg-sfx} GecodeFloat${dbg-sfx} GecodeInt${dbg-sfx} GecodeKernel${dbg-sfx} GecodeSupport${dbg-sfx} GecodeFlatZinc${dbg-sfx} GecodeSupport${dbg-sfx})

foreach(n ${examples})
  add_executable(${n} ${n}.cpp)
  set_target_properties(${n} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=../../Gecode/bin;%PATH%")
  # Is this also necessary when adding in say Gist::dfs(&l7) ? I think not.
  # QT_QPA_PLATFORM_PLUGIN_PATH=../../Gecode/bin/platforms
  target_link_libraries(${n} ${GECODE_LIBS})
endforeach(n)

