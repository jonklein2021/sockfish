project(
  'sockfish',
  'cpp',
  version : '0.2',
  default_options : [
    'cpp_std=c++17',
    'warning_level=3',
    'optimization=3'
  ]
)

cc = meson.get_compiler('cpp')

# SFML libraries (link statically or dynamically depending on install)
sfml_libs = [
  cc.find_library('sfml-graphics', dirs : ['/usr/lib/sfml', '/usr/local/lib/sfml','/usr/lib/SFML', '/usr/local/lib/SFML'], required: true),
  cc.find_library('sfml-window',   dirs : ['/usr/lib/sfml', '/usr/local/lib/sfml','/usr/lib/SFML', '/usr/local/lib/SFML'], required: true),
  cc.find_library('sfml-system',   dirs : ['/usr/lib/sfml', '/usr/local/lib/sfml','/usr/lib/SFML', '/usr/local/lib/SFML'], required: true),
]

common_sources = [
  'src/Config.cpp',
  'src/CliFrontend.cpp',
  'src/Engine.cpp',
  'src/GameController.cpp',
  'src/GuiFrontend.cpp',
  'src/Move.cpp',
  'src/MoveComputers.cpp',
  'src/MoveGenerator.cpp',
  'src/Position.cpp',
  'src/PositionUtil.cpp',
  'src/Printers.cpp',
  'src/PromotionMenu.cpp',
  'src/Zobrist.cpp'
]

# -------------------- EXECUTABLE: main --------------------
main_exe = executable(
  'main',
  [ 'src/main.cpp', common_sources ],
  dependencies : sfml_libs,
  install : false
)

# -------------------- EXECUTABLE: test/test --------------------
test_exe = executable(
  'test_test',
  [ 'src/test/test.cpp', common_sources ],
  dependencies : sfml_libs,
  install : false
)

# -------------------- EXECUTABLE: test/perft --------------------
test_exe = executable(
  'perft',
  [ 'src/test/perft.cpp', common_sources ],
  dependencies : sfml_libs,
  install : false
)

