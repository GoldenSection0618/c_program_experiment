#include "business.h"
#include "data.h"

void bizAddCard(void)
{
    printf("[业务逻辑层] 添加卡功能入口。\n");
    dataLogOperation("添加卡");
}

void bizQueryCard(void)
{
    printf("[业务逻辑层] 查询卡功能入口。\n");
    dataLogOperation("查询卡");
}

void bizStartBilling(void)
{
    printf("[业务逻辑层] 上机计费功能入口。\n");
    dataLogOperation("上机");
}

void bizStopBilling(void)
{
    printf("[业务逻辑层] 下机计费功能入口。\n");
    dataLogOperation("下机");
}

void bizRecharge(void)
{
    printf("[业务逻辑层] 充值功能入口。\n");
    dataLogOperation("充值");
}

void bizRefund(void)
{
    printf("[业务逻辑层] 退费功能入口。\n");
    dataLogOperation("退费");
}

void bizStatistics(void)
{
    printf("[业务逻辑层] 查询统计功能入口。\n");
    dataLogOperation("查询统计");
}

void bizCancelCard(void)
{
    printf("[业务逻辑层] 注销卡功能入口。\n");
    dataLogOperation("注销卡");
}
