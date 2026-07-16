#include <stdio.h>
#include "defs.h"
#include <string.h>
#include <stdbool.h>

#define INPUTBUFFER 400 * 6

// parse the go command from the uci input
void ParseGo(char *line, S_SEARCHINFO *info, S_BOARD *pos, S_HASHTABLE *table) {
    int depth = -1;
    int movestogo = 30;
    int movetime = -1;
    int time = -1;
    int inc = 0;
    char *ptr = NULL;
    info->timeset = false;

    if((ptr = strstr(line, "infinite"))) {
        ;
    }


    if(((ptr = strstr(line, "binc")) && pos->side == BLACK)) {
        inc = atoi(ptr + 5);
    }

    if(((ptr = strstr(line, "winc")) && pos->side == WHITE)) {
        inc = atoi(ptr + 5);
    }

    if(((ptr = strstr(line, "wtime")) && pos->side == WHITE)) {
        time = atoi(ptr + 6);
    }

    if(((ptr = strstr(line, "btime")) && pos->side == BLACK)) {
        time = atoi(ptr + 6);
    }

    if((ptr = strstr(line, "movestogo"))) {
        movestogo = atoi(ptr + 10);
    }
    
    if((ptr = strstr(line, "movetime"))) {
        movetime = atoi(ptr + 9);
    }

    if((ptr = strstr(line, "depth"))) {
        depth = atoi(ptr + 6);
    }

    if(movetime != -1) {
        time = movetime;
        movestogo = 1;
    }

    info->starttime = GetTimeMs();
    info->depth = depth;

    if(time != -1) {
        info->timeset = true;
        time /= movestogo;
        time -= 50; // make sure there is buffer so we don't go over time
        info->stoptime = info->starttime + time + inc;
    }

    if(depth == -1) {
        info->depth = MAXDEPTH;
    }
    printf("time:%d start:%d stop:%d depth:%d timeset:%d\n", time, info->starttime, info->stoptime, info->depth, info->timeset);
    SearchPosition(pos,info, table);
}

// parse the positionb given by the uci input
void ParsePosition(char *line_in, S_BOARD *pos) {
    line_in += 9;
    char *ptrchar = line_in;
    if(strncmp(line_in, "startpos", 8) == 0) {
        ParseFen(START_FEN, pos);
    } else {
        ptrchar = strstr(line_in, "fen");
        if(ptrchar == NULL) {
            ParseFen(START_FEN, pos);
        } else {
            ptrchar+=4;
            ParseFen(ptrchar, pos);
        }
    }

    ptrchar = strstr(line_in, "moves");
    int move;
    if(ptrchar != NULL) {
        ptrchar += 6;
        while(*ptrchar) {
            move = ParseMove(ptrchar, pos);
            if(move == NOMOVE) break;
            MakeMove(pos,move);
            pos->half_moves = 0;
            while(*ptrchar && *ptrchar!= ' ') ptrchar++;
            ptrchar++;
        }
    }
    PrintBoard(pos);
}

// main lopp of the UCI protocol
void Uci_Loop() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char line[INPUTBUFFER];
    printf("id name %s\n", NAME);
    printf("id author Tarunvir Randhawa\n");
    printf("uciok\n");

    S_BOARD *pos = GenBoard();
    S_SEARCHINFO info[1];
    S_HASHTABLE hash_table[1];
    InitHashTable(hash_table);

    while(1) {
        memset(&line[0], 0, sizeof(line)); // clear out line array
        fflush(stdout);
        if (!fgets(line, INPUTBUFFER, stdin)) {
            continue;
        }

        if (line[0] == '\n') {
            continue;
        }

        if (!strncmp(line, "isready", 7)) {
            printf("readyok\n");
            continue;
        } else if (!strncmp(line, "position", 8)) {
            ParsePosition(line, pos);
        } else if (!strncmp(line, "ucinewgame", 10)) {
            ClearHashTable(hash_table);
            ParsePosition("position startpos\n", pos);
        } else if (!strncmp(line, "go", 2)) {
            ParseGo(line, info, pos, hash_table);
        } else if (!strncmp(line, "quit", 4)) {
            info->quit = true;
            break;
        } else if (!strncmp(line, "uci", 3)) {
            printf("id name %s\n", NAME);
            printf("id author Tarunvir Randhawa\n");
            printf("uciok\n");
        } else if (!strncmp(line, "eval", 4)) {         // used for testing
            PrintBoard(pos);
            printf("Eval:%d\n", EvalPosition(pos));
            MirrorBoard(pos);
            PrintBoard(pos);
            printf("Eval:%d\n", EvalPosition(pos));
            MirrorBoard(pos);
        } else if (!strncmp(line, "mirror", 6)) {
            MirrorEvalTest(pos);
        }

        if(info->quit) break;
    }
    free(pos->hash_table->p_table);
    free(pos);

}