# exp7 测试套件说明

## 1. 验收套件
- `money_repository_api`：实验七充值退费记录数据层验证，覆盖保存、读取、计数、最近记录查询、缺失文件和空文件处理
- `menu_recharge_success`：离线卡充值成功，验证卡余额更新与充值记录追加
- `menu_recharge_online_success`：正在上机卡也允许充值
- `menu_recharge_card_missing`：卡不存在时充值失败，且不污染卡文件和 money 文件
- `menu_recharge_password_wrong`：密码错误时充值失败，且不污染卡文件和 money 文件
- `menu_recharge_card_canceled`：已注销卡禁止充值
- `menu_recharge_amount_invalid`：非法充值金额被拒绝，且不污染卡文件和 money 文件

## 2. 运行方式
```bash
make test LAB=exp7 TEST_SUITE=money_repository_api
make test LAB=exp7 TEST_SUITE=menu_recharge_success
make test LAB=exp7 TEST_SUITE=menu_recharge_online_success
make test LAB=exp7 TEST_SUITE=menu_recharge_card_missing
make test LAB=exp7 TEST_SUITE=menu_recharge_password_wrong
make test LAB=exp7 TEST_SUITE=menu_recharge_card_canceled
make test LAB=exp7 TEST_SUITE=menu_recharge_amount_invalid
make test-batch LAB=exp7 TEST_GROUP=acceptance
```

## 3. 当前阶段说明
- 本阶段已完成 `Money` 数据层与菜单 `5. 充值` 主流程
- 暂不实现菜单 `6. 退费` 与 `8. 注销卡`
- 记录文件固定为 `data/money.txt`
