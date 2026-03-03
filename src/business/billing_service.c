#include "business.h"
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

static int readAmount(const char *prompt, float *amount)
{
    char buf[INPUT_BUF_SIZE];
    char *endptr = NULL;
    float value = 0.0f;

    if (amount == NULL) {
        return -1;
    }

    if (readLine(prompt, buf, sizeof(buf)) != 0) {
        return -1;
    }

    errno = 0;
    value = strtof(buf, &endptr);
    if (endptr == buf || errno == ERANGE) {
        return -1;
    }

    while (*endptr != '\0' && isspace((unsigned char)*endptr)) {
        endptr++;
    }

    if (*endptr != '\0' || value < 0.0f) {
        return -1;
    }

    *amount = value;
    return 0;
}

static const char *cardStatusToText(int status)
{
    switch (status) {
    case CARD_STATUS_OFFLINE:
        return "未上机";
    case CARD_STATUS_ONLINE:
        return "正在上机";
    case CARD_STATUS_CANCELED:
        return "已注销";
    case CARD_STATUS_INVALID:
        return "失效";
    default:
        return "未知";
    }
}

static void formatTimeString(time_t ts, char *buffer, size_t size)
{
    struct tm *local = NULL;

    if (buffer == NULL || size == 0) {
        return;
    }

    if (ts == 0) {
        snprintf(buffer, size, "-");
        return;
    }

    local = localtime(&ts);
    if (local == NULL) {
        snprintf(buffer, size, "-");
        return;
    }

    if (strftime(buffer, size, "%Y-%m-%d %H:%M:%S", local) == 0) {
        snprintf(buffer, size, "-");
    }
}

static void showCardSummary(const Card *card)
{
    if (card == NULL) {
        return;
    }

    printf("卡号\t密码\t状态\t余额\n");
    printf("%s\t%s\t%s\t%.2f\n", card->aCardName, card->aPwd, cardStatusToText(card->nStatus), card->fBalance);
}

static void showQueryCardDetails(const Card *card)
{
    char lastUseBuf[32];

    if (card == NULL) {
        return;
    }

    formatTimeString(card->tLast, lastUseBuf, sizeof(lastUseBuf));
    printf("查询结果：\n");
    printf("卡号：%s\n", card->aCardName);
    printf("卡状态：%s\n", cardStatusToText(card->nStatus));
    printf("余额：%.2f\n", card->fBalance);
    printf("累计使用金额：%.2f\n", card->fTotalUse);
    printf("使用次数：%d\n", card->nUseCount);
    printf("最后使用时间：%s\n", lastUseBuf);
}

void bizAddCard(void)
{
    char cardName[INPUT_BUF_SIZE];
    char password[INPUT_BUF_SIZE];
    float amount = 0.0f;
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

    if (readAmount("请输入开卡金额（元）：", &amount) != 0) {
        printf("开卡金额输入不合法，应为非负数字。\n");
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
    card.fTotalUse = 0.0f;
    card.nUseCount = 0;
    card.fBalance = amount;
    card.nDel = 0;

    ret = dataAddCard(&card);
    if (ret == DATA_ERR_FULL) {
        printf("卡库已满，无法继续新增。\n");
        return;
    }

    if (ret != DATA_OK) {
        printf("添加卡失败。\n");
        return;
    }

    printf("添加卡成功！\n");
    showCardSummary(&card);
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

    showQueryCardDetails(card);
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
