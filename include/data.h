#ifndef DATA_H
#define DATA_H

#include <stddef.h>

#include "model.h"

typedef enum DataResult {
    DATA_OK = 0,
    DATA_ERR_DUPLICATE = -1,
    DATA_ERR_NO_MEMORY = -2,
    DATA_ERR_INVALID_ARG = -3,
    DATA_ERR_FILE_OPEN = -4,
    DATA_ERR_FILE_NOT_FOUND = -5,
    DATA_ERR_RECORD_FORMAT = -6,
    DATA_ERR_TIME_PARSE = -7,
    DATA_ERR_NOT_FOUND = -8
} DataResult;

int dataAddCard(const Card *card);
const Card *dataFindCardByName(const char *cardName);
DataResult dataDeleteCardByName(const char *cardName);
DataResult saveCard(const Card *card);
int readCard(void);
int getCardCount(void);
int isCardExists(const char *cardName);
DataResult updateCard(const Card *card);
void dataCleanup(void);

#endif
