# exp5 测试套件说明

## 1. 验收套件
- `menu_persist_linked_flow`：实验五文件-链表打通验证，覆盖首次添加、程序重启后查询、文件恢复后链表查找
- `menu_add_card_linked`：实验五链表版添加卡验证，覆盖菜单 1 正常添加、链表插入与文件恢复后查找
- `repository_list_foundation`：实验五链表基础设施验证，覆盖空链表、首结点插入、多结点尾插、重复卡号拦截、删除头/中/尾结点，以及 `dataCleanup()` 后重建场景

## 2. 运行方式
```bash
make test LAB=exp5 TEST_SUITE=repository_list_foundation
make test-batch LAB=exp5 TEST_GROUP=acceptance
```

当前 5.3 在 5.2 基础上补充“链表版添加卡”验证，不额外修改现有添加卡行为。

当前 5.2 测试只验证链表基础设施本身，不混入文件持久化和模糊查询业务。
