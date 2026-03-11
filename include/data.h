#ifndef DATA_H
#define DATA_H

#include "model.h"

typedef enum DataResult {
    DATA_OK = 0,
    DATA_ERR_DUPLICATE = -1,
    DATA_ERR_NO_MEMORY = -2,
    DATA_ERR_INVALID_ARG = -3
} DataResult;

int dataAddCard(const Card *card);
const Card *dataFindCardByName(const char *cardName);
void dataCleanup(void);

#endif
