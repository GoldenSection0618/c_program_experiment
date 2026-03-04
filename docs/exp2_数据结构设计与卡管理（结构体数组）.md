# 实验二：数据结构设计与卡管理（结构体数组）

## 1. 实验目标
1. 完成计费管理系统核心数据结构设计（`typedef struct`）。
2. 在实验一框架基础上，使用结构体数组实现卡管理的“新增卡、查询卡”。

## 2. 数据结构设计
本实验在 [model.h](../include/model.h) 中定义以下类型：

1. `Card`：卡信息（卡号、密码、状态、开卡时间、截止时间、累计使用、最后使用、次数、余额、删除标识）
2. `Billing`：消费记录
3. `LogonInfo`：上机信息
4. `SettleInfo`：下机结算信息
5. `Money`：充值/退费记录
6. `Rate`：计费标准
7. `Admin`：管理员信息

说明：
- 使用了 `time_t` 时间字段；
- 已包含 `#include <time.h>`；
- 状态码包含：`0未上机/1正在上机/2已注销/3失效`。

## 3. 结构体数组存储方案
数据存储层使用内存结构体数组：

- `Card g_cards[MAX_CARD_COUNT]`
- `size_t g_card_count`

数据层接口见 [data.h](../include/data.h)：
- `dataAddCard`
- `dataFindCardByName`
- `dataGetCardCount`
- `dataGetCardByIndex`

实现见 [repository.c](../src/data/repository.c)。

## 4. 已实现功能（实验二重点）
### 4.1 菜单 1：新增卡
实现位置：[billing_service.c](../src/business/billing_service.c)

已覆盖：
1. 输入卡号（1\~18位）/密码（1\~8位）/开卡金额；
2. 卡号重复校验；
3. 密码长度校验（含超长拦截）；
4. 新卡字段初始化（状态、时间、余额、次数、累计金额、删除标识）；
5. 成功后列表化显示（卡号/密码/状态/余额）。

### 4.2 菜单 2：查询卡
实现位置：[billing_service.c](../src/business/billing_service.c)

已覆盖：
1. 按卡号查找；
2. 存在时显示：卡号、卡状态、余额、累计使用、使用次数、最后使用时间；
3. 不存在时提示“没有该卡的信息！”。

## 5. 测试用例
默认实验二测试数据：
- 输入：`tests/exp2/menu.input`
- 断言：`tests/exp2/menu.expect.tsv`

执行：
```bash
make test
```

覆盖点：
1. 菜单 1/2 可运行；
2. 新增卡正常；
3. 密码超长拦截；
4. 重复卡号拦截；
5. 查询存在/不存在两种场景。
