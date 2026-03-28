#include "card_ui.h"

#include "business.h"
#include "card_view.h"
#include "common.h"
#include "menu.h"

#include <stdlib.h>
#include <stdio.h>

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

        result = bizCountFuzzyQueryCards(queryText, &matchCount);
        if (result != BIZ_OK) {
            printf("%s\n", bizGetMessage(result));
            return;
        }

        cards = (Card *)malloc(matchCount * sizeof(Card));
        if (cards == NULL) {
            printf("%s\n", bizGetMessage(BIZ_ERR_NO_MEMORY));
            return;
        }

        result = bizFillFuzzyQueryCards(queryText, cards, matchCount, &actualCount);
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
