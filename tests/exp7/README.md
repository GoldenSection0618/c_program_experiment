# exp7 测试套件说明

## 1. 验收套件
- `money_repository_api`：实验七充值退费记录数据层验证，覆盖保存、读取、计数、最近记录查询、缺失文件和空文件处理

## 2. 运行方式
```bash
make test LAB=exp7 TEST_SUITE=money_repository_api
make test-batch LAB=exp7 TEST_GROUP=acceptance
```

## 3. 当前阶段说明
- 本阶段只补 `Money` 数据层
- 不接入菜单 5、6、8 业务流程
- 记录文件固定为 `data/money.txt`
