#include "menu.h"
#include "common.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void discardRemainingInputLine(void)
{
    int ch = 0;

    do {
        ch = getchar();
    } while (ch != '\n' && ch != EOF);
}

int readTextInput(const char *prompt, char *buffer, size_t size)
{
    char *newLine = NULL;

    if (prompt == NULL || buffer == NULL || size == 0) {
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

void outputMenu(void)
{
    printf("%s\n", STUDENT_INFO);
    printf("========== %s ==========\n", SYSTEM_NAME);
    printf("1. 添加卡\n");
    printf("2. 查询卡\n");
    printf("3. 上机\n");
    printf("4. 下机\n");
    printf("5. 充值\n");
    printf("6. 退费\n");
    printf("7. 查询统计\n");
    printf("8. 注销卡\n");
    printf("0. 退出系统\n");
}

int readChoiceInput(const char *prompt, int *choice)
{
    char buffer[INPUT_BUF_SIZE];
    char *endptr = NULL;
    long value = 0;

    if (choice == NULL) {
        return -1;
    }

    if (readTextInput(prompt, buffer, sizeof(buffer)) != 0) {
        return -1;
    }

    errno = 0;
    value = strtol(buffer, &endptr, 10);

    if (endptr == buffer || errno == ERANGE) {
        return -1;
    }

    while (*endptr != '\0' && isspace((unsigned char)*endptr)) {
        endptr++;
    }

    if (*endptr != '\0') {
        return -1;
    }

    if (value < INT_MIN || value > INT_MAX) {
        return -1;
    }

    *choice = (int)value;
    return 0;
}

int readMenuChoice(int *choice)
{
    return readChoiceInput("请输入菜单编号：", choice);
}
