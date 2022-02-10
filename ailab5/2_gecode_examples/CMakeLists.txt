cmake_minimum_required(VERSION 3.0.0)
project(gecode-examples)
#include_directories("./gecode_vs2015")
#link_directories("./gecode_vs2015/build")
include_directories("../Gecode/include")
include_directories("../Gecode") # Ach...for examples/sudoku-instances.hh
link_directories("../Gecode/lib")
#include_directories(".") #for scowl.hpp

set(examples all-interval alpha archimedean-spiral bacp bibd bin-packing black-hole car-sequencing cartesian-heart colored-matrix crew crossword crowded-chess descartes-folium dominating-queens domino donald efpa eq20 golden-spiral golf golomb-ruler graph-color grocery hamming ind-set kakuro knights langford-number magic-sequence magic-square minesweeper money multi-bin-packing nonogram open-shop ortho-latin partition pentominoes perfect-square photo qcp queen-armies queens radiotherapy sat schurs-lemma sports-league steel-mill steiner sudoku tsp warehouses word-square)

set(dbg-sfx -6-2-0-d-x64)

set(GECODE_LIBS GecodeMinimodel${dbg-sfx} GecodeDriver${dbg-sfx} GecodeSet${dbg-sfx} GecodeSearch${dbg-sfx} GecodeFloat${dbg-sfx} GecodeInt${dbg-sfx} GecodeKernel${dbg-sfx} GecodeSupport${dbg-sfx} GecodeFlatZinc${dbg-sfx} GecodeSupport${dbg-sfx})

foreach(n ${examples})
  add_executable(${n} ../Gecode/examples/${n}.cpp)
  set_target_properties(${n} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=../../Gecode/bin;%PATH%")
  target_link_libraries(${n} ${GECODE_LIBS})
endforeach(n)

