#include "data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int count = 0;
    const Card *card = NULL;

    if (argc != 8) {
        fprintf(stderr, "usage: %s <expected_count> <card_name> <pwd> <status> <total_use_cent> <use_count> <balance_cent>\n", argv[0]);
        return 2;
    }

    count = readCard();
    if (count != atoi(argv[1])) {
        fprintf(stderr, "count mismatch: got %d want %s\n", count, argv[1]);
        return 1;
    }

    card = dataFindCardByName(argv[2]);
    if (card == NULL) {
        fprintf(stderr, "card not found: %s\n", argv[2]);
        return 1;
    }

    if (strcmp(card->aPwd, argv[3]) != 0 ||
        card->nStatus != atoi(argv[4]) ||
        card->nTotalUseCent != atoi(argv[5]) ||
        card->nUseCount != atoi(argv[6]) ||
        card->nBalanceCent != atoi(argv[7])) {
        fprintf(stderr, "card content mismatch\n");
        return 1;
    }

    dataCleanup();
    return 0;
}
