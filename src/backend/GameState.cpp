#include "GameState.h"

GameState::GameState() : GameState(defaultFEN) {}

GameState::GameState(const std::string &fen) {
    // init metadata
    md = {
        0, // castleRights
        0, // enPassantSquare
        0, // movesSinceCapture
        {} // history (not used yet)
    };

    const size_t n = fen.size();
    
    // 1: position data
    size_t i = 0;
    int x = 0, y = 0;
    for (; i < n && fen[i] != ' '; i++) {
        const char c = fen[i];
        if (c == '/') { // move to next row
            x = 0;
            y++;
        } else if (isdigit(c)) { // empty square; skip x squares
            x += c - '0';
        } else { // piece
            const PieceType pt = fenPieceMap.at(c);
            const uint64_t pieceBit = coordsToBit(x, y);
            
            // set this piece's bit in its bitboard
            pieceBits[pt] |= pieceBit;

            // update this piece's attack bitboard
            pieceAttacks[pt] |= computePieceAttacks(pt, pieceBit);
            x++;
        }
    }

    // update occupancies bitboards
    occupancies[WHITE] = pieceBits[WP] | pieceBits[WN] | pieceBits[WB] | pieceBits[WR] | pieceBits[WQ] | pieceBits[WK];
    occupancies[BLACK] = pieceBits[BP] | pieceBits[BN] | pieceBits[BB] | pieceBits[BR] | pieceBits[BQ] | pieceBits[BK];
    occupancies[BOTH] = occupancies[WHITE] | occupancies[BLACK];
    occupancies[NONE] = ~occupancies[BOTH];

    if (i >= n) return;

    // 2: whose turn it is
    whiteToMove = fen[++i] == 'w';

    // 3: castling rights (castleRights = 0b0000qkQK)
    i += 2;
    for (; fen[i] != ' ' && i < n; i++) {
        if (fen[i] == 'K') md.castleRights |= 0b0001;
        if (fen[i] == 'Q') md.castleRights |= 0b0010;
        if (fen[i] == 'k') md.castleRights |= 0b0100;
        if (fen[i] == 'q') md.castleRights |= 0b1000;
    }

    if (i >= n) return;

    // 4: en passant square
    if (fen[++i] != '-') {
        int file = fen[i] - 'a';
        int rank = '8' - fen[i + 1];
        md.enPassantBit = 1ull << (rank * 8 + file);
    }

    if (i >= n) return;

    // 5: halfmove clock
    i += 2;
    std::string halfmoveClock = "";
    for (; i < n && fen[i] != ' '; i++) {
        halfmoveClock += fen[i];
    }
    md.movesSinceCapture = halfmoveClock.empty() ? 0 : std::stoi(halfmoveClock);

    // 6: fullmove number (not used)

}

uint64_t GameState::computePieceAttacks(PieceType piece, uint64_t pieceBit) const {
    switch (piece) {
        case WP:
        case BP:
            return computePawnAttacks(pieceBit, piece == WP);
        case WN:
        case BN:
            return computeKnightAttacks(pieceBit);
        case WB:
        case BB:
            return computeBishopAttacks(pieceBit);
        case WR:
        case BR:
            return computeRookAttacks(pieceBit);
        case WQ:
        case BQ:
            return computeQueenAttacks(pieceBit);
        case WK:
        case BK:
            return computeKingAttacks(pieceBit);
        default:
            return 0;
    }
}

uint64_t GameState::computePawnAttacks(const uint64_t squareBit, const bool white) const {
    if (white) {
        return ((squareBit >> 7) & not_file_a) | ((squareBit >> 9) & not_file_h); // up right, up left
    } else {
        return ((squareBit << 7) & not_file_h) | ((squareBit << 9) & not_file_a); // down left, down right
    }
}

