#include "business.h"
#include "card_view.h"
#include "common.h"
#include "menu.h"

#include <stdio.h>

static void handleAddCard(void)
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

static void handleQueryCard(void)
{
    char cardName[INPUT_BUF_SIZE];
    Card card;
    BizResult result = BIZ_OK;

    if (readTextInput("请输入卡号（1~18位）：", cardName, sizeof(cardName)) != 0) {
        printf("%s\n", bizGetMessage(BIZ_ERR_INVALID_CARD_NAME));
        return;
    }

    result = bizQueryCard(cardName, &card);
    if (result != BIZ_OK) {
        printf("%s\n", bizGetMessage(result));
        return;
    }

    viewShowQueryCardDetails(&card);
}

static void handleMenuChoice(int choice)
{
    switch (choice) {
    case 1:
        printf("你选择了：添加卡\n");
        handleAddCard();
        break;
    case 2:
        printf("你选择了：查询卡\n");
        handleQueryCard();
        break;
    case 3:
        printf("你选择了：上机\n");
        bizStartBilling();
        break;
    case 4:
        printf("你选择了：下机\n");
        bizStopBilling();
        break;
    case 5:
        printf("你选择了：充值\n");
        bizRecharge();
        break;
    case 6:
        printf("你选择了：退费\n");
        bizRefund();
        break;
    case 7:
        printf("你选择了：查询统计\n");
        bizStatistics();
        break;
    case 8:
        printf("你选择了：注销卡\n");
        bizCancelCard();
        break;
    case 0:
        printf("系统已退出。\n");
        break;
    default:
        printf("无效菜单编号，请输入 0~8。\n");
        break;
    }
}

int main(void)
{
    int choice = -1;

    do {
        outputMenu();

        if (readMenuChoice(&choice) != 0) {
            printf("输入格式错误，请输入数字菜单编号（0~8）。\n");
            continue;
        }

        handleMenuChoice(choice);
        printf("\n");
    } while (choice != 0);

    bizShutdown();
    return 0;
}
