project(
  'sockfish',
  'cpp',
  version : '0.3',
  default_options : [
    'cpp_std=c++17',
    'warning_level=3',
    'optimization=3'
  ]
)

cc = meson.get_compiler('cpp')
cxx_args = ['-fconstexpr-ops-limit=500000000']

# SFML libraries (link statically or dynamically depending on install)
sfml_libs = [
    declare_dependency(link_args: ['-L/usr/local/lib/SFML', '-lsfml-audio'], include_directories: ['/usr/include/SFML']),
    declare_dependency(link_args: ['-L/usr/local/lib/SFML', '-lsfml-graphics'], include_directories: ['/usr/include/SFML']),
    declare_dependency(link_args: ['-L/usr/local/lib/SFML', '-lsfml-window'], include_directories: ['/usr/include/SFML']),
    declare_dependency(link_args: ['-L/usr/local/lib/SFML', '-lsfml-system'], include_directories: ['/usr/include/SFML'])
]

cfg_sources = [
  'src/config/Config.cpp'
]

cli_sources = [
  'src/frontend/cli/CliFrontend.cpp'
]

gui_sources = [
  'src/frontend/gui/GuiFrontend.cpp',
  'src/frontend/gui/PromotionMenu.cpp',
]

uci_sources = [
  'src/frontend/uci/UciFrontend.cpp',
]

common_sources = [
  'src/ai/Engine.cpp',
  'src/ai/Evaluator.cpp',
  'src/ai/MoveSorter.cpp',
  'src/ai/PolyglotBook.cpp',
  'src/ai/TranspositionTable.cpp',
  'src/ai/search/Searcher.cpp',
  'src/ai/search/SearchStopwatch.cpp',
  'src/ai/search/ManualSearchStopper.cpp',
  'src/core/GameController.cpp',
  'src/core/Notation.cpp',
  'src/core/Position.cpp',
  'src/core/PGNWriter.cpp',
  'src/core/PositionUtil.cpp',
  'src/core/Printers.cpp',
  'src/movegen/MoveGenerator.cpp'
]

# -------------------- EXECUTABLE: main --------------------
main_exe = executable(
  'main',
  [ 'src/main.cpp', cfg_sources, cli_sources, gui_sources, common_sources ],
  dependencies : sfml_libs,
  cpp_args: cxx_args,
  install : false
)

# -------------------- EXECUTABLE: uci --------------------
uci_exe = executable(
  'sockfish',
  [ 'src/uci.cpp', uci_sources, common_sources ],
  cpp_args: cxx_args,
  install : false
)

# -------------------- EXECUTABLE: test/test --------------------
test_exe = executable(
  'test_test',
  [ 'src/test/test.cpp', common_sources ],
  cpp_args: cxx_args,
  install : false
)

# -------------------- EXECUTABLE: test/perft --------------------
test_exe = executable(
  'perft',
  [ 'src/test/perft.cpp', common_sources ],
  cpp_args: cxx_args,
  install : false
)

