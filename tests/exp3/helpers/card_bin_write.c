#include "data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
    Card card;

    if (argc != 7) {
        fprintf(stderr, "usage: %s <card_name> <pwd> <status> <total_use_cent> <use_count> <balance_cent>\n", argv[0]);
        return 2;
    }

    memset(&card, 0, sizeof(card));
    snprintf(card.aCardName, sizeof(card.aCardName), "%s", argv[1]);
    snprintf(card.aPwd, sizeof(card.aPwd), "%s", argv[2]);
    card.nStatus = atoi(argv[3]);
    card.nTotalUseCent = atoi(argv[4]);
    card.nUseCount = atoi(argv[5]);
    card.nBalanceCent = atoi(argv[6]);
    card.nDel = 0;
    card.tStart = time(NULL);
    card.tEnd = card.tStart;
    card.tLast = card.tStart;

    if (saveCard(&card) != DATA_OK) {
        fprintf(stderr, "saveCard failed\n");
        return 1;
    }

    return 0;
}
