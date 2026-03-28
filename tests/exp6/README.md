# exp6 测试套件说明

## 1. 验收套件
- `billing_repository_api`：实验六消费记录基础设施验证，覆盖保存、读取、查找未结算记录、更新记录、程序重启后重载
- `menu_start_billing_success`：实验六上机成功验证，覆盖菜单 3、卡状态更新、未结算消费记录追加与结果展示
- `menu_start_billing_card_missing`：实验六上机失败验证，覆盖卡不存在场景
- `menu_start_billing_password_wrong`：实验六上机失败验证，覆盖密码错误场景
- `menu_start_billing_card_unavailable`：实验六上机失败验证，覆盖卡正在上机或已注销场景
- `menu_stop_billing_success`：实验六下机成功验证，覆盖菜单 4、卡状态恢复、余额扣减与消费记录结算
- `menu_stop_billing_no_unsettled`：实验六下机失败验证，覆盖未找到未结算记录场景
- `menu_stop_billing_balance_not_enough`：实验六下机失败验证，覆盖余额不足场景
- `menu_stop_billing_password_wrong`：实验六下机失败验证，覆盖密码错误场景

## 2. 运行方式
```bash
make test LAB=exp6 TEST_SUITE=billing_repository_api
make test LAB=exp6 TEST_SUITE=menu_start_billing_success
make test LAB=exp6 TEST_SUITE=menu_start_billing_card_missing
make test LAB=exp6 TEST_SUITE=menu_start_billing_password_wrong
make test LAB=exp6 TEST_SUITE=menu_start_billing_card_unavailable
make test LAB=exp6 TEST_SUITE=menu_stop_billing_success
make test LAB=exp6 TEST_SUITE=menu_stop_billing_no_unsettled
make test LAB=exp6 TEST_SUITE=menu_stop_billing_balance_not_enough
make test LAB=exp6 TEST_SUITE=menu_stop_billing_password_wrong
make test-batch LAB=exp6 TEST_GROUP=acceptance
```

当前迭代 3 在上机主流程基础上，补齐了菜单 4 的下机结算闭环。
