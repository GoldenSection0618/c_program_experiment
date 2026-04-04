#ifndef BUSINESS_H
#define BUSINESS_H

#include <stddef.h>
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
    BIZ_ERR_TIME_PARSE = -10,
    BIZ_ERR_NO_MEMORY = -11,
    BIZ_ERR_SYSTEM = -12,
    BIZ_ERR_NO_MATCHED_CARD = -13,
    BIZ_ERR_WRONG_PASSWORD = -14,
    BIZ_ERR_CARD_UNAVAILABLE = -15,
    BIZ_ERR_BALANCE_NOT_ENOUGH = -16,
    BIZ_ERR_NO_UNSETTLED_BILLING = -17,
    BIZ_ERR_CARD_STATUS_INVALID_FOR_STOP = -18,
    BIZ_ERR_CARD_CANCELED_FOR_START = -19,
    BIZ_ERR_CARD_CANCELED_FOR_RECHARGE = -20
} BizResult;

BizResult bizAddCard(const char *cardNameInput, const char *passwordInput, const char *amountInput, Card *createdCard);
BizResult bizQueryCard(const char *cardNameInput, Card *queriedCard);
BizResult bizQueryCardsByKeyword(const char *keywordInput,
                                 Card *buffer,
                                 size_t capacity,
                                 size_t *actualCount,
                                 size_t *requiredCount);
const char *bizGetMessage(BizResult result);
BizResult bizStartBilling(const char *cardNameInput,
                          const char *passwordInput,
                          time_t requestTime,
                          LogonInfo *logonInfo);
BizResult bizStopBilling(const char *cardNameInput,
                         const char *passwordInput,
                         time_t requestTime,
                         SettleInfo *settleInfo);
BizResult bizRecharge(const char *cardNameInput,
                      const char *passwordInput,
                      const char *amountInput,
                      Money *rechargeRecord,
                      Card *updatedCard);
void bizRefund(void);
void bizStatistics(void);
void bizCancelCard(void);
void bizShutdown(void);

#endif
