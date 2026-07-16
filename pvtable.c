#include <stdio.h>
#include "defs.h"
#include <stdbool.h>

const int HashSize = 0x100000 * 16;

int GetPvLine(const int depth, S_BOARD *pos) {
    assert(depth < MAXDEPTH);
    int move = ProbePvMove(pos);
    int count = 0;

    while(move != NOMOVE && count < depth) {
        assert(count < MAXDEPTH);

        if(MoveExists(pos, move)) {
            MakeMove(pos, move);
            pos->PvArray[count++] = move;
        } else {
            break;
        }
        move = ProbePvMove(pos);
    }

    while(pos->half_moves > 0) {
        TakeMove(pos);
    }

    return count;
    
}

void ClearHashTable(S_HASHTABLE * table) {
    S_HASHENTRY *table_entry;
    for(table_entry = table->p_table; table_entry < table->p_table + table->num_entries; table_entry++) {
        table_entry->pos_key = 0ULL;
        table_entry->move = NOMOVE;
        table_entry->depth = 0;
        table_entry->score = 0;
        table_entry->flags = 0;
    }
    table->new_write = 0;
}

void InitHashTable(S_HASHTABLE *table) {
    table->num_entries = HashSize / sizeof(S_HASHENTRY);
    table->num_entries -= 2;
    if(table->p_table) {
        free(table->p_table);
    }
    table->p_table = (S_HASHENTRY *) malloc(table->num_entries * sizeof(S_HASHENTRY));
    ClearHashTable(table);
    printf("HashTable init complete with %d enteries \n", table->num_entries);
}

int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth) {
    int index = pos->pos_key % pos->hash_table->num_entries;
    assert(index >= 0 && index <= pos->hash_table->num_entries - 1);
    assert(depth>=1 && depth < MAXDEPTH);
    assert(alpha<beta);
    assert(alpha >= -INFINITE && alpha <= INFINITE);
    assert(beta >= -INFINITE && beta <= INFINITE);
    assert(pos->half_moves >= 0 && pos->half_moves < MAXDEPTH);

    if(pos->hash_table->p_table[index].pos_key == pos->pos_key) {
        *move = pos->hash_table->p_table[index].move;
        if(pos->hash_table->p_table[index].depth >= depth) {
            pos->hash_table->hit++;
            assert(pos->hash_table->p_table[index].depth >= 1 && pos->hash_table->p_table[index].depth < MAXDEPTH);
            assert(pos->hash_table->p_table[index].flags >= HFALPHA && pos->hash_table->p_table[index].flags <= HFEXACT);

            *score = pos->hash_table->p_table[index].score;
            if(*score > ISMATE) *score -= pos->half_moves;
            else if(*score < -ISMATE) * score += pos->half_moves;

            switch(pos->hash_table->p_table[index].flags) {

                assert(*score>=-INFINITE && *score<=INFINITE);

                case HFALPHA: if(*score<=alpha) {
                    *score=alpha;
                    return true;
                }
                    break;
                case HFBETA: if(*score >= beta) {
                    *score = beta;
                    return true;
                }
                    break;
                case HFEXACT:
                    return true;
                    break;
                default: assert(false); break;
            }
        }
    }
    return false;
}

void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth) {
    int index = pos->pos_key % pos->hash_table->num_entries;
    assert(index >= 0 && index <= pos->hash_table->num_entries - 1);
    assert(depth>=1 && depth < MAXDEPTH);
    assert(score >= -INFINITE && score <= INFINITE);
    assert(pos->half_moves >= 0 && pos->half_moves < MAXDEPTH);

    if(pos->hash_table->p_table[index].pos_key == 0) {
        pos->hash_table->new_write++;
    } else {
        pos->hash_table->over_write++;
    }

    if(score > ISMATE) score+= pos->half_moves;
    else if(score < -ISMATE) score-= pos->half_moves;

    pos->hash_table->p_table[index].move = move;
    pos->hash_table->p_table[index].pos_key = pos->pos_key;
    pos->hash_table->p_table[index].flags = flags;
    pos->hash_table->p_table[index].score = score;
    pos->hash_table->p_table[index].depth = depth;
}

int ProbePvMove(const S_BOARD *pos) {
    int index = pos->pos_key % pos->hash_table->num_entries;
    assert(index >= 0 && index <= pos->Pv_Table->num_entries - 1);

    if(pos->hash_table->p_table[index].pos_key == pos->pos_key) {
        return pos->hash_table->p_table[index].move;
    }

    return NOMOVE;
}