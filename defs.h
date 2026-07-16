#ifndef DEFS_H
#define DEFS_H

#define NDEBUG  // uncomment to disable assertion checks 

#include <stdlib.h>
#include <assert.h>

#ifndef DEBUG 
//#define DEBUG 
#endif


typedef unsigned long long U64; // using a bits for board representation (64 bits = 64 squares)

#define NAME "Helios 1.0"
#define BOARD_SQR_NUM 120

#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define INF_BOUND 32000
#define AB_BOUND 30000
#define ISMATE (AB_BOUND - MAXDEPTH)

enum { EMPTY, wP, wH, wB, wR, wQ, wK, bP, bH, bB, bR, bQ, bK }; 
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum { WHITE, BLACK, BOTH };
enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1, 
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 }; // castling (format: colour - side being castled) represented by bits

///////// Structures ////////////

typedef struct {
    int move;
    int score;
} S_MOVE;

typedef struct {
    S_MOVE moves [MAXPOSITIONMOVES];
    int count;
} S_MOVELIST;

enum { HFNONE, HFALPHA, HFBETA, HFEXACT};


// structure of the entry into the principle varaition table
typedef struct {
    U64 pos_key;
    int move;
    int score;
    int depth;
    int flags;
} S_HASHENTRY;

typedef struct {
    S_HASHENTRY *p_table;
    int num_entries;
    int new_write;
    int over_write;
    int hit;
    int cut;
} S_HASHTABLE;

// structure to keep record of the history of the chess game
typedef struct {
    int move;
    int castle_perm;
    int enPas;
    int fifty_move_count;
    U64 posKey;
} S_UNDO;

// structure for the chess board
typedef struct {
    int pieces[BOARD_SQR_NUM];
    U64 pawns[3]; // 2 for colors 1 for combined (acts like an array[64])

    int KingSq[2];

    int side; // which side is currently going
    int enPas; // keep track of en-passent square
    int fifty_move_count;

    int half_moves; // half moves for a positon (used for search)
    int hist_half_moves; // half  moves done for the entire game

    int castle_perm;

    U64 pos_key; // hashkey

    int piece_num[13]; // record of piece totals, piece determined by enum above
    int big_piece[2]; // non-pawn pieces for each colour
    int maj_piece[2]; // rooks and queens for each colour
    int min_piece[2]; // bishops and knights for each colour
    int material[2]; // material values for black and white

    S_UNDO history[MAXGAMEMOVES]; //array of all moves

    int piece_list[13][10]; // example of use [wH][0] = E1

    S_HASHTABLE hash_table[1];
    int PvArray[MAXDEPTH];

    int search_history[13][BOARD_SQR_NUM]; 
    int search_killers[2][MAXDEPTH]; // most recent beta cutoff move [0][depth] == best move [1][depth] == second best
    
} S_BOARD;

typedef struct {
    int starttime;
    int stoptime;
    int depth;
    int depthset;
    int timeset;
    int movestogo;
    int infinite;
    long nodes;
    int quit;
    int stopped;
    float fhf; // number of time we get beta cutoff on first move of the search
    float fh; // number of time we get beta cutoff in total during the search 
} S_SEARCHINFO;

///////// Game Move ////////////

// 7 bits: from, 7 bits: to, 4 bits: captured, 1 bit: enPas, 1 bit: Pawn Start, 4 bits: Promoted Piece, 1 bit: castle

#define FROMSQ(m) ((m) & 0x7F) // provides square moved from
#define TOSQ(m) (((m)>>7) & 0x7F) // provides squared moved to
#define CAPTURED(m) (((m)>>14) & 0xF) // provides captured piece
#define PROMOTED(m) (((m)>>20) & 0xF) // provides promoted piece

#define MFLAGEP 0x40000 // checks if there was an enPas
#define MFLAGPS 0x80000 // checks if there was a pawn start
#define MFLAGCASTLE 0x1000000 // checks if there was a castle

#define MFLAGCAP 0x7C000 // checks if any piece was captured
#define MFLAGPROM 0xF00000 // checks if any piece was promoted

