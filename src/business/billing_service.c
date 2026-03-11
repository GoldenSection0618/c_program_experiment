#include "business.h"
#include "card_view.h"
#include "data.h"

static void discardRemainingInputLine(void)
{
    int ch = 0;

    do {
        ch = getchar();
    } while (ch != '\n' && ch != EOF);
}

static void trimInPlace(char *text)
{
    size_t len = 0;
    size_t start = 0;
    size_t end = 0;

    if (text == NULL) {
        return;
    }

    len = strlen(text);
    if (len == 0) {
        return;
    }

    while (start < len && isspace((unsigned char)text[start])) {
        start++;
    }

    if (start == len) {
        text[0] = '\0';
        return;
    }

    end = len;
    while (end > start && isspace((unsigned char)text[end - 1])) {
        end--;
    }

    if (start > 0) {
        memmove(text, text + start, end - start);
    }
    text[end - start] = '\0';
}

static int readLine(const char *prompt, char *buffer, size_t size)
{
    char *newLine = NULL;

    if (buffer == NULL || size == 0) {
        return -1;
    }

    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return -1;
    }

    newLine = strchr(buffer, '\n');
    if (newLine == NULL) {
        discardRemainingInputLine();
        return -1;
    }
    *newLine = '\0';

    return 0;
}

static int isValidCardName(const char *cardName)
{
    size_t len = 0;

    if (cardName == NULL) {
        return 0;
    }

    len = strlen(cardName);
    return len > 0 && len <= CARD_NAME_MAX_LEN;
}

static int isValidPassword(const char *password)
{
    size_t len = 0;

    if (password == NULL) {
        return 0;
    }

    len = strlen(password);
    return len > 0 && len <= CARD_PWD_MAX_LEN;
}

static int parseMoneyToCent(const char *text, int32_t *amountCent)
{
    const char *p = text;
    int64_t yuan = 0;
    int32_t frac = 0;
    int fracDigits = 0;
    int64_t totalCent = 0;

    if (text == NULL || amountCent == NULL) {
        return -1;
    }

    if (*p == '\0') {
        return -1;
    }

    while (*p >= '0' && *p <= '9') {
        yuan = yuan * 10 + (int64_t)(*p - '0');
        if (yuan > (INT32_MAX / 100)) {
            return -1;
        }
        p++;
    }

    if (p == text) {
        return -1;
    }

    if (*p == '.') {
        p++;
        while (*p >= '0' && *p <= '9') {
            if (fracDigits >= 2) {
                return -1;
            }
            frac = (int32_t)(frac * 10 + (*p - '0'));
            fracDigits++;
            p++;
        }
    }

    if (*p != '\0') {
        return -1;
    }

    if (fracDigits == 1) {
        frac *= 10;
    }

    totalCent = yuan * 100 + frac;
    if (totalCent < 0 || totalCent > INT32_MAX) {
        return -1;
    }

    *amountCent = (int32_t)totalCent;
    return 0;
}

static int readAmountCent(const char *prompt, int32_t *amountCent)
{
    char buf[INPUT_BUF_SIZE];

    if (amountCent == NULL) {
        return -1;
    }

    if (readLine(prompt, buf, sizeof(buf)) != 0) {
        return -1;
    }
    trimInPlace(buf);

    return parseMoneyToCent(buf, amountCent);
}

void bizAddCard(void)
{
    char cardName[INPUT_BUF_SIZE];
    char password[INPUT_BUF_SIZE];
    int32_t amountCent = 0;
    Card card;
    time_t now = 0;
    int ret = DATA_OK;

    if (readLine("请输入卡号（1~18位）：", cardName, sizeof(cardName)) != 0) {
        printf("卡号输入不合法，应为1~18位且不能为空。\n");
        return;
    }
    trimInPlace(cardName);
    if (!isValidCardName(cardName)) {
        printf("卡号输入不合法，应为1~18位且不能为空。\n");
        return;
    }

    if (dataFindCardByName(cardName) != NULL) {
        printf("卡号已存在，不能重复添加！\n");
        return;
    }

    if (readLine("请输入密码（1~8位）：", password, sizeof(password)) != 0) {
        printf("密码输入不合法，应为1~8位且不能为空。\n");
        return;
    }
    trimInPlace(password);
    if (!isValidPassword(password)) {
        printf("密码输入不合法，应为1~8位且不能为空。\n");
        return;
    }

    if (readAmountCent("请输入开卡金额（元）：", &amountCent) != 0) {
        printf("开卡金额输入不合法，应为非负数字。\n");
        return;
    }
    if (amountCent >= MAX_BALANCE_CENT) {
        printf("余额过大，卡内余额必须小于1000000元。\n");
        return;
    }

    memset(&card, 0, sizeof(card));
    snprintf(card.aCardName, sizeof(card.aCardName), "%s", cardName);
    snprintf(card.aPwd, sizeof(card.aPwd), "%s", password);
    card.nStatus = CARD_STATUS_OFFLINE;
    now = time(NULL);
    card.tStart = now;
    card.tEnd = now + (time_t)(365 * 24 * 60 * 60);
    card.tLast = now;
    card.nTotalUseCent = 0;
    card.nUseCount = 0;
    card.nBalanceCent = amountCent;
    card.nDel = 0;

    ret = dataAddCard(&card);
    if (ret == DATA_ERR_NO_MEMORY) {
        printf("系统内存不足，无法继续新增。\n");
        return;
    }

    if (ret != DATA_OK) {
        printf("添加卡失败。\n");
        return;
    }

    printf("添加卡成功！\n");
    viewShowCardSummary(&card);
    dataLogOperation("添加卡");
}

void bizQueryCard(void)
{
    char cardName[INPUT_BUF_SIZE];
    const Card *card = NULL;

    if (readLine("请输入卡号（1~18位）：", cardName, sizeof(cardName)) != 0) {
        printf("卡号输入不合法，应为1~18位且不能为空。\n");
        return;
    }
    trimInPlace(cardName);
    if (!isValidCardName(cardName)) {
        printf("卡号输入不合法，应为1~18位且不能为空。\n");
        return;
    }

    card = dataFindCardByName(cardName);
    if (card == NULL) {
        printf("没有该卡的信息！\n");
        return;
    }

    viewShowQueryCardDetails(card);
    dataLogOperation("查询卡");
}

void bizStartBilling(void)
{
    printf("[业务逻辑层] 上机计费功能入口。\n");
    dataLogOperation("上机");
}

void bizStopBilling(void)
{
    printf("[业务逻辑层] 下机计费功能入口。\n");
    dataLogOperation("下机");
}

void bizRecharge(void)
{
    printf("[业务逻辑层] 充值功能入口。\n");
    dataLogOperation("充值");
}

void bizRefund(void)
{
    printf("[业务逻辑层] 退费功能入口。\n");
    dataLogOperation("退费");
}

void bizStatistics(void)
{
    printf("[业务逻辑层] 查询统计功能入口。\n");
    dataLogOperation("查询统计");
}

void bizCancelCard(void)
{
    printf("[业务逻辑层] 注销卡功能入口。\n");
    dataLogOperation("注销卡");
}

void bizShutdown(void)
{
    dataCleanup();
}
