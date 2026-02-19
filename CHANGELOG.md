# Changelog

Note: This only applies to the Sockfish UCI engine; no frontend changes are recorded here

## v1 - commit 16ce0d0c254615f1f21a171800774789440dcd11
Changes:
- Replace std::vector with array-based MoveList struct in move generation
- Loosen time management, allowing for more time to make a move

## v0 - commit 8662839775afc002305a465d3e9cb5901eb016a9
Features:
- Basic UCI compliance
- Minimax search w/ alpha-beta pruning and quiescence search
- MVV-LVA move ordering + promotion bonus
- Endgame and non-endgame PSQTs and king distance heuristic for evaluation
- Basic 96Mb transposition table with "always-replace" eviction scheme
- Polyglot opening book compliance
- Naive repetition table using std::unordered_set
