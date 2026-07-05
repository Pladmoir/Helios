#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char fen[256];
    long target_leaf_n;
    long leafnodes;
    long nodes;
    long start_time;
    long end_time;
    long saved_nodes;
} S_PERFTRES;

void ResetResult(S_PERFTRES *res) {

    memset(&res->fen[0],256,0);
    res->target_leaf_n = 0;
    res->leafnodes = 0;
    res->nodes = 0;
    res->start_time = 0;
    res->end_time = 0;
    res->saved_nodes = 0;
}

void PerftGo(int depth, S_BOARD *pos, S_PERFTRES *res) {
    assert(CheckBoard(pos));

    if(depth == 0) {
        res->leafnodes++;
        return;
    }
    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
    int movenum = 0;
    for(movenum = 0; movenum < list->count; ++ movenum) {
        if(!MakeMove(pos,list->moves[movenum].move)) {
            continue;
        }
        PerftGo(depth - 1, pos, res);
        TakeMove(pos);
        res->nodes++;
    }
}

void PerftGoRoot(int depth, S_BOARD *pos, S_PERFTRES *res) {
    assert(CheckBoard(pos));


    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);

    int move;
    int MoveNum = 0;
    for(MoveNum = 0; MoveNum < list->count; ++ MoveNum) {
        move = list->moves[MoveNum].move;
        if( !MakeMove(pos,move)) {
            continue;
        }
        long cumnodes = res->leafnodes;
        PerftGo(depth - 1, pos,res);
        TakeMove(pos);
        long oldnodes = res->leafnodes - cumnodes;
        printf("move %d : %s : %1d\n", MoveNum + 1, PrMove(move), oldnodes);
    }

}


long leafnodes;

void PerftMulti (const int depth, S_BOARD *pos, S_PERFTRES *res) {
    printf("\n\n***** Perft Depth %d ******\n",depth);
    ParseFen(res->fen,pos);
    PerftGoRoot(depth, pos, res);
    printf("\nTotal Nodes: %1d\n", res->leafnodes);
}

void ParsePerftLine(char *line, S_PERFTRES *res, const int depth) {
    int i = 0;
    ResetResult(res);
    while(*line != ';') {
        res->fen[i++] = *line;
        line++;
    }

    res->fen[i] = '\0';

    while(*line) {
        if(*line == 'D') {
            line++;
            i = atoi(line);
            if(i == depth) {
                line += 2;
                res->target_leaf_n = atoi(line);
                printf("Fen: %s : Target :1d\n", res->fen, res->target_leaf_n);
                return;
            }
        }
        line++;
    }
}



void PerftFile(const int depth) {
    FILE *perft_file;
    char lineIn[1024];
    S_PERFTRES results[512];
    int rescount = 0;
    int index = 0;
    int index2 = 0;

    perft_file = fopen("perftsuite.epd","r");
    if(perft_file == NULL) {
        printf("File Not Found\n");
        return;
    } else {
        while(fgets (lineIn, 1024, perft_file) != NULL) {
            ParsePerftLine(lineIn, &results[rescount++], depth);
            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }

    S_BOARD board[1];
    printf("Start time: %s", __TIME__);

    if(rescount != 0) {
        printf("\n\nResults:\n\n");
        int success = false;
        for(index = 0; index < rescount; ++index) {
            PerftMulti(depth, board, &results[index]);
        }
        for(index2 = 0; index2 < rescount; ++index2) {
            success = results[index2].leafnodes == results[index2].target_leaf_n ? true : false;
            printf("%s : %s\n",(success)?"OK":"***Failed***",results[index2].fen);
        }
    }

    printf("End time: %s", __TIME__);
}

// singular perf test suite
void Perft(int depth, S_BOARD *pos) {
    assert(CheckBoard(pos));
    if(depth == 0) {
        leafnodes++;
        return;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int move_num = 0;
    for(move_num = 0; move_num < list->count; ++ move_num) {
        if(!MakeMove(pos,list->moves[move_num].move)) {
            continue;
        }
        Perft(depth - 1, pos);
        TakeMove(pos);
    }

    return;
}

void PerftTest(int depth, S_BOARD *pos) {
    assert(CheckBoard(pos));
    PrintBoard(pos);

    printf("\nStarting Test to Depth %d\n",depth);
    leafnodes = 0;
    int start = GetTimeMs();

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);

    int move;
    int MoveNum = 0;
    for(MoveNum = 0; MoveNum < list->count; ++ MoveNum) {
        move = list->moves[MoveNum].move;
        if( !MakeMove(pos,move)) {
            continue;
        }
        long cumnodes = leafnodes;
        Perft(depth - 1, pos);
        TakeMove(pos);
        long oldnodes = leafnodes - cumnodes;
        printf("move %d : %s : %1d\n", MoveNum + 1, PrMove(move), oldnodes);
    }
    printf("\nTest Complete: %1d nodes visited in %dms\n",leafnodes,GetTimeMs() - start);
}