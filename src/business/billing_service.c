#include "business.h"
#include "billing_repository.h"
#include "billing_storage_file.h"
#include "card_repository.h"
#include "card_storage_file.h"
#include "card_validator.h"
#include "common.h"
#include "operation_log.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static BizResult mapDataResult(DataResult result)
{
    switch (result) {
    case DATA_OK:
        return BIZ_OK;
    case DATA_ERR_DUPLICATE:
        return BIZ_ERR_DUPLICATE_CARD;
    case DATA_ERR_NO_MEMORY:
        return BIZ_ERR_NO_MEMORY;
    case DATA_ERR_FILE_OPEN:
        return BIZ_ERR_FILE_OPEN;
    case DATA_ERR_FILE_NOT_FOUND:
        return BIZ_ERR_FILE_NOT_FOUND;
    case DATA_ERR_RECORD_FORMAT:
        return BIZ_ERR_RECORD_FORMAT;
    case DATA_ERR_TIME_PARSE:
        return BIZ_ERR_TIME_PARSE;
    default:
        return BIZ_ERR_SYSTEM;
    }
}

static BizResult prepareFuzzyQueryKeyword(const char *keywordInput,
                                          char *keyword,
                                          size_t keywordSize)
{
    int readResult = 0;

    if (validatorNormalizeInput(keywordInput, keyword, keywordSize) != 0 ||
        !validatorIsValidCardName(keyword)) {
        return BIZ_ERR_INVALID_CARD_NAME;
    }

    readResult = dataLoadCards();
    if (readResult < 0) {
        return mapDataResult((DataResult)readResult);
    }

    return BIZ_OK;
}

static int isStartBillingAllowedStatus(int status)
{
    return status == CARD_STATUS_OFFLINE;
}

BizResult bizAddCard(const char *cardNameInput, const char *passwordInput, const char *amountInput, Card *createdCard)
{
    char cardName[INPUT_BUF_SIZE];
    char password[INPUT_BUF_SIZE];
    char amountText[INPUT_BUF_SIZE];
    int32_t amountCent = 0;
    MoneyParseResult moneyParseResult = MONEY_PARSE_OK;
    Card card;
    time_t now = 0;
    int readResult = 0;
    DataResult dataResult = DATA_OK;

    if (validatorNormalizeInput(cardNameInput, cardName, sizeof(cardName)) != 0 ||
        !validatorIsValidCardName(cardName)) {
        return BIZ_ERR_INVALID_CARD_NAME;
    }

    if (validatorNormalizeInput(passwordInput, password, sizeof(password)) != 0 ||
        !validatorIsValidPassword(password)) {
        return BIZ_ERR_INVALID_PASSWORD;
    }

    if (validatorNormalizeInput(amountInput, amountText, sizeof(amountText)) != 0) {
        return BIZ_ERR_INVALID_AMOUNT;
    }

    moneyParseResult = validatorParseMoneyToCent(amountText, &amountCent);
    if (moneyParseResult == MONEY_PARSE_INVALID) {
        return BIZ_ERR_INVALID_AMOUNT;
    }
    if (moneyParseResult == MONEY_PARSE_TOO_LARGE) {
        return BIZ_ERR_BALANCE_TOO_LARGE;
    }

    readResult = dataLoadCards();
    if (readResult < 0 && readResult != DATA_ERR_FILE_NOT_FOUND) {
        return mapDataResult((DataResult)readResult);
    }

    if (dataCardExists(cardName)) {
        return BIZ_ERR_DUPLICATE_CARD;
    }

    memset(&card, 0, sizeof(card));
    memcpy(card.aCardName, cardName, strlen(cardName) + 1);
    memcpy(card.aPwd, password, strlen(password) + 1);
    card.nStatus = CARD_STATUS_OFFLINE;
    now = time(NULL);
    card.tStart = now;
    card.tEnd = now + (time_t)(365 * 24 * 60 * 60);
    card.tLast = now;
    card.nTotalUseCent = 0;
    card.nUseCount = 0;
    card.nBalanceCent = amountCent;
    card.nDel = 0;

    dataResult = (DataResult)dataAddCard(&card);
    if (dataResult != DATA_OK) {
        return mapDataResult(dataResult);
    }

    dataResult = dataSaveCard(&card);
    if (dataResult != DATA_OK) {
        (void)dataLoadCards();
        return mapDataResult(dataResult);
    }

    logOperation("添加卡");
    if (createdCard != NULL) {
        *createdCard = card;
    }
    return BIZ_OK;
}

