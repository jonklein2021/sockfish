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

common_sources = [
  'src/ai/Engine.cpp',
  'src/ai/Evaluator.cpp',
  'src/ai/MoveSorter.cpp',
  'src/ai/TranspositionTable.cpp',
  'src/core/GameController.cpp',
  'src/core/Notation.cpp',
  'src/core/Position.cpp',
  'src/core/PositionUtil.cpp',
  'src/core/Printers.cpp',
  'src/config/Config.cpp',
  'src/ui/cli/CliFrontend.cpp',
  'src/ui/gui/GuiFrontend.cpp',
  'src/ui/gui/PromotionMenu.cpp',
  'src/movegen/MoveGenerator.cpp'
]

# -------------------- EXECUTABLE: main --------------------
main_exe = executable(
  'main',
  [ 'src/main.cpp', common_sources ],
  dependencies : sfml_libs,
  cpp_args: cxx_args,
  install : false
)

# -------------------- EXECUTABLE: uci --------------------
uci_exe = executable(
  'uci',
  [ 'src/uci.cpp', common_sources ],
  dependencies : sfml_libs,
  cpp_args: cxx_args,
  install : false
)

# -------------------- EXECUTABLE: test/test --------------------
test_exe = executable(
  'test_test',
  [ 'src/test/test.cpp', common_sources ],
  dependencies : sfml_libs,
  cpp_args: cxx_args,
  install : false
)

# -------------------- EXECUTABLE: test/perft --------------------
test_exe = executable(
  'perft',
  [ 'src/test/perft.cpp', common_sources ],
  dependencies : sfml_libs,
  cpp_args: cxx_args,
  install : false
)

