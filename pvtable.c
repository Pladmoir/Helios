#include <stdio.h>
#include "defs.h"

const int PvSize = 0x100000 * 2;

int GetPvLine(const int depth, S_BOARD *pos) {
    assert(depth < MAXDEPTH);
    int move = ProbePvTable(pos);
    int count = 0;

    while(move != NOMOVE && count < depth) {
        assert(count < MAXDEPTH);

        if(MoveExists(pos, move)) {
            MakeMove(pos, move);
            pos->PvArray[count++] = move;
        } else {
            break;
        }
        move = ProbePvTable(pos);
    }

    while(pos->half_moves > 0) {
        TakeMove(pos);
    }

    return count;

    
}

void ClearPvTable(S_PVTABLE * table) {
    S_PVENTRY *pv_entry;
    for(pv_entry = table->p_table; pv_entry < table->p_table + table->num_entries; pv_entry++) {
        pv_entry->pos_key = 0ULL;
        pv_entry->move = NOMOVE;
    }
}

void InitPvTable(S_PVTABLE *table) {
    table->num_entries = PvSize / sizeof(S_PVENTRY);
    table->num_entries -= 2;
    if(table->p_table) {
        free(table->p_table);
    }
    table->p_table = (S_PVENTRY *) malloc(table->num_entries * sizeof(S_PVENTRY));
    ClearPvTable(table);
    printf("PvTable init complete with %d enteries \n", table->num_entries);
}

void StorePvMove(const S_BOARD *pos, const int move) {
    int index = pos->pos_key % pos->Pv_Table->num_entries;
    assert(index >= 0 && index <= pos->Pv_Table->num_entries - 1);
    pos->Pv_Table->p_table[index].move = move;
    pos->Pv_Table->p_table[index].pos_key = pos->pos_key;
}

int ProbePvTable(const S_BOARD *pos) {
    int index = pos->pos_key % pos->Pv_Table->num_entries;
    assert(index >= 0 && index <= pos->Pv_Table->num_entries - 1);

    if(pos->Pv_Table->p_table[index].pos_key == pos->pos_key) {
        return pos->Pv_Table->p_table[index].move;
    }

    return NOMOVE;
}