BizResult bizQueryCard(const char *cardNameInput, Card *queriedCard)
{
    char cardName[INPUT_BUF_SIZE];
    const Card *card = NULL;
    int readResult = 0;

    if (validatorNormalizeInput(cardNameInput, cardName, sizeof(cardName)) != 0 ||
        !validatorIsValidCardName(cardName)) {
        return BIZ_ERR_INVALID_CARD_NAME;
    }

    readResult = dataLoadCards();
    if (readResult < 0) {
        return mapDataResult((DataResult)readResult);
    }

    card = dataQueryCardByName(cardName);
    if (card == NULL) {
        return BIZ_ERR_CARD_NOT_FOUND;
    }

    logOperation("查询卡");
    if (queriedCard != NULL) {
        *queriedCard = *card;
    }
    return BIZ_OK;
}

BizResult bizQueryCardsByKeyword(const char *keywordInput,
                                 Card *buffer,
                                 size_t capacity,
                                 size_t *actualCount,
                                 size_t *requiredCount)
{
    char keyword[INPUT_BUF_SIZE];
    size_t copied = 0;
    BizResult result = BIZ_OK;

    if (actualCount == NULL || requiredCount == NULL) {
        return BIZ_ERR_SYSTEM;
    }

    *actualCount = 0;
    *requiredCount = 0;

    result = prepareFuzzyQueryKeyword(keywordInput, keyword, sizeof(keyword));
    if (result != BIZ_OK) {
        return result;
    }

    if (dataQueryCardsByKeyword(keyword, buffer, capacity, &copied, requiredCount) != DATA_OK) {
        return BIZ_ERR_SYSTEM;
    }
    if (*requiredCount == 0) {
        return BIZ_ERR_NO_MATCHED_CARD;
    }

    if (buffer == NULL || capacity == 0) {
        return BIZ_OK;
    }

    if (copied != *requiredCount) {
        return BIZ_ERR_SYSTEM;
    }

    logOperation("模糊查询");
    *actualCount = copied;
    return BIZ_OK;
}

const char *bizGetMessage(BizResult result)
{
    switch (result) {
    case BIZ_OK:
        return "操作成功。";
    case BIZ_ERR_INVALID_CARD_NAME:
        return "卡号输入不合法，应为1~18位，且只能包含大小写字母、数字和 _ @ # $ % !。";
    case BIZ_ERR_INVALID_PASSWORD:
        return "密码输入不合法，应为1~8位，且只能包含大小写字母、数字和 _ @ # $ % !。";
    case BIZ_ERR_INVALID_AMOUNT:
        return "开卡金额输入不合法，应为非负金额，且最多保留两位小数。";
    case BIZ_ERR_BALANCE_TOO_LARGE:
        return "余额过大，卡内余额必须小于1000000元。";
    case BIZ_ERR_DUPLICATE_CARD:
        return "卡号已存在，不能重复添加！";
    case BIZ_ERR_CARD_NOT_FOUND:
        return "没有该卡的信息！";
    case BIZ_ERR_NO_MATCHED_CARD:
        return "没有符合关键字的卡信息！";
    case BIZ_ERR_WRONG_PASSWORD:
        return "密码错误！";
    case BIZ_ERR_CARD_UNAVAILABLE:
        return "该卡正在使用或已注销，不能上机！";
    case BIZ_ERR_BALANCE_NOT_ENOUGH:
        return "余额不足，不能上机！";
    case BIZ_ERR_FILE_OPEN:
        return "数据文件异常：卡信息文件打开失败。";
    case BIZ_ERR_FILE_NOT_FOUND:
        return "数据文件异常：卡信息文件不存在。";
    case BIZ_ERR_RECORD_FORMAT:
        return "数据文件内容异常：卡信息文件记录格式错误。";
    case BIZ_ERR_TIME_PARSE:
        return "数据文件内容异常：卡信息文件时间字段解析失败。";
    case BIZ_ERR_NO_MEMORY:
        return "系统内存不足，无法继续操作。";
    default:
        return "系统内部错误。";
    }
}

