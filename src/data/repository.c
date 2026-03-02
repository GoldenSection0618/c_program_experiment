#include "data.h"

void dataLogOperation(const char *operation)
{
#if ENABLE_LOG
    printf("[数据存储层] 操作记录：%s\n", operation);
#else
    (void)operation;
#endif
}
