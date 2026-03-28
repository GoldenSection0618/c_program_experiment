# 实验五：模糊查询确认

## 1. 本轮目标
本轮对应 Iteration 5.6，目标是在当前“文件 -> 链表 -> 精确查询”基础上，补齐“文件 -> 链表 -> 模糊查询”能力，并把菜单 2 正式升级为实验五查询入口。

## 2. 当前模糊查询主线
当前仓库中的模糊查询职责已经明确分层：
- `readCard()`：负责从 `data/cards.txt` 读取全部卡记录，并恢复到链表
- `bizFuzzyQueryCards()`：负责输入规范化、触发链表恢复、按关键字生成结果集并返回业务结果
- `dataCountCardsByKeyword()` / `dataCopyCardsByKeyword()`：负责在链表中做“卡号包含关键字”的匹配并复制结果
- `viewShowFuzzyQueryResults()`：负责以表格形式输出全部命中结果

## 3. 当前查询入口
当前菜单 2 已升级为二级查询入口：
1. 精确查询
2. 模糊查询

进入模糊查询后，程序按以下顺序工作：
1. 读取用户输入关键字
2. `readCard()` 清空旧链表并从文件重建
3. 遍历链表，判断卡号是否包含该关键字
4. 把所有命中项复制到结果集
5. 有结果则按表格输出
6. 无结果则输出“没有符合关键字的卡信息！”

## 4. 匹配规则
当前模糊查询采用最直接的包含匹配：
- 规则：`strstr(card->aCardName, keyword) != NULL`
- 不做大小写折叠
- 不支持通配符
- 关键字字符集与卡号一致：`1~18` 位，只允许大小写字母、数字和 `_ @ # $ % !`

## 5. 实验五专属验证
本轮新增实验五测试套件：
- [tests/exp5/menu_query_card_fuzzy_found.input](../tests/exp5/menu_query_card_fuzzy_found.input)
- [tests/exp5/menu_query_card_fuzzy_found.expect.tsv](../tests/exp5/menu_query_card_fuzzy_found.expect.tsv)
- [tests/exp5/menu_query_card_fuzzy_missing.input](../tests/exp5/menu_query_card_fuzzy_missing.input)
- [tests/exp5/menu_query_card_fuzzy_missing.expect.tsv](../tests/exp5/menu_query_card_fuzzy_missing.expect.tsv)

这两组测试分别验证：
1. 关键字能命中多张卡，并全部输出
2. 关键字无结果时有明确提示

## 6. 本轮结论
Iteration 5.6 的结论是：
- 当前模糊查询已经完成
- 实验五要求的“精确查询 + 模糊查询”主路径已经补齐
- 5.7 可以把重点放到输入校验与异常场景补全