#define NOMOVE 0

#define INFINITE 30000

///////// Macros ////////////

#define FR2SQ(f,r) ( (21 + (f)) + ((r) * 10)) // determines index in the 120 array based on file and rank
#define SQ64(sq120) (Sq120ToSq64[(sq120)])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define CLRBIT(bb,sq)  ((bb) &= ClearMask[(sq)])
#define SETBIT(bb,sq)  ((bb) |= SetMask[(sq)])
#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define IsKing(p) (PieceKing[(p)])
#define IsHorse(p) (PieceKnight[(p)])
#define MIRROR64(sq) (Mirror64[(sq)])

///////// Globals ////////////

extern int Sq120ToSq64[BOARD_SQR_NUM];
extern int Sq64ToSq120[64];
extern U64 SetMask[64];
extern U64 ClearMask[64];
extern U64 PieceKeys[13][120]; // key based on pieces
extern U64 SideKey; // key for side 
extern U64 CastleKey[16];// key for castle status
extern char PieceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];

extern int PieceBig[13];
extern int PieceMajor[13];
extern int PieceMinor[13];
extern int PieceValue[13];
extern int PieceColour[13];
extern int PiecePawn[13];

extern int FilesBrd[BOARD_SQR_NUM];
extern int RanksBrd[BOARD_SQR_NUM];

extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];
extern int PieceSlides[13];

extern int Mirror64[64];
extern U64 FileBBMask[8];
extern U64 RankBBMask[8];
extern U64 BlackPassedMask[64];
extern U64 WhitePassedMask[64];
extern U64 IsolatedMask[64];

///////// Functions ////////////

// init.c
extern void init();

// bitboards.c
extern void PrintBitBoard(U64 bb);
extern int PopBit(U64 *bb);
extern int CountBits(U64 b);

// hashkeys.c
extern U64 GeneratePositionKey (const S_BOARD *pos);

// board.c
extern void ResetBoard(S_BOARD * pos);
extern int ParseFen (char *fen, S_BOARD *pos);
extern void PrintBoard(const S_BOARD *pos);
extern void UpdateListsMaterials (S_BOARD *pos);
extern int CheckBoard(const S_BOARD *pos);
extern S_BOARD *GenBoard ();
extern void MirrorBoard(S_BOARD *pos);

// attack.c
extern int SqAttacked (const int sq, const int attack_side, const S_BOARD *pos);

// io.c
extern char *PrSq(const int sq);
extern char *PrMove (const int move);
extern void PrintMoveList(const S_MOVELIST *list);
extern int ParseMove(char *ptrChar, S_BOARD *pos);

// validate.c
extern int SqOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int piece);
extern int PieceValid(const int piece);

// movegen.c
extern void GenerateAllMoves (const S_BOARD *pos,  S_MOVELIST *list );
extern int MoveExists(S_BOARD *pos, const int move);
extern void InitMvvLva();
extern void GenerateAllCaps (const S_BOARD *pos,  S_MOVELIST *list );
extern void MirrorEvalTest(S_BOARD *pos);

// makemove.c
extern int MakeMove (S_BOARD *pos, int move);
extern void TakeMove(S_BOARD *pos);
extern void MakeNullMove(S_BOARD *pos);
extern void TakeNullMove(S_BOARD *pos);

// perft.c
extern void PerftTest(int depth, S_BOARD *pos);
extern void PerftFile(const int depth);

// search.c
extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table);

// misc.c
extern int GetTimeMs();
extern void ReadInput(S_SEARCHINFO *info);

// pvtable.c
extern void InitHashTable(S_HASHTABLE *table);
extern void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth);
extern int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth);
extern int GetPvLine(const int depth, S_BOARD *pos);
extern void ClearHashTable(S_HASHTABLE * table);
extern int ProbePvMove(const S_BOARD *pos);

// evaluate.c
extern int EvalPosition(const S_BOARD *pos);

// uci.c
extern void Uci_Loop();

#endif