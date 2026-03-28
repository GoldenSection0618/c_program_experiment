# exp6 测试套件说明

## 1. 验收套件
- `billing_repository_api`：实验六消费记录基础设施验证，覆盖保存、读取、查找未结算记录、更新记录、程序重启后重载

## 2. 运行方式
```bash
make test LAB=exp6 TEST_SUITE=billing_repository_api
make test-batch LAB=exp6 TEST_GROUP=acceptance
```

当前迭代 1 只验证消费记录模型与存储，不进入上机 / 下机业务流程。