BizResult bizStartBilling(const char *cardNameInput, const char *passwordInput, LogonInfo *logonInfo)
{
    char cardName[INPUT_BUF_SIZE];
    char password[INPUT_BUF_SIZE];
    int cardLoadResult = 0;
    int billingLoadResult = 0;
    const Card *card = NULL;
    Card updatedCard;
    Card originalCard;
    Billing billing;
    DataResult dataResult = DATA_OK;
    time_t now = 0;

    if (validatorNormalizeInput(cardNameInput, cardName, sizeof(cardName)) != 0 ||
        !validatorIsValidCardName(cardName)) {
        return BIZ_ERR_INVALID_CARD_NAME;
    }

    if (validatorNormalizeInput(passwordInput, password, sizeof(password)) != 0 ||
        !validatorIsValidPassword(password)) {
        return BIZ_ERR_INVALID_PASSWORD;
    }

    cardLoadResult = dataLoadCards();
    if (cardLoadResult < 0) {
        return mapDataResult((DataResult)cardLoadResult);
    }

    billingLoadResult = dataLoadBillings();
    if (billingLoadResult < 0 && billingLoadResult != DATA_ERR_FILE_NOT_FOUND) {
        return mapDataResult((DataResult)billingLoadResult);
    }

    card = dataQueryCardByName(cardName);
    if (card == NULL) {
        return BIZ_ERR_CARD_NOT_FOUND;
    }
    if (card->nDel != 0) {
        return BIZ_ERR_CARD_NOT_FOUND;
    }
    if (strcmp(card->aPwd, password) != 0) {
        return BIZ_ERR_WRONG_PASSWORD;
    }
    if (!isStartBillingAllowedStatus(card->nStatus)) {
        return BIZ_ERR_CARD_UNAVAILABLE;
    }
    if (card->nBalanceCent < 0) {
        return BIZ_ERR_BALANCE_NOT_ENOUGH;
    }

    originalCard = *card;
    updatedCard = *card;
    now = time(NULL);
    updatedCard.nStatus = CARD_STATUS_ONLINE;

    dataResult = dataUpdateCard(&updatedCard);
    if (dataResult != DATA_OK) {
        return mapDataResult(dataResult);
    }

    memset(&billing, 0, sizeof(billing));
    memcpy(billing.aCardName, updatedCard.aCardName, strlen(updatedCard.aCardName) + 1);
    billing.tStart = now;
    billing.tEnd = (time_t)0;
    billing.nAmountCent = 0;
    billing.nStatus = 0;
    billing.nDel = 0;

    dataResult = dataSaveBilling(&billing);
    if (dataResult != DATA_OK) {
        if (dataUpdateCard(&originalCard) != DATA_OK) {
            return BIZ_ERR_SYSTEM;
        }
        return mapDataResult(dataResult);
    }

    if (logonInfo != NULL) {
        memset(logonInfo, 0, sizeof(*logonInfo));
        memcpy(logonInfo->aCardName, updatedCard.aCardName, strlen(updatedCard.aCardName) + 1);
        logonInfo->tStart = now;
        logonInfo->nStatus = CARD_STATUS_ONLINE;
        logonInfo->nBalanceCent = updatedCard.nBalanceCent;
    }

    logOperation("上机");
    return BIZ_OK;
}

void bizStopBilling(void)
{
    printf("[业务逻辑层] 下机计费功能入口。\n");
    logOperation("下机");
}

void bizRecharge(void)
{
    printf("[业务逻辑层] 充值功能入口。\n");
    logOperation("充值");
}

void bizRefund(void)
{
    printf("[业务逻辑层] 退费功能入口。\n");
    logOperation("退费");
}

void bizStatistics(void)
{
    printf("[业务逻辑层] 查询统计功能入口。\n");
    logOperation("查询统计");
}

void bizCancelCard(void)
{
    printf("[业务逻辑层] 注销卡功能入口。\n");
    logOperation("注销卡");
}

void bizShutdown(void)
{
    dataCleanup();
    dataCleanupBillings();
}
