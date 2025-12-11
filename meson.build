
project(
  'sockfish',
  'cpp',
  version : '0.1',
  default_options : [
    'cpp_std=c++17',
    'warning_level=3',
    'optimization=3'
  ]
)

cc = meson.get_compiler('cpp')
cxx_args = ['-Wno-parentheses']

# Include directories
inc_dirs = include_directories(
  'src/common',
  'src/backend',
  'src/engine'
)

# SFML libraries (link statically or dynamically depending on install)
sfml_libs = [
  cc.find_library('sfml-graphics', dirs : ['/usr/lib/sfml', '/usr/local/lib/sfml'], required: true),
  cc.find_library('sfml-window',   dirs : ['/usr/lib/sfml', '/usr/local/lib/sfml'], required: true),
  cc.find_library('sfml-system',   dirs : ['/usr/lib/sfml', '/usr/local/lib/sfml'], required: true),
]

common_sources = [
  'src/backend/GameState.cpp',
  'src/backend/Move.cpp',
  'src/backend/bit_tools.cpp',
  'src/engine/Engine.cpp',
  'src/game/cli/Cli.cpp',
  'src/game/gui/Gui.cpp',
  'src/game/gui/PromotionMenu.cpp',
]

# -------------------- EXECUTABLE: main --------------------
main_exe = executable(
  'main',
  [ 'src/main.cpp', common_sources ],
  include_directories : inc_dirs,
  cpp_args : cxx_args,
  dependencies : sfml_libs,
  install : false
)

# -------------------- EXECUTABLE: test/test --------------------
test_exe = executable(
  'test_test',  # Meson cannot output nested paths as names
  [ 'src/test/test.cpp', common_sources ],
  include_directories : inc_dirs,
  cpp_args : cxx_args,
  dependencies : sfml_libs,
  install : false
)

