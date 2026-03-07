# Changelog

Note: This only applies to the UCI engine; no frontend changes are recorded here

## v1 - commit df3555dee84c32a0e41019573842ec603c0c7355
Features:
- Basic UCI compliance
- Minimax search w/ alpha-beta pruning and quiescence search
- MVV-LVA move ordering + promotion bonus
- Endgame and non-endgame PSQTs and king distance heuristic for evaluation
- Basic 96Mb transposition table with "always-replace" eviction scheme
- Polyglot opening book compliance
- Naive repetition table using std::unordered_set
