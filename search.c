#include <stdio.h>
#include <stdbool.h>
#include "defs.h"


// check if time alloted is done, or there is an interrupt from GUI
static void CheckUp(S_SEARCHINFO *info) {
    if(info->timeset == true && GetTimeMs() > info->stoptime) {
        info->stopped = true;
    }
    ReadInput(info);
}

// swaps given move with the best scoring move from the remaining list
void PickNextMove(int move_num, S_MOVELIST *list) {
    S_MOVE temp;
    int index = 0;
    int best_score = 0;
    int best_num = move_num;

    for(index = move_num; index < list->count; ++index) {
        if(list->moves[index].score > best_score) {
            best_score = list->moves[index].score;
            best_num = index;
        }
    }
    temp = list->moves[move_num];
    list->moves[move_num] = list->moves[best_num];
    list->moves[best_num] = temp;
}

static int IsRepetition(const S_BOARD *pos) {
    int index = 0;

    for(index = pos->hist_half_moves - pos->fifty_move_count; index < pos->hist_half_moves - 1; ++index) {
        assert(index >= 0 && index < MAXGAMEMOVES);

        if(pos->pos_key == pos->history[index].posKey) {
            return true;
        }
    }
    return false;
}

// clear the search
static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
    int index = 0;
    int index2 = 0;

    for(index = 0; index < 13; ++index) {
        for(index2 = 0; index2 < BOARD_SQR_NUM; ++index2) {
            pos->search_history[index][index2] = 0;
        }
    }

    for(index = 0; index < 2; ++index) {
        for(index2 = 0; index2 < MAXDEPTH; ++index2) {
            pos->search_killers[index][index2] = 0;
        }
    }

    ClearPvTable(pos->Pv_Table);
    pos->half_moves = 0;
    info->stopped = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
}

static int Quiescence(int alpha, int beta,  S_BOARD *pos, S_SEARCHINFO *info) {
    assert(CheckBoard(pos));

    //check if time alloted for the search is up (check happens every 2048 nodes)
    if((info->nodes & 2047) == 0) {
        CheckUp(info);
    }
    info->nodes++;

    if(IsRepetition(pos) || pos->fifty_move_count >= 100) {
        return 0;
    }

    if(pos->half_moves > MAXDEPTH - 1) {
        return EvalPosition(pos);
    }

    int score = EvalPosition(pos);

    if(score >= beta) {
        return beta;
    }

    if(score > alpha) {
        alpha = score;
    }

    S_MOVELIST list[1];
    GenerateAllCaps(pos,list);
    int move_num = 0;
    int legal = 0;
    int old_alpha = alpha;
    int best_move = 0;
    score = -INFINITE;
    int PvMove = ProbePvTable(pos);
    
    for(move_num = 0; move_num < list->count; ++move_num) {
        
        PickNextMove(move_num, list); // sorts the movelist

        if(!MakeMove(pos, list->moves[move_num].move)) {
            continue;
        }
        legal++;
        score = -Quiescence( -beta, -alpha, pos, info);
        TakeMove(pos);

         if(info->stopped == true) {
            return 0;
        }

        if(score > alpha) {
            if(score >= beta) { // beta cutoff 
                if(legal == 1) {
                    info->fhf++;
                }
                info->fh++;
                return beta;
            }
                alpha = score;
                best_move = list->moves[move_num].move;
        }
    }

    if(alpha != old_alpha) {
        StorePvMove(pos, best_move);
    }

    return alpha;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {
    assert(CheckBoard(pos));

    if(depth == 0) {
        return Quiescence(alpha, beta, pos, info);
    }

    if((info->nodes & 2047) == 0) {
        CheckUp(info);
    }

    info->nodes++;

    // returns draw evaluation
    if(IsRepetition(pos) || pos->fifty_move_count >= 100) {
        return 0;
    }

    // checks if half moves (our depth) has exceeded or equal to maxdepth (stops search)
    if(pos->half_moves > MAXDEPTH - 1) {
        return EvalPosition(pos);
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
    int move_num = 0;
    int legal = 0;
    int old_alpha = alpha;
    int best_move = 0;
    int score = -INFINITE;
    int PvMove = ProbePvTable(pos);

    // if the move has been shown to be the best move before search it first
    if(PvMove != NOMOVE) {
        for(move_num = 0; move_num < list->count; ++move_num) {
            if (list->moves[move_num].move == PvMove) {
                list->moves[move_num].score = 2000000;
                break;
            }
        }
    }

    for(move_num = 0; move_num < list->count; ++move_num) {
        
        PickNextMove(move_num, list); // sorts the movelist

        if(!MakeMove(pos, list->moves[move_num].move)) {
            continue;
        }
        legal++;
        score = -AlphaBeta( -beta, -alpha, depth - 1, pos, info, true);
        TakeMove(pos);

        if(info->stopped == true) {
            return 0;
        }

        if(score > alpha) {
            if(score >= beta) { // beta cutoff 
                if(legal == 1) {
                    info->fhf++;
                }
                info->fh++;

                if(!(list->moves[move_num].move & MFLAGCAP)) {
                    pos->search_killers[1][pos->half_moves] = pos->search_killers[0][pos->half_moves]; 
                    pos->search_killers[0][pos->half_moves] = list->moves[move_num].move;
                }
                return beta;
            }
                alpha = score;
                best_move = list->moves[move_num].move;
                if(!(list->moves[move_num].move & MFLAGCAP)) {
                    pos->search_history[pos->pieces[FROMSQ(best_move)]][TOSQ(best_move)] += depth;
                }
        }
    }

    if(legal == 0) {
        if(SqAttacked(pos->KingSq[pos->side], pos->side^1, pos)) {
            return -MATE + pos->half_moves; // distance to mate from root
        } else {
            return 0; // stalemate
        }
    }

    // stores best move in our pv table
    if(alpha != old_alpha) {
        StorePvMove(pos, best_move);
    }

    return alpha;
}

// does iterative deepning (search alphabeta for depths 1 to max depth until time runs out or stop command)
void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
    int best_move = NOMOVE;
    int best_score = -INFINITE;
    int curr_depth = 0; // current depth
    int pv_moves = 0;
    int pv_num = 0;
    ClearForSearch(pos, info);
    
    for(curr_depth = 1; curr_depth <= info->depth; ++ curr_depth) {
        best_score = AlphaBeta(-INFINITE, INFINITE, curr_depth, pos, info, true);

        if(info->stopped == true) {
            break;
        }

        pv_moves = GetPvLine(curr_depth, pos);
        best_move = pos->PvArray[0];

        printf("info score cp %d depth %d nodes %ld time %d ", best_score, curr_depth, info->nodes, GetTimeMs()-info->starttime);
        pv_moves = GetPvLine(curr_depth, pos);
        printf("pv");
        for(pv_num = 0; pv_num < pv_moves; ++pv_num) {
            printf(" %s", PrMove(pos->PvArray[pv_num]));
        }
        printf("\n");
        //printf("Ordering:%.2f\n",(info->fhf/info->fh)); // aim for > 90%
    }

    printf("bestmove %s\n", PrMove(best_move));
}
