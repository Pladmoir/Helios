#include <stdio.h>
#include <stdbool.h>
#include "defs.h"

void UpdateListsMaterials (S_BOARD *pos) {
    int piece, sq, index, colour;

    for(index = 0; index < BOARD_SQR_NUM; ++index) {
        sq = index;
        piece = pos->pieces[index];
        if (piece != OFFBOARD && piece != EMPTY) {
            colour = PieceColour[piece];
            if (PieceBig[piece] == true) pos->big_piece[colour]++;
            if (PieceMajor[piece] == true) pos->maj_piece[colour]++;
            if (PieceMinor[piece] == true) pos->min_piece[colour]++;

            pos->material[colour] += PieceValue[piece];

            pos->piece_list[piece][pos->piece_num[piece]] = sq;;
            pos->piece_num[piece]++;

            if (piece == wK) pos->KingSq[WHITE] = sq;
            if (piece == bK) pos->KingSq[BLACK] = sq;
        }
    }
}

// decipher FEN notation into our board structure
int ParseFen (char *fen, S_BOARD *pos) {
    assert(fen != NULL);
    assert(pos != NULL);

    int rank = RANK_8;
    int file = FILE_A;
    int piece = 0;
    int count = 0;
    int i = 0;
    int sq64 = 0;
    int sq120 = 0;

    ResetBoard(pos);

    while ((rank >= RANK_1) && *fen) {
        count = 1;
        switch (*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bH; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wH; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;
            
            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;
            
            default:
                printf("FEN error \n");
                return -1;
        }

        for (i = 0; i < count; ++i) {
            sq64 = (rank * 8) + file;
            sq120 = SQ120(sq64);
            if (piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
            file++;
        }
        fen++;
    }

    assert(*fen == 'w' || *fen == 'b');
    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen += 2;

    for (i = 0; i < 4; ++i) {
        if (*fen == ' ') {
            break;
        }
        switch(*fen) {
            case 'K': pos->castle_perm |= WKCA; break;
            case 'Q': pos->castle_perm |= WQCA; break;
            case 'k': pos->castle_perm |= BKCA; break;
            case 'q': pos->castle_perm |= BQCA; break;
            default:    break;
        }
        fen++;
    }
    fen++;

    assert(pos->castle_perm >= 0 && pos->castle_perm <= 15);

    if (*fen != '-') {
        file = fen[0] - 'a';
        rank = fen[1] - '1';
        assert(file >= FILE_A && file <= FILE_H);
        assert(rank >= RANK_1 && rank <= RANK_8);

        pos->enPas = FR2SQ(file, rank);
    }

    pos->pos_key = GeneratePositionKey(pos);

    UpdateListsMaterials(pos);

    return 0;
}

// Reset the chess board
void ResetBoard(S_BOARD * pos) {
    assert(pos != NULL);
    int index = 0;

    for(index = 0; index < BOARD_SQR_NUM; ++index) {
        pos->pieces[index] = OFFBOARD;
    }

    for(index = 0; index < 64; ++index) {
        pos->pieces[SQ120(index)] = EMPTY;
    }

    for(index = 0; index < 2; ++index) {
        pos->big_piece[index] = 0;
        pos->maj_piece[index] = 0;
        pos->min_piece[index] = 0;
    }

     for(index = 0; index < 3; ++index) {
        pos->pawns[index] = 0ULL;
    }

    for(index = 0; index < 13; ++index) {
        pos->piece_num[index] = 0;
    }

    pos->KingSq[WHITE] = pos->KingSq[BLACK] = NO_SQ;
    pos->side = BOTH;
    pos->enPas = NO_SQ;
    pos->fifty_move_count = 0;
    pos->half_moves = 0;
    pos->hist_half_moves = 0;
    pos->castle_perm = 0;
    pos->pos_key = 0ULL;
}

void PrintBoard(const S_BOARD *pos) {
    int sq, file, rank, piece;

    printf("\nGame Board: \n \n");

    //print the pieces and rank
    for (rank = RANK_8; rank >= RANK_1; rank--) {
        printf("%d  ",rank + 1);
        for (file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file, rank);
            piece = pos->pieces[sq];
            printf("%3c",PieceChar[piece]);
        }
        printf("\n");
    }
    printf("\n   ");

    // print the file characters
     for (file = FILE_A; file <= FILE_H; file++) {
         printf("%3c",'a' + file);
    }
    printf("\n");
    printf("side:%c\n",SideChar[pos->side]);
    printf("enPas:%d\n",pos->enPas);
    printf("castle:%c%c%c%c\n",
            pos->castle_perm & WKCA? 'K': '-',
            pos->castle_perm & WQCA? 'Q': '-',
            pos->castle_perm & BKCA? 'k': '-',
            pos->castle_perm & BQCA? 'q': '-'
        );
    printf("PosKey:%llX\n", pos->pos_key);
}