#ifndef BUSINESS_H
#define BUSINESS_H

#include <stdint.h>

#include "model.h"

typedef enum BizResult {
    BIZ_OK = 0,
    BIZ_ERR_INVALID_CARD_NAME = -1,
    BIZ_ERR_INVALID_PASSWORD = -2,
    BIZ_ERR_INVALID_AMOUNT = -3,
    BIZ_ERR_BALANCE_TOO_LARGE = -4,
    BIZ_ERR_DUPLICATE_CARD = -5,
    BIZ_ERR_CARD_NOT_FOUND = -6,
    BIZ_ERR_FILE_OPEN = -7,
    BIZ_ERR_FILE_NOT_FOUND = -8,
    BIZ_ERR_RECORD_FORMAT = -9,
    BIZ_ERR_NO_MEMORY = -10,
    BIZ_ERR_SYSTEM = -11
} BizResult;

BizResult bizAddCard(const char *cardNameInput, const char *passwordInput, const char *amountInput, Card *createdCard);
BizResult bizQueryCard(const char *cardNameInput, Card *queriedCard);
const char *bizGetMessage(BizResult result);
void bizStartBilling(void);
void bizStopBilling(void);
void bizRecharge(void);
void bizRefund(void);
void bizStatistics(void);
void bizCancelCard(void);
void bizShutdown(void);

#endif