uint64_t GameState::computeKnightAttacks(const uint64_t squareBit) const {
    const uint64_t ddl = (squareBit & not_rank_12 & not_file_a) << 15; // down 2, left 1
    const uint64_t ddr = (squareBit & not_rank_12 & not_file_h) << 17; // down 2, right 1
    const uint64_t drr = (squareBit & not_rank_1 & not_file_gh) << 10; // down 1, right 2
    const uint64_t dll = (squareBit & not_rank_1 & not_file_ab) << 6; // down 1, left 2
    const uint64_t uur = (squareBit & not_rank_78 & not_file_h) >> 15; // up 2, right 1
    const uint64_t uul = (squareBit & not_rank_78 & not_file_a) >> 17; // up 2, left 1
    const uint64_t ull = (squareBit & not_rank_8 & not_file_ab) >> 10; // up 1, left 2
    const uint64_t urr = (squareBit & not_rank_8 & not_file_gh) >> 6; //  up 1, right 2
    
    return ddl | ddr | drr | dll | uur | uul | ull | urr;
}

uint64_t GameState::computeBishopAttacks(const uint64_t squareBit) const {
    uint64_t attacks = 0;
    uint64_t mask = squareBit;
    uint64_t filteredMask;
    
    // bitboard representing other pieces on the board
    const uint64_t otherPieces = occupancies[BOTH] & ~squareBit;
    
    // up right
    while (filteredMask = (mask & not_rank_8 & not_file_h & ~otherPieces)) {
        mask = filteredMask >> 7;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // up left
    while (filteredMask = (mask & not_rank_8 & not_file_a & ~otherPieces)) {
        mask = filteredMask >> 9;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // down right
    while (filteredMask = (mask & not_rank_1 & not_file_h & ~otherPieces)) {
        mask = filteredMask << 9;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // down left
    while (filteredMask = (mask & not_rank_1 & not_file_a & ~otherPieces)) {
        mask = filteredMask << 7;
        attacks |= mask;
    }
    
    return attacks;
}

uint64_t GameState::computeRookAttacks(const uint64_t squareBit) const {
    uint64_t attacks = 0;
    uint64_t mask = squareBit;
    uint64_t filteredMask;
    
    // bitboard representing other pieces on the board
    const uint64_t otherPieces = occupancies[BOTH] & ~squareBit;
    
    // up
    while (filteredMask = (mask & not_rank_8 & ~otherPieces)) {
        mask = filteredMask >> 8;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // down
    while (filteredMask = (mask & not_rank_1 & ~otherPieces)) {
        mask = filteredMask << 8;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // left
    while (filteredMask = (mask & not_file_a & ~otherPieces)) {
        mask = filteredMask >> 1;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // right
    while (filteredMask = (mask & not_file_h & ~otherPieces)) {
        mask = filteredMask << 1;
        attacks |= mask;
    }
    
    return attacks;
}

uint64_t GameState::computeQueenAttacks(const uint64_t squareBit) const {
    return computeBishopAttacks(squareBit) | computeRookAttacks(squareBit); // heheh
}

uint64_t GameState::computeKingAttacks(const uint64_t squareBit) const {
    const uint64_t d = (squareBit & not_rank_1) << 8; // down
    const uint64_t u = (squareBit & not_rank_8) >> 8; // up
    const uint64_t l = (squareBit & not_file_a) >> 1; // left
    const uint64_t r = (squareBit & not_file_h) << 1; // right
    const uint64_t dl = (squareBit & not_rank_1 & not_file_a) << 7; // down left
    const uint64_t dr = (squareBit & not_rank_1 & not_file_h) << 9; // down right
    const uint64_t ul = (squareBit & not_rank_8 & not_file_a) >> 9; // up left
    const uint64_t ur = (squareBit & not_rank_8 & not_file_h) >> 7; // up right
    
    return d | u | l | r | dl | dr | ul | ur;
}

PieceType GameState::getCapturedPiece(const uint64_t toBit, const std::vector<PieceType> &oppPieces) const {
    PieceType capturedPiece = None;
    for (PieceType p : oppPieces) {
        if (pieceBits[p] & toBit) {
            capturedPiece = p;
            break;
        }
    }
    return capturedPiece;
}

PieceType GameState::pieceAt(sf::Vector2<int> square) const {
    uint64_t targetBit = coordsToBit(square);
    for (int i = 0; i < 12; i++) {
        if (pieceBits[i] & targetBit) {
            return static_cast<PieceType>(i);
        }
    }
    return None;
}

Metadata GameState::makeMove(const Move &move) {
    // save metadata about this state before making move
    Metadata oldMD = md;

    /*** BITBOARD CHANGES ***/

    // useful constants
    const uint64_t from = coordsToBit(move.from);
    const uint64_t to = coordsToBit(move.to);
    const uint64_t fromTo = from | to;
    
    // "move" the bit of the piece's old location to its new location
    pieceBits[move.piece] ^= fromTo;
    
    // handle additional rook movement for castling
    if (move.piece == WK && move.isKCastle) {
        // white kingside castle
        pieceBits[WR] ^= (1ull << h1) | (1ull << f1);
    } else if (move.piece == BK && move.isKCastle) {
        // black kingside castle
        pieceBits[BR] ^= (1ull << h8) | (1ull << f8);
    } else if (move.piece == WK && move.isQCastle) {
        // white queenside castle
        pieceBits[WR] ^= (1ull << a1) | (1ull << d1);
    } else if (move.piece == BK && move.isQCastle) {
        // black kingside castle
        pieceBits[BR] ^= (1ull << a8) | (1ull << d8);
    }

    // handle captures
    if (move.capturedPiece != None) {
        if (move.isEnPassant) {
            // the piece to remove is at the attacker pawn's new x and old y
            uint64_t removeBit = coordsToBit(move.to.x, move.from.y);
            
            // remove the captured pawn
            pieceBits[move.piece == WP ? BP : WP] &= ~removeBit;
        } else {
            // determine what piece to remove
            for (PieceType p : {WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK}) {
                if (p != move.piece && (pieceBits[p] & to)) {
                    // zero out the captured piece's bit
                    pieceBits[p] &= ~to;
                    break;
                }
            }
        }
    }

    // handle pawn promotion
    if (move.promotionPiece != None) {
        pieceBits[move.piece] &= ~to; // remove pawn
        pieceBits[move.promotionPiece] |= to; // add promoted piece
    }

    // update attack squares
    // TODO: make this more efficient
    for (int i = 0; i < 12; i++) {
        pieceAttacks[i] = computePieceAttacks(static_cast<PieceType>(i), pieceBits[i]);
    }

    // update occupancies bitboards
    occupancies[WHITE] = pieceBits[WP] | pieceBits[WN] | pieceBits[WB] | pieceBits[WR] | pieceBits[WQ] | pieceBits[WK];
    occupancies[BLACK] = pieceBits[BP] | pieceBits[BN] | pieceBits[BB] | pieceBits[BR] | pieceBits[BQ] | pieceBits[BK];
    occupancies[BOTH] = occupancies[WHITE] | occupancies[BLACK];
    occupancies[NONE] = ~occupancies[BOTH];

    /*** METADATA CHANGES ***/

    // covers standard king moves and castling
    if (whiteToMove) {
        if (move.piece == WK) {
            // white cannot castle to either side
            md.castleRights &= ~0b0011;
        }
    } else {
        if (move.piece == BK) {
            // black cannot castle to either side
            md.castleRights &= ~0b1100;
        }
    }
    
    // prevent black queenside castle
    if ((move.to.x == 0 && move.to.y == 0) ||
        (move.piece == BR && move.from.x == 0))
      md.castleRights &= ~0b1000;
    
    // prevent black kingside castle
    if ((move.to.x == 7 && move.to.y == 0) ||
        (move.piece == BR && move.from.x == 7))
      md.castleRights &= ~0b0100;
    
    // prevent white queenside castle
    if ((move.to.x == 0 && move.to.y == 7) ||
        (move.piece == WR && move.from.x == 0))
      md.castleRights &= ~0b0010;
    
    // prevent white kingside castle
    if ((move.to.x == 7 && move.to.y == 7) ||
        (move.piece == WR && move.from.x == 7))
      md.castleRights &= ~0b0001;

    // update en passant square if pawn has moved two squares
    if (move.piece == WP && move.from.y == 6 && move.to.y == 4)
        md.enPassantBit = coordsToBit(move.from.x, 5); // {x, 5}
    else if (move.piece == BP && move.from.y == 1 && move.to.y == 3)
        md.enPassantBit = coordsToBit(move.from.x, 2); // {x, 2}
    else
        md.enPassantBit = 0;

    // update 50 move rule
    if (move.capturedPiece != None)
        md.movesSinceCapture = 0;
    else
        md.movesSinceCapture++;

    // change turns
    whiteToMove = !whiteToMove;
    
    return oldMD;
}

void GameState::unmakeMove(const Move &move, const Metadata &prevMD) {
    /* BITBOARD RESTORATION */

    // useful constants
    const uint64_t from = coordsToBit(move.from);
    const uint64_t to = coordsToBit(move.to);
    const uint64_t fromTo = from | to;

    // "Move" this piece's bit to its original position
    pieceBits[move.piece] ^= fromTo;

    // restore rook position if castling
    if (move.piece == WK && move.isKCastle) {
        // white kingside castle
        pieceBits[WR] ^= (1ull << h1) | (1ull << f1);
    } else if (move.piece == BK && move.isKCastle) {
        // black kingside castle
        pieceBits[BR] ^= (1ull << h8) | (1ull << f8);
    } else if (move.piece == WK && move.isQCastle) {
        // white queenside castle
        pieceBits[WR] ^= (1ull << a1) | (1ull << d1);
    } else if (move.piece == BK && move.isQCastle) {
        // black kingside castle
        pieceBits[BR] ^= (1ull << a8) | (1ull << d8);
    }

    // restored captures
    if (move.capturedPiece != None) {
        if (move.isEnPassant) {
            uint64_t restoreBit = coordsToBit(move.to.x, move.from.y);
            pieceBits[move.piece == WP ? BP : WP] |= restoreBit;
        } else {
            pieceBits[move.capturedPiece] |= to;  // restore captured piece
        }
    }

    // undo pawn promotion
    if (move.promotionPiece != None) {
        pieceBits[move.piece] |= from;  // restore original pawn
        pieceBits[move.piece] &= ~to;  // compensate for pieceBits[move.piece] ^= fromTo;
        pieceBits[move.promotionPiece] &= ~to;  // remove promoted piece
    }

    // restore attack squares
    // TODO: make this more efficient
    for (int i = 0; i < 12; i++) {
        pieceAttacks[i] = computePieceAttacks(static_cast<PieceType>(i), pieceBits[i]);
    }
    
    // update occupancies
    occupancies[WHITE] = pieceBits[WP] | pieceBits[WN] | pieceBits[WB] | pieceBits[WR] | pieceBits[WQ] | pieceBits[WK];
    occupancies[BLACK] = pieceBits[BP] | pieceBits[BN] | pieceBits[BB] | pieceBits[BR] | pieceBits[BQ] | pieceBits[BK];
    occupancies[BOTH] = occupancies[WHITE] | occupancies[BLACK];
    occupancies[NONE] = ~occupancies[BOTH];

    /* METADATA RESTORATION */

    // restore turn order
    whiteToMove = !whiteToMove;

    // restore metadata
    md = prevMD;
}

bool GameState::underAttack(const sf::Vector2<int> &square, const bool white) const {
    const int from = white ? 0 : 6;
    const int to = from + 6;
    for (int i = from; i < to; i++) {
        if (pieceAttacks[i] & coordsToBit(square)) {
            return true;
        }
    }
    return false;
}

bool GameState::underAttack(const sf::Vector2<int> &square) const {
    return underAttack(square, !whiteToMove);
}

bool GameState::isTerminal() const {
    // TODO: add threefold repetition check and make this more efficient
    const int n = md.history.size();
    return (
        md.movesSinceCapture >= 100 ||
        (n >= 9 && md.history[n-1] == md.history[n-5] && md.history[n-5] == md.history[n-9]) ||
        // board.insufficientMaterial() ||
        generateMoves().empty()
    );
}

bool GameState::isCheck() const {
    // get position of king
    const uint64_t king = pieceBits[whiteToMove ? WK : BK];

    // check if king is under attack
    return underAttack(bitToCoords(king));
}

std::vector<Move> GameState::generateMoves() const {
    std::vector<Move> legalMoves;
    std::vector<Move> moves;
    
    // used to check if a move is legal
    GameState copy(*this);

    // stores a copy of the bitboard of a selected piece
    uint64_t pieceBitboard;

    // bit that signifies a piece's initial location
    uint64_t fromBit;

    // bit that signifies a piece's final location
    uint64_t toBit;

    PieceType capturedPiece;

    // king position
    sf::Vector2<int> kingPos;
    
    std::vector<PieceType> promotionPieces;
    std::vector<PieceType> oppPieces;
    PieceType PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, OPP_PAWN;
    int SIDE;
    int OPP;
    if (whiteToMove) {
        promotionPieces = {WQ, WR, WB, WN};
        oppPieces = {BP, BN, BB, BR, BQ, BK};
        PAWN = WP;
        KNIGHT = WN;
        BISHOP = WB;
        ROOK = WR;
        QUEEN = WQ;
        KING = WK;
        OPP_PAWN = BP;
        SIDE = WHITE;
        OPP = BLACK;
    } else {
        promotionPieces = {BQ, BR, BB, BN};
        oppPieces = {WP, WN, WB, WR, WQ, WK};
        PAWN = BP;
        KNIGHT = BN;
        BISHOP = BB;
        ROOK = BR;
        QUEEN = BQ;
        KING = BK;
        OPP_PAWN = WP;
        SIDE = BLACK;
        OPP = WHITE;
    }

    /* PAWN MOVES */ // TODO: add promotion checks
    pieceBitboard = pieceBits[PAWN];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        // single pawn moves
        toBit = whiteToMove ? (fromBit >> 8) : (fromBit << 8);
        if (occupancies[NONE] & toBit) {
            const sf::Vector2<int> to = bitToCoords(toBit);
            moves.push_back({from, to, PAWN});
        }
        
        // double pawn moves
        toBit = whiteToMove ? (fromBit >> 16) : (fromBit << 16);
        uint64_t overBit = whiteToMove ? (fromBit >> 8) : (fromBit << 8);
        if (((whiteToMove && (rank2 & fromBit)) || (rank7 & fromBit)) && (occupancies[NONE] & overBit) && (occupancies[NONE] & toBit)) {
            const sf::Vector2<int> to = bitToCoords(toBit);
            moves.push_back({from, to, PAWN});
        }
        
        // pawn captures right
        toBit = (whiteToMove ? (fromBit >> 7) : (fromBit << 9)) & not_file_h;
        if (toBit & (occupancies[OPP] | md.enPassantBit)) {
            const sf::Vector2<int> to = bitToCoords(toBit);
            PieceType capturedPiece = None;
            for (PieceType p : oppPieces) {
                if (pieceBits[p] & toBit) {
                    moves.push_back({from, to, PAWN, p});
                    break;
                }
            }

            // must be en passant
            if (capturedPiece == None) {
                moves.push_back({from, to, PAWN, OPP_PAWN, None, false, false, true});
            }
        }
        
        // pawn captures (up and) left
        toBit = (fromBit >> 9) & not_file_a;
        if (toBit & (occupancies[OPP] | md.enPassantBit)) {
            const sf::Vector2<int> to = bitToCoords(toBit);
            
            // determine what piece was captured
            PieceType capturedPiece = getCapturedPiece(toBit, oppPieces);

            // must be en passant
            if (capturedPiece == None) {
                moves.push_back({from, to, WP, BP, None, false, false, true});
            }
        }

        pieceBitboard &= pieceBitboard - 1;
    }
    
    /* KNIGHT MOVES */
    pieceBitboard = pieceBits[KNIGHT];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        // down 2, left 1
        if ((toBit = ((fromBit & not_rank_12 & not_file_a) << 15)) && !(toBit & occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), KNIGHT, capturedPiece});
        }
        
        // down 2, right 1
        if ((toBit = ((fromBit & not_rank_12 & not_file_h) << 17)) && !(toBit & occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), KNIGHT, capturedPiece});
        }

        // down 1, right 2
        if ((toBit = ((fromBit & not_rank_1 & not_file_gh) << 10)) && !(toBit & occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), KNIGHT, capturedPiece});
        }

        // down 1, left 2
        if ((toBit = ((fromBit & not_rank_1 & not_file_ab) << 6)) && !(toBit & occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), KNIGHT, capturedPiece});
        }

        // up 2, right 1
        if ((toBit = ((fromBit & not_rank_78 & not_file_h) >> 15)) && !(toBit & occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), KNIGHT, capturedPiece});
        }

        // up 2, left 1
        if ((toBit = ((fromBit & not_rank_78 & not_file_a) >> 17)) && !(toBit & occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), KNIGHT, capturedPiece});
        }

        // up 1, left 2
        if ((toBit = ((fromBit & not_rank_8 & not_file_ab) >> 10)) && !(toBit & occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), KNIGHT, capturedPiece});
        }

        // up 1, right 2
        if ((toBit = ((fromBit & not_rank_8 & not_file_gh) >> 6)) && !(toBit & occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), KNIGHT, capturedPiece});
        }

        pieceBitboard &= pieceBitboard - 1;
    }


    /* BISHOP MOVES */
    pieceBitboard = pieceBits[BISHOP];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        toBit = fromBit;
        uint64_t filteredMask;

        // up right
        while (filteredMask = (toBit & not_rank_8 & not_file_h & ~occupancies[SIDE])) {
            toBit = filteredMask >> 7;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), BISHOP, capturedPiece});
        }
        
        toBit = fromBit;
        
        // up left
        while (filteredMask = (toBit & not_rank_8 & not_file_a & ~occupancies[SIDE])) {
            toBit = filteredMask >> 9;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), BISHOP, capturedPiece});
        }
        
        toBit = fromBit;
        
        // down right
        while (filteredMask = (toBit & not_rank_1 & not_file_h & ~occupancies[SIDE])) {
            toBit = filteredMask << 9;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), BISHOP, capturedPiece});
        }
        
        toBit = fromBit;
        
        // down left
        while (filteredMask = (toBit & not_rank_1 & not_file_a & ~occupancies[SIDE])) {
            toBit = filteredMask << 7;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), BISHOP, capturedPiece});
        }

        pieceBitboard &= pieceBitboard - 1;
    }

    /* ROOK MOVES */
    pieceBitboard = pieceBits[ROOK];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        toBit = fromBit;
        uint64_t filteredMask;

        // up
        while (filteredMask = (toBit & not_rank_8 & ~occupancies[SIDE])) {
            toBit = filteredMask >> 8;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), ROOK, capturedPiece});
        }
        
        toBit = fromBit;
        
        // down
        while (filteredMask = (toBit & not_rank_1 & ~occupancies[SIDE])) {
            toBit = filteredMask << 8;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), ROOK, capturedPiece});
        }
        
        toBit = fromBit;
        
        // left
        while (filteredMask = (toBit & not_file_a & ~occupancies[SIDE])) {
            toBit = filteredMask >> 1;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), ROOK, capturedPiece});
        }
        
        toBit = fromBit;
        
        // right
        while (filteredMask = (toBit & not_file_h & ~occupancies[SIDE])) {
            toBit = filteredMask << 1;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), ROOK, capturedPiece});
        }
        
        pieceBitboard &= pieceBitboard - 1;
    }

    /* QUEEN MOVES */
    pieceBitboard = pieceBits[QUEEN];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        toBit = fromBit;
        uint64_t filteredMask;

        // up
        while (filteredMask = (toBit & not_rank_8 & ~occupancies[SIDE])) {
            toBit = filteredMask >> 8;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), QUEEN, capturedPiece});
        }
        
        toBit = fromBit;
        
        // down
        while (filteredMask = (toBit & not_rank_1 & ~occupancies[SIDE])) {
            toBit = filteredMask << 8;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), QUEEN, capturedPiece});
        }
        
        toBit = fromBit;
        
        // left
        while (filteredMask = (toBit & not_file_a & ~occupancies[SIDE])) {
            toBit = filteredMask >> 1;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), QUEEN, capturedPiece});
        }
        
        toBit = fromBit;
        
        // right
        while (filteredMask = (toBit & not_file_h & ~occupancies[SIDE])) {
            toBit = filteredMask << 1;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), QUEEN, capturedPiece});
        }

        toBit = fromBit;

        // up right
        while (filteredMask = (toBit & not_rank_8 & not_file_h & ~occupancies[SIDE])) {
            toBit = filteredMask >> 7;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), QUEEN, capturedPiece});
        }
        
        toBit = fromBit;
        
        // up left
        while (filteredMask = (toBit & not_rank_8 & not_file_a & ~occupancies[SIDE])) {
            toBit = filteredMask >> 9;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), QUEEN, capturedPiece});
        }
        
        toBit = fromBit;
        
        // down right
        while (filteredMask = (toBit & not_rank_1 & not_file_h & ~occupancies[SIDE])) {
            toBit = filteredMask << 9;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), QUEEN, capturedPiece});
        }
        
        toBit = fromBit;
        
        // down left
        while (filteredMask = (toBit & not_rank_1 & not_file_a & ~occupancies[SIDE])) {
            toBit = filteredMask << 7;
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            moves.push_back({from, bitToCoords(toBit), QUEEN, capturedPiece});
        }
        
        pieceBitboard &= pieceBitboard - 1;
    }

    /* KING MOVES */
    pieceBitboard = pieceBits[KING];
    fromBit = 1ull << indexOfLs1b(pieceBitboard);
    const sf::Vector2<int> from = bitToCoords(fromBit);
    kingPos = from;

    // standard moves

    if (((toBit = ((fromBit & not_rank_1) << 8)) != 0) && !(toBit & occupancies[SIDE])) { // down
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        moves.push_back({from, bitToCoords(toBit), KING, capturedPiece});
    }

    if (((toBit = ((fromBit & not_rank_8) >> 8))) != 0 && !(toBit & occupancies[SIDE])) { // up
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        moves.push_back({from, bitToCoords(toBit), KING, capturedPiece});
    }

    if (((toBit = ((fromBit & not_file_a) >> 1)) != 0) && !(toBit & occupancies[SIDE])) { // left
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        moves.push_back({from, bitToCoords(toBit), KING, capturedPiece});            
    }

    if (((toBit = ((fromBit & not_file_h) << 1)) != 0) && !(toBit & occupancies[SIDE])) { // right
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        moves.push_back({from, bitToCoords(toBit), KING, capturedPiece});            
    }

    if (((toBit = ((fromBit & not_rank_1 & not_file_a) << 7)) != 0) && !(toBit & occupancies[SIDE])) { // down left
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        moves.push_back({from, bitToCoords(toBit), KING, capturedPiece});            
    }

    if (((toBit = ((fromBit & not_rank_1 & not_file_h) << 9)) != 0) && !(toBit & occupancies[SIDE])) { // down right
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        moves.push_back({from, bitToCoords(toBit), KING, capturedPiece});            
    }

    if (((toBit = ((fromBit & not_rank_8 & not_file_a) >> 9)) != 0) && !(toBit & occupancies[SIDE])) { // up left
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        moves.push_back({from, bitToCoords(toBit), KING, capturedPiece});            
    }

    if (((toBit = ((fromBit & not_rank_8 & not_file_h) >> 7)) != 0) && !(toBit & occupancies[SIDE])) { // up right
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        moves.push_back({from, bitToCoords(toBit), KING, capturedPiece});            
    }

    // castling
    
    // white kingside castle
    if (whiteToMove &&
        (md.castleRights & 0b0001) &&
        (occupancies[NONE] & (1ull << f1)) &&
        (occupancies[NONE] & (1ull << g1)) &&
        !underAttack(from) && // {4, 7}
        !underAttack({5, 7}) &&
        !underAttack({6, 7})
    ) {
        moves.push_back({from, {6, 7}, KING, None, None, true, false});
    }

    // white queenside castle
    if (whiteToMove &&
        (md.castleRights & 0b0010) &&
        (occupancies[NONE] & (1ull << d1)) &&
        (occupancies[NONE] & (1ull << c1)) &&
        (occupancies[NONE] & (1ull << b1)) &&
        !underAttack(from) && // {4, 7}
        !underAttack({3, 7}) &&
        !underAttack({2, 7})
    ) {
        moves.push_back({from, {2, 7}, KING, None, None, false, true});
    }

    // black kingside castle
    if (!whiteToMove &&
        (md.castleRights & 0b0100) &&
        (occupancies[NONE] & (1ull << f8)) &&
        (occupancies[NONE] & (1ull << g8)) &&
        !underAttack(from) && // {4, 0}
        !underAttack({5, 0}) &&
        !underAttack({6, 0})
    ) {
        moves.push_back({from, {6, 0}, KING, None, None, true, false});
    }

    // black queenside castle
    if (!whiteToMove &&
        (md.castleRights & 0b1000) &&
        (occupancies[NONE] & (1ull << d1)) &&
        (occupancies[NONE] & (1ull << c1)) &&
        (occupancies[NONE] & (1ull << b1)) &&
        !underAttack(from) && // {4, 0}
        !underAttack({3, 0}) &&
        !underAttack({2, 0})
    ) {
        moves.push_back({from, {2, 0}, KING, None, None, false, true});
    }
    
    // filter out moves that put the king in check
    for (auto it = moves.begin(); it != moves.end(); it++) {
        Metadata md = copy.makeMove(*it);
        if (!copy.underAttack(kingPos, !whiteToMove)) {
            legalMoves.push_back(*it);
        }
        copy.unmakeMove(*it, md);
    }

    return legalMoves;
}

