#include "card_ui.h"

#include "business.h"
#include "card_view.h"
#include "common.h"
#include "menu.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static const char *getStartBillingMessage(BizResult result)
{
    switch (result) {
    case BIZ_ERR_CARD_NOT_FOUND:
    case BIZ_ERR_WRONG_PASSWORD:
        return "卡号或密码错误！";
    case BIZ_ERR_CARD_UNAVAILABLE:
        return "该卡正在使用！";
    case BIZ_ERR_CARD_CANCELED_FOR_START:
        return "该卡已注销！";
    case BIZ_ERR_BALANCE_NOT_ENOUGH:
        return "余额不足！";
    default:
        return bizGetMessage(result);
    }
}

static const char *getStopBillingMessage(BizResult result)
{
    switch (result) {
    case BIZ_ERR_CARD_NOT_FOUND:
    case BIZ_ERR_WRONG_PASSWORD:
        return "卡号或密码错误！";
    default:
        return bizGetMessage(result);
    }
}

void handleAddCardInteraction(void)
{
    char cardName[INPUT_BUF_SIZE];
    char password[INPUT_BUF_SIZE];
    char amountText[INPUT_BUF_SIZE];
    Card card;
    BizResult result = BIZ_OK;

    if (readTextInput("请输入卡号（1~18位）：", cardName, sizeof(cardName)) != 0) {
        printf("%s\n", bizGetMessage(BIZ_ERR_INVALID_CARD_NAME));
        return;
    }

    if (readTextInput("请输入密码（1~8位）：", password, sizeof(password)) != 0) {
        printf("%s\n", bizGetMessage(BIZ_ERR_INVALID_PASSWORD));
        return;
    }

    if (readTextInput("请输入开卡金额（元）：", amountText, sizeof(amountText)) != 0) {
        printf("%s\n", bizGetMessage(BIZ_ERR_INVALID_AMOUNT));
        return;
    }

    result = bizAddCard(cardName, password, amountText, &card);
    if (result != BIZ_OK) {
        printf("%s\n", bizGetMessage(result));
        return;
    }

    printf("添加卡成功！\n");
    viewShowCardSummary(&card);
}

void handleQueryCardInteraction(void)
{
    char queryText[INPUT_BUF_SIZE];
    Card card;
    Card *cards = NULL;
    size_t matchCount = 0;
    size_t actualCount = 0;
    int queryMode = 0;
    BizResult result = BIZ_OK;

    printf("1. 精确查询\n");
    printf("2. 模糊查询\n");
    if (readChoiceInput("请选择查询方式：", &queryMode) != 0) {
        printf("查询方式输入格式错误，请输入数字编号（1~2）。\n");
        return;
    }

    switch (queryMode) {
    case 1:
        if (readTextInput("请输入卡号（1~18位）：", queryText, sizeof(queryText)) != 0) {
            printf("%s\n", bizGetMessage(BIZ_ERR_INVALID_CARD_NAME));
            return;
        }

        result = bizQueryCard(queryText, &card);
        if (result != BIZ_OK) {
            printf("%s\n", bizGetMessage(result));
            return;
        }

        viewShowQueryCardDetails(&card);
        break;
    case 2:
        if (readTextInput("请输入查询关键字（1~18位）：", queryText, sizeof(queryText)) != 0) {
            printf("%s\n", bizGetMessage(BIZ_ERR_INVALID_CARD_NAME));
            return;
        }

        result = bizQueryCardsByKeyword(queryText, NULL, 0, &actualCount, &matchCount);
        if (result != BIZ_OK) {
            printf("%s\n", bizGetMessage(result));
            return;
        }

        cards = (Card *)malloc(matchCount * sizeof(Card));
        if (cards == NULL) {
            printf("%s\n", bizGetMessage(BIZ_ERR_NO_MEMORY));
            return;
        }

        result = bizQueryCardsByKeyword(queryText, cards, matchCount, &actualCount, &matchCount);
        if (result != BIZ_OK) {
            free(cards);
            printf("%s\n", bizGetMessage(result));
            return;
        }

        viewShowFuzzyQueryResults(queryText, cards, actualCount);
        free(cards);
        break;
    default:
        printf("无效查询方式，请输入 1~2。\n");
        return;
    }
}

void handleStartBillingInteraction(void)
{
    char cardName[INPUT_BUF_SIZE];
    char password[INPUT_BUF_SIZE];
    LogonInfo logonInfo;
    BizResult result = BIZ_OK;
    time_t requestTime = (time_t)0;

    if (readTextInput("请输入卡号（1~18位）：", cardName, sizeof(cardName)) != 0) {
        printf("%s\n", bizGetMessage(BIZ_ERR_INVALID_CARD_NAME));
        return;
    }

    if (readTextInput("请输入密码（1~8位）：", password, sizeof(password)) != 0) {
        printf("%s\n", bizGetMessage(BIZ_ERR_INVALID_PASSWORD));
        return;
    }

    requestTime = time(NULL);
    result = bizStartBilling(cardName, password, requestTime, &logonInfo);
    if (result != BIZ_OK) {
        printf("上机失败！\n");
        printf("%s\n", getStartBillingMessage(result));
        return;
    }

    printf("上机成功！\n");
    viewShowLogonInfo(&logonInfo);
}

void handleStopBillingInteraction(void)
{
    char cardName[INPUT_BUF_SIZE];
    char password[INPUT_BUF_SIZE];
    SettleInfo settleInfo;
    BizResult result = BIZ_OK;
    time_t requestTime = (time_t)0;

    if (readTextInput("请输入卡号（1~18位）：", cardName, sizeof(cardName)) != 0) {
        printf("%s\n", bizGetMessage(BIZ_ERR_INVALID_CARD_NAME));
        return;
    }

    if (readTextInput("请输入密码（1~8位）：", password, sizeof(password)) != 0) {
        printf("%s\n", bizGetMessage(BIZ_ERR_INVALID_PASSWORD));
        return;
    }

    requestTime = time(NULL);
    result = bizStopBilling(cardName, password, requestTime, &settleInfo);
    if (result != BIZ_OK) {
        printf("下机失败！\n");
        printf("%s\n", getStopBillingMessage(result));
        return;
    }

    printf("下机成功！\n");
    viewShowSettleInfo(&settleInfo);
}
