#include "business.h"
#include "common.h"
#include "data.h"
#include "operation_log.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static void trimInPlace(char *text)
{
    size_t len = 0;
    size_t start = 0;
    size_t end = 0;

    if (text == NULL) {
        return;
    }

    len = strlen(text);
    if (len == 0) {
        return;
    }

    while (start < len && isspace((unsigned char)text[start])) {
        start++;
    }

    if (start == len) {
        text[0] = '\0';
        return;
    }

    end = len;
    while (end > start && isspace((unsigned char)text[end - 1])) {
        end--;
    }

    if (start > 0) {
        memmove(text, text + start, end - start);
    }
    text[end - start] = '\0';
}

static int normalizeInput(const char *input, char *buffer, size_t size)
{
    if (input == NULL || buffer == NULL || size == 0) {
        return -1;
    }

    if (snprintf(buffer, size, "%s", input) >= (int)size) {
        return -1;
    }

    trimInPlace(buffer);
    return 0;
}

static int isValidCardName(const char *cardName)
{
    size_t len = 0;

    if (cardName == NULL) {
        return 0;
    }

    len = strlen(cardName);
    if (!(len > 0 && len <= CARD_NAME_MAX_LEN)) {
        return 0;
    }

    while (*cardName != '\0') {
        unsigned char ch = (unsigned char)*cardName;

        if (!(isalnum(ch) || ch == '_' || ch == '@' || ch == '#' || ch == '$' || ch == '%' || ch == '!')) {
            return 0;
        }
        cardName++;
    }

    return 1;
}

static int isValidPassword(const char *password)
{
    size_t len = 0;

    if (password == NULL) {
        return 0;
    }

    len = strlen(password);
    if (!(len > 0 && len <= CARD_PWD_MAX_LEN)) {
        return 0;
    }

    while (*password != '\0') {
        unsigned char ch = (unsigned char)*password;

        if (!(isalnum(ch) || ch == '_' || ch == '@' || ch == '#' || ch == '$' || ch == '%' || ch == '!')) {
            return 0;
        }
        password++;
    }

    return 1;
}

typedef enum MoneyParseResult {
    MONEY_PARSE_OK = 0,
    MONEY_PARSE_INVALID = -1,
    MONEY_PARSE_TOO_LARGE = -2
} MoneyParseResult;

static MoneyParseResult parseMoneyToCent(const char *text, int32_t *amountCent)
{
    const char *p = text;
    int64_t yuan = 0;
    int32_t frac = 0;
    int fracDigits = 0;
    int64_t totalCent = 0;
    int digit = 0;

    if (text == NULL || amountCent == NULL) {
        return MONEY_PARSE_INVALID;
    }

    if (*p == '\0') {
        return MONEY_PARSE_INVALID;
    }

    while (*p >= '0' && *p <= '9') {
        digit = *p - '0';
        if (yuan > (INT64_MAX - digit) / 10) {
            return MONEY_PARSE_TOO_LARGE;
        }
        yuan = yuan * 10 + digit;
        p++;
    }

    if (p == text) {
        return MONEY_PARSE_INVALID;
    }

    if (*p == '.') {
        p++;
        while (*p >= '0' && *p <= '9') {
            if (fracDigits >= 2) {
                return MONEY_PARSE_INVALID;
            }
            frac = (int32_t)(frac * 10 + (*p - '0'));
            fracDigits++;
            p++;
        }
    }

    if (*p != '\0') {
        return MONEY_PARSE_INVALID;
    }

    if (fracDigits == 1) {
        frac *= 10;
    }

    if (yuan > (INT64_MAX - frac) / 100) {
        return MONEY_PARSE_TOO_LARGE;
    }
    totalCent = yuan * 100 + frac;
    if (totalCent < 0) {
        return MONEY_PARSE_INVALID;
    }
    if (totalCent >= MAX_BALANCE_CENT || totalCent > INT32_MAX) {
        return MONEY_PARSE_TOO_LARGE;
    }

    *amountCent = (int32_t)totalCent;
    return MONEY_PARSE_OK;
}

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
    default:
        return BIZ_ERR_SYSTEM;
    }
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

    if (normalizeInput(cardNameInput, cardName, sizeof(cardName)) != 0 || !isValidCardName(cardName)) {
        return BIZ_ERR_INVALID_CARD_NAME;
    }

    if (normalizeInput(passwordInput, password, sizeof(password)) != 0 || !isValidPassword(password)) {
        return BIZ_ERR_INVALID_PASSWORD;
    }

    if (normalizeInput(amountInput, amountText, sizeof(amountText)) != 0) {
        return BIZ_ERR_INVALID_AMOUNT;
    }

    moneyParseResult = parseMoneyToCent(amountText, &amountCent);
    if (moneyParseResult == MONEY_PARSE_INVALID) {
        return BIZ_ERR_INVALID_AMOUNT;
    }
    if (moneyParseResult == MONEY_PARSE_TOO_LARGE) {
        return BIZ_ERR_BALANCE_TOO_LARGE;
    }

    readResult = readCard();
    if (readResult < 0 && readResult != DATA_ERR_FILE_NOT_FOUND) {
        return mapDataResult((DataResult)readResult);
    }

    if (isCardExists(cardName)) {
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

    dataResult = saveCard(&card);
    if (dataResult != DATA_OK) {
        (void)readCard();
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

    if (normalizeInput(cardNameInput, cardName, sizeof(cardName)) != 0 || !isValidCardName(cardName)) {
        return BIZ_ERR_INVALID_CARD_NAME;
    }

    readResult = readCard();
    if (readResult < 0) {
        return mapDataResult((DataResult)readResult);
    }

    card = dataFindCardByName(cardName);
    if (card == NULL) {
        return BIZ_ERR_CARD_NOT_FOUND;
    }

    logOperation("查询卡");
    if (queriedCard != NULL) {
        *queriedCard = *card;
    }
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
    case BIZ_ERR_FILE_OPEN:
        return "数据文件异常：卡信息文件打开失败。";
    case BIZ_ERR_FILE_NOT_FOUND:
        return "数据文件异常：卡信息文件不存在。";
    case BIZ_ERR_RECORD_FORMAT:
        return "数据文件内容异常：卡信息文件记录损坏或格式不完整。";
    case BIZ_ERR_NO_MEMORY:
        return "系统内存不足，无法继续操作。";
    default:
        return "系统内部错误。";
    }
}

void bizStartBilling(void)
{
    printf("[业务逻辑层] 上机计费功能入口。\n");
    logOperation("上机");
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
}
