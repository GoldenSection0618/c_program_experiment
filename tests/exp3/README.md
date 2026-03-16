# exp3 测试套件说明

## 1. 验收套件
- `menu`：菜单主流程，覆盖添加卡、重复卡号、密码超长、查询存在与不存在，并校验落盘文件内容
- `menu_add_card_pipe` / `menu_add_password_pipe`：输入包含分隔符 `|` 时拒绝添加，防止文本记录被破坏
- `menu_add_card_dash_invalid` / `menu_add_password_dash_invalid`：输入包含白名单外字符时拒绝添加
- `menu_add_special_chars_valid`：允许字符集 `A-Za-z0-9_@#$%!` 可正常保存和查询
- `menu_query_missing_file`：文件不存在时的查询提示
- `menu_query_empty_file`：空文件查询提示
- `menu_record_format_error`：损坏记录格式保护
- `menu_time_parse_error`：时间字段解析保护
- `menu_persist_across_restart`：新增后重启程序仍可查询
- `repository_api`：底层 `readCard/saveCard/getCardCount/isCardExists/updateCard` 协同检查

## 2. 运行方式
```bash
make test
make test-batch LAB=exp3 TEST_GROUP=acceptance
make test-batch LAB=exp3 TEST_GROUP=regression
```

`run_smoke_test.sh` 已支持套件级 `*.setup.sh` / `*.verify.sh` 钩子，用于准备文件夹、校验落盘内容和做重启验证。