uint64_t GameState::hash() const {
    return (
        pieceBits[0] ^
        pieceBits[1] ^
        pieceBits[2] ^
        pieceBits[3] ^
        pieceBits[4] ^
        pieceBits[5] ^
        pieceBits[6] ^
        pieceBits[7] ^
        pieceBits[8] ^
        pieceBits[9] ^
        pieceBits[10] ^
        pieceBits[11] ^
        (whiteToMove << 1) ^
        (md.castleRights << 2) ^
        (md.enPassantBit << 3)
    );
    // N.B: movesSinceCapture not included so that transpositions hash to the same value
}

void GameState::print() const {
    prettyPrintPosition(pieceBits, true);
    std::cout << "whiteToMove = " << whiteToMove << std::endl;
    std::cout << "castleRights = " << (
        md.castleRights == 0 ? "-" :
        (
            std::string((md.castleRights & 0b0001) ? "K" : "") +
            std::string((md.castleRights & 0b0010) ? "Q" : "") +
            std::string((md.castleRights & 0b0100) ? "k" : "") +
            std::string((md.castleRights & 0b1000) ? "q" : "")
        )
    ) << std::endl;
    std::cout << "enPassantSquare = " << bitToCoords(md.enPassantBit).x << ", " << bitToCoords(md.enPassantBit).y << std::endl;
    std::cout << "movesSinceCapture = " << md.movesSinceCapture << std::endl;
}