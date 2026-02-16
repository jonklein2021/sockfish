# Sockfish

## Dependencies
- C++17 Compiler
- Meson
- SFML 3+

## Build and Run Instructions
```
meson setup build
cd build
meson compile
./build.sh
```

## SPRT Testing Instructions
```
./cutechess-cli -engine cmd=../../sockfish_baseline/build/sockfish -engine cmd=../../sockfish/build/sockfish -each proto=uci tc=10+0.10 -rounds 1 -debug
```

## Credits
- Stockfish: https://github.com/official-stockfish/Stockfish
- Cutechess: https://github.com/cutechess/cutechess
- Maksim Korzh's tutorials: https://github.com/maksimKorzh/chess_programming
- Raphael: https://github.com/Orbital-Web/Raphael
- Pieces: https://github.com/lichess-org/lila/tree/master/public/piece
