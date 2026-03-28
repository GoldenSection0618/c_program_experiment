# 实验三：文件存储管理（链表持久化）

## 1. 实现目标
本分支在实验二“链表内存存储”的基础上，将卡信息持久化方式改为二进制文件存储，使卡信息能够：
- 长期保存到 `data/card.bin`
- 程序再次运行时从文件恢复到链表
- 基于文件最新内容完成添加卡、查询卡
- 提供记录计数、存在性判断、更新单条记录等底层能力

当前项目的实现原则是：
- 链表负责运行时内存组织
- 二进制文件负责持久化保存
- 相关卡业务执行前先从文件加载链表
- 修改卡数据时同时保证链表与文件一致

## 2. 分层职责
### 2.1 表示层
- [main.c](../src/main.c)
- [menu.c](../src/presentation/menu.c)
- [card_view.c](../src/presentation/card_view.c)

职责：
- 菜单显示与菜单编号读取
- 添加卡/查询卡输入采集
- 根据业务结果码输出成功或失败信息
- 表格化显示卡信息

### 2.2 业务逻辑层
- [billing_service.c](../src/business/billing_service.c)
- [operation_log.c](../src/business/operation_log.c)

职责：
- 校验卡号、密码、金额输入
- 限制卡号、密码必须匹配字符集 `^[A-Za-z0-9_@#$%!]+$`
- 执行添加卡、查询卡业务流程
- 调用数据层完成文件加载、重复校验、落盘、查询
- 把结果码返回给表示层

### 2.3 数据存储层
- [repository.c](../src/data/repository.c)
- [card_storage.h](../include/card_storage.h)

职责：
- 维护内存链表 `g_pCardListHead`
- 管理私有计数 `g_cardCount`
- 负责卡信息二进制文件的读取、写入、重写

## 3. 二进制文件格式
卡数据文件固定为：`data/card.bin`

文件中顺序写入多个 `Card` 结构体记录，每条记录的写入方式为：
```c
fwrite(card, sizeof(Card), 1, fp);
```

读取方式为：
```c
fread(&card, sizeof(Card), 1, fp);
```

说明：
- 文件中不再保存文本字段分隔符，也不再做逐行解析
- 金额字段继续使用“分”为单位的整型存储
- 时间字段直接以 `time_t` 形式保存在 `Card` 结构中
- 读写方式必须严格对称，始终按 `sizeof(Card)` 为单位处理
- 若文件字节数不是 `sizeof(Card)` 的整数倍，按记录损坏处理

## 4. 关键函数职责
### 4.1 saveCard(const Card *card)
- 将一条 `Card` 记录以二进制追加方式写入 `data/card.bin`
- 添加卡成功后用于落盘
- 文件打开失败或写入失败时返回错误状态

### 4.2 readCard(void)
- 先清空当前链表，再从 `data/card.bin` 顺序读取全部 `Card` 记录
- 每读取一条就装入链表
- 文件不存在时返回“文件不存在”状态
- 空文件返回 `0`，表示成功读取到 0 条记录
- 若出现截断记录、读取错误或坏数据，则返回失败状态并清空半成品链表

### 4.3 getCardCount(void)
- 顺序扫描 `data/card.bin`
- 统计文件中完整 `Card` 记录数量
- 文件不存在时返回 `0`

### 4.4 isCardExists(const char *cardName)
- 在当前已加载链表中判断卡号是否存在
- 供添加卡时做重复校验

### 4.5 updateCard(const Card *card)
- 先调用 `readCard()` 取得文件最新状态
- 在链表中找到目标卡并覆盖内容
- 调用 `rewriteCardFile()` 用二进制方式重写整个文件

### 4.6 bizAddCard(...)
- 表示层传入原始输入字符串
- 业务层完成 trim、长度校验、金额校验
- 调用 `readCard()` 获取文件最新内容
- 调用 `isCardExists()` 检查重复卡号
- 构造完整 `Card`
- 先更新链表，再调用 `saveCard()` 落盘
- 若落盘失败，调用 `readCard()` 回滚内存状态

### 4.7 bizQueryCard(...)
- 表示层传入卡号字符串
- 业务层先调用 `readCard()` 同步文件数据到链表
- 再通过 `dataFindCardByName()` 做精确查询
- 查询成功后交给表示层表格显示

## 5. 当前实现如何在链表下完成文件持久化
1. 相关卡业务开始前，先调用 `readCard()`
2. `readCard()` 从 `data/card.bin` 读取全部二进制记录并恢复链表
3. 添加卡时，先在链表中做重复校验，再将新卡加入链表并调用 `saveCard()` 追加到文件
4. 查询卡时，链表数据来自文件最新内容，再做精确匹配
5. 更新卡时，先改链表节点，再调用 `updateCard()` 重写整个文件
6. 程序结束时调用 `dataCleanup()` 释放链表

这意味着：
- 链表是运行态缓存与查询载体
- 二进制文件是唯一持久化数据源
- 二者通过 `readCard/saveCard/updateCard` 保持一致

## 6. 已完成测试
默认测试：
```bash
make test
```

批量测试：
```bash
make test-batch LAB=exp3 TEST_GROUP=acceptance
make test-batch LAB=exp3 TEST_GROUP=regression
```

当前 `exp3` 已覆盖：
1. 正常添加卡并落盘
2. 重复卡号拒绝添加
3. 密码超长拒绝添加
4. 卡号或密码包含非法字符时拒绝添加且文件不被污染
5. 允许字符集 `A-Za-z0-9_@#$%!` 可正常保存和查询
6. 查询存在的卡
7. 查询不存在的卡
8. 数据文件不存在时的查询提示
9. 空二进制文件按无记录处理
10. 损坏二进制记录保护
11. 程序重启后的持久化查询验证
12. `readCard/getCardCount/isCardExists/updateCard` 的底层协同验证
