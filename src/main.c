#include "business.h"
#include "card_view.h"
#include "menu.h"

#include <stdio.h>

static void handleMenuChoice(int choice)
{
    Card card;

    switch (choice) {
    case 1:
        printf("你选择了：添加卡\n");
        if (bizAddCard(&card) == 0) {
            printf("添加卡成功！\n");
            viewShowCardSummary(&card);
        }
        break;
    case 2:
        printf("你选择了：查询卡\n");
        if (bizQueryCard(&card) == 0) {
            viewShowQueryCardDetails(&card);
        }
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
