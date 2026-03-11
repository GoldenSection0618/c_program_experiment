#ifndef DATA_H
#define DATA_H

#include "common.h"
#include "model.h"

typedef enum DataResult {
    DATA_OK = 0,
    DATA_ERR_DUPLICATE = -1,
    DATA_ERR_NO_MEMORY = -2,
    DATA_ERR_INVALID_ARG = -3
} DataResult;

void dataLogOperation(const char *operation);
int dataAddCard(const Card *card);
const Card *dataFindCardByName(const char *cardName);
size_t dataGetCardCount(void);
const Card *dataGetCardByIndex(size_t index);
void dataCleanup(void);

#endif
