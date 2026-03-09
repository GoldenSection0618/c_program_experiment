#include "card_view.h"

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

static int utf8DisplayWidth(const char *text)
{
    const unsigned char *p = (const unsigned char *)text;
    int width = 0;

    if (text == NULL) {
        return 0;
    }

    while (*p != '\0') {
        if ((*p & 0x80) == 0) {
            width += 1;
            p++;
            continue;
        }

        if ((*p & 0xE0) == 0xC0) {
            p += (p[1] == '\0') ? 1 : 2;
        } else if ((*p & 0xF0) == 0xE0) {
            p += (p[1] == '\0' || p[2] == '\0') ? 1 : 3;
        } else if ((*p & 0xF8) == 0xF0) {
            p += (p[1] == '\0' || p[2] == '\0' || p[3] == '\0') ? 1 : 4;
        } else {
            p += 1;
        }
        width += 2;
    }

    return width;
}

static void printSpaces(int count)
{
    int i = 0;

    for (i = 0; i < count; i++) {
        putchar(' ');
    }
}

static void printCellLeftUtf8(const char *text, int width)
{
    int pad = 0;
    const char *safeText = text == NULL ? "" : text;

    fputs(safeText, stdout);
    pad = width - utf8DisplayWidth(safeText);
    if (pad > 0) {
        printSpaces(pad);
    }
}

static void printCellRightAscii(const char *text, int width)
{
    int pad = 0;
    const char *safeText = text == NULL ? "" : text;

    pad = width - (int)strlen(safeText);
    if (pad > 0) {
        printSpaces(pad);
    }
    fputs(safeText, stdout);
}

static void printCellRule(int width)
{
    putchar('+');
    while (width-- > 0) {
        putchar('-');
    }
}

static void formatMoneyFromCent(int32_t amountCent, char *buffer, size_t size)
{
    int32_t absCent = amountCent;
    int32_t yuan = 0;
    int32_t cent = 0;

    if (buffer == NULL || size == 0) {
        return;
    }

    if (amountCent < 0) {
        absCent = -amountCent;
    }

    yuan = absCent / 100;
    cent = absCent % 100;

    if (amountCent < 0) {
        snprintf(buffer, size, "-%d.%02d", yuan, cent);
    } else {
        snprintf(buffer, size, "%d.%02d", yuan, cent);
    }
}

void viewShowCardSummary(const Card *card)
{
    const int cardColWidth = 18;
    const int pwdColWidth = 8;
    const int statusColWidth = 8;
    const int balanceColWidth = 10;
    char balanceBuf[32];

    if (card == NULL) {
        return;
    }

    formatMoneyFromCent(card->nBalanceCent, balanceBuf, sizeof(balanceBuf));

    printCellRule(cardColWidth + 2);
    printCellRule(pwdColWidth + 2);
    printCellRule(statusColWidth + 2);
    printCellRule(balanceColWidth + 2);
    printf("+\n");

    printf("| ");
    printCellLeftUtf8("卡号", cardColWidth);
    printf(" | ");
    printCellLeftUtf8("密码", pwdColWidth);
    printf(" | ");
    printCellLeftUtf8("状态", statusColWidth);
    printf(" | ");
    printCellLeftUtf8("余额", balanceColWidth);
    printf(" |\n");

    printCellRule(cardColWidth + 2);
    printCellRule(pwdColWidth + 2);
    printCellRule(statusColWidth + 2);
    printCellRule(balanceColWidth + 2);
    printf("+\n");

    printf("| ");
    printCellLeftUtf8(card->aCardName, cardColWidth);
    printf(" | ");
    printCellLeftUtf8(card->aPwd, pwdColWidth);
    printf(" | ");
    printCellLeftUtf8(cardStatusToText(card->nStatus), statusColWidth);
    printf(" | ");
    printCellRightAscii(balanceBuf, balanceColWidth);
    printf(" |\n");

    printCellRule(cardColWidth + 2);
    printCellRule(pwdColWidth + 2);
    printCellRule(statusColWidth + 2);
    printCellRule(balanceColWidth + 2);
    printf("+\n");
}

void viewShowQueryCardDetails(const Card *card)
{
    const int cardColWidth = 18;
    const int statusColWidth = 8;
    const int balanceColWidth = 10;
    const int totalUseColWidth = 10;
    const int useCountColWidth = 8;
    const int lastUseTimeColWidth = 19;
    char lastUseBuf[32];
    char balanceBuf[32];
    char totalUseBuf[32];
    char useCountBuf[16];

    if (card == NULL) {
        return;
    }

    formatTimeString(card->tLast, lastUseBuf, sizeof(lastUseBuf));
    formatMoneyFromCent(card->nBalanceCent, balanceBuf, sizeof(balanceBuf));
    formatMoneyFromCent(card->nTotalUseCent, totalUseBuf, sizeof(totalUseBuf));
    snprintf(useCountBuf, sizeof(useCountBuf), "%d", card->nUseCount);

    printf("查询结果：\n");
    printCellRule(cardColWidth + 2);
    printCellRule(statusColWidth + 2);
    printCellRule(balanceColWidth + 2);
    printCellRule(totalUseColWidth + 2);
    printCellRule(useCountColWidth + 2);
    printCellRule(lastUseTimeColWidth + 2);
    printf("+\n");

    printf("| ");
    printCellLeftUtf8("卡号", cardColWidth);
    printf(" | ");
    printCellLeftUtf8("卡状态", statusColWidth);
    printf(" | ");
    printCellLeftUtf8("余额", balanceColWidth);
    printf(" | ");
    printCellLeftUtf8("累计使用", totalUseColWidth);
    printf(" | ");
    printCellLeftUtf8("使用次数", useCountColWidth);
    printf(" | ");
    printCellLeftUtf8("最后使用时间", lastUseTimeColWidth);
    printf(" |\n");

    printCellRule(cardColWidth + 2);
    printCellRule(statusColWidth + 2);
    printCellRule(balanceColWidth + 2);
    printCellRule(totalUseColWidth + 2);
    printCellRule(useCountColWidth + 2);
    printCellRule(lastUseTimeColWidth + 2);
    printf("+\n");

    printf("| ");
    printCellLeftUtf8(card->aCardName, cardColWidth);
    printf(" | ");
    printCellLeftUtf8(cardStatusToText(card->nStatus), statusColWidth);
    printf(" | ");
    printCellRightAscii(balanceBuf, balanceColWidth);
    printf(" | ");
    printCellRightAscii(totalUseBuf, totalUseColWidth);
    printf(" | ");
    printCellRightAscii(useCountBuf, useCountColWidth);
    printf(" | ");
    printCellLeftUtf8(lastUseBuf, lastUseTimeColWidth);
    printf(" |\n");

    printCellRule(cardColWidth + 2);
    printCellRule(statusColWidth + 2);
    printCellRule(balanceColWidth + 2);
    printCellRule(totalUseColWidth + 2);
    printCellRule(useCountColWidth + 2);
    printCellRule(lastUseTimeColWidth + 2);
    printf("+\n");
}
