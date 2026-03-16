#include "data.h"

#include <stdio.h>

int main(void)
{
    int count = 0;
    const Card *card = NULL;
    Card updated;

    count = readCard();
    if (count != 1) {
        fprintf(stderr, "readCard count mismatch: %d\n", count);
        return 1;
    }

    count = getCardCount();
    if (count != 1) {
        fprintf(stderr, "getCardCount mismatch: %d\n", count);
        return 1;
    }

    if (!isCardExists("cardApi") || isCardExists("missingCard")) {
        fprintf(stderr, "isCardExists mismatch\n");
        return 1;
    }

    card = dataFindCardByName("cardApi");
    if (card == NULL) {
        fprintf(stderr, "dataFindCardByName failed\n");
        return 1;
    }

    updated = *card;
    updated.nBalanceCent = 20500;
    updated.nTotalUseCent = 500;
    updated.nUseCount = 2;

    if (updateCard(&updated) != DATA_OK) {
        fprintf(stderr, "updateCard failed\n");
        return 1;
    }

    dataCleanup();
    count = readCard();
    if (count != 1) {
        fprintf(stderr, "reload count mismatch: %d\n", count);
        return 1;
    }

    card = dataFindCardByName("cardApi");
    if (card == NULL) {
        fprintf(stderr, "reload find failed\n");
        return 1;
    }

    if (card->nBalanceCent != 20500 || card->nTotalUseCent != 500 || card->nUseCount != 2) {
        fprintf(stderr, "updated values mismatch\n");
        return 1;
    }

    dataCleanup();
    return 0;
}
