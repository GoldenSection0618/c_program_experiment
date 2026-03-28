#include "data.h"
#include "card_storage.h"
#include "common.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

static CardNode *g_pCardListHead = NULL;
static size_t g_cardCount = 0;

static DataResult rewriteCardFile(void);
static DataResult ensureCardDataDir(void);
static int isCardNameEqual(const char *a, const char *b);
static CardNode *findCardNodeByName(const char *cardName);

static int isCardNameEqual(const char *a, const char *b)
{
    if (a == NULL || b == NULL) {
        return 0;
    }
    return strcmp(a, b) == 0;
}

static CardNode *findCardNodeByName(const char *cardName)
{
    CardNode *pCurrent = g_pCardListHead;

    if (cardName == NULL) {
        return NULL;
    }

    while (pCurrent != NULL) {
        if (pCurrent->cardData.nDel == 0 && isCardNameEqual(pCurrent->cardData.aCardName, cardName)) {
            return pCurrent;
        }
        pCurrent = pCurrent->pNext;
    }

    return NULL;
}

static DataResult ensureCardDataDir(void)
{
    char dirPath[INPUT_BUF_SIZE];
    char *slash = NULL;

    snprintf(dirPath, sizeof(dirPath), "%s", CARD_DATA_FILE_PATH);
    slash = strrchr(dirPath, '/');
    if (slash == NULL) {
        return DATA_OK;
    }

    *slash = '\0';
    if (dirPath[0] == '\0') {
        return DATA_OK;
    }

    if (mkdir(dirPath, 0777) != 0 && errno != EEXIST) {
        return DATA_ERR_FILE_OPEN;
    }

    return DATA_OK;
}

static DataResult rewriteCardFile(void)
{
    FILE *fp = NULL;
    CardNode *pCurrent = g_pCardListHead;
    DataResult ret = DATA_OK;

    ret = ensureCardDataDir();
    if (ret != DATA_OK) {
        return ret;
    }

    fp = fopen(CARD_DATA_FILE_PATH, "wb");
    if (fp == NULL) {
        return DATA_ERR_FILE_OPEN;
    }

    while (pCurrent != NULL) {
        if (fwrite(&pCurrent->cardData, sizeof(Card), 1, fp) != 1) {
            fclose(fp);
            return DATA_ERR_FILE_OPEN;
        }
        pCurrent = pCurrent->pNext;
    }

    if (fclose(fp) != 0) {
        return DATA_ERR_FILE_OPEN;
    }

    return DATA_OK;
}

const Card *dataFindCardByName(const char *cardName)
{
    CardNode *pNode = findCardNodeByName(cardName);

    if (pNode == NULL) {
        return NULL;
    }
    return &pNode->cardData;
}

DataResult dataDeleteCardByName(const char *cardName)
{
    CardNode *pCurrent = g_pCardListHead;
    CardNode *pPrev = NULL;

    if (cardName == NULL || *cardName == '\0') {
        return DATA_ERR_INVALID_ARG;
    }

    while (pCurrent != NULL) {
        if (isCardNameEqual(pCurrent->cardData.aCardName, cardName)) {
            if (pPrev == NULL) {
                g_pCardListHead = pCurrent->pNext;
            } else {
                pPrev->pNext = pCurrent->pNext;
            }

            free(pCurrent);
            if (g_cardCount > 0) {
                g_cardCount--;
            }
            return DATA_OK;
        }

        pPrev = pCurrent;
        pCurrent = pCurrent->pNext;
    }

    return DATA_ERR_NOT_FOUND;
}

int dataAddCard(const Card *card)
{
    CardNode *pNewNode = NULL;
    CardNode *pTail = NULL;

    if (card == NULL) {
        return DATA_ERR_INVALID_ARG;
    }

    if (findCardNodeByName(card->aCardName) != NULL) {
        return DATA_ERR_DUPLICATE;
    }

    pNewNode = (CardNode *)malloc(sizeof(CardNode));
    if (pNewNode == NULL) {
        return DATA_ERR_NO_MEMORY;
    }

    pNewNode->cardData = *card;
    pNewNode->pNext = NULL;

    if (g_pCardListHead == NULL) {
        g_pCardListHead = pNewNode;
    } else {
        pTail = g_pCardListHead;
        while (pTail->pNext != NULL) {
            pTail = pTail->pNext;
        }
        pTail->pNext = pNewNode;
    }

    g_cardCount++;
    return DATA_OK;
}

DataResult saveCard(const Card *card)
{
    FILE *fp = NULL;
    DataResult ret = DATA_OK;

    if (card == NULL) {
        return DATA_ERR_INVALID_ARG;
    }

    ret = ensureCardDataDir();
    if (ret != DATA_OK) {
        return ret;
    }

    fp = fopen(CARD_DATA_FILE_PATH, "ab");
    if (fp == NULL) {
        return DATA_ERR_FILE_OPEN;
    }

    if (fwrite(card, sizeof(Card), 1, fp) != 1) {
        fclose(fp);
        return DATA_ERR_FILE_OPEN;
    }

    if (fclose(fp) != 0) {
        return DATA_ERR_FILE_OPEN;
    }

    return DATA_OK;
}

int readCard(void)
{
    FILE *fp = NULL;
    int count = 0;

    dataCleanup();

    fp = fopen(CARD_DATA_FILE_PATH, "rb");
    if (fp == NULL) {
        if (errno == ENOENT) {
            return DATA_ERR_FILE_NOT_FOUND;
        }
        return DATA_ERR_FILE_OPEN;
    }

    while (1) {
        Card card;
        size_t readBytes = fread(&card, 1, sizeof(Card), fp);

        if (readBytes == sizeof(Card)) {
            DataResult ret = (DataResult)dataAddCard(&card);
            if (ret != DATA_OK) {
                fclose(fp);
                dataCleanup();
                return (ret == DATA_ERR_DUPLICATE) ? DATA_ERR_RECORD_FORMAT : ret;
            }
            count++;
            continue;
        }

        if (readBytes == 0) {
            if (feof(fp)) {
                break;
            }
            if (ferror(fp)) {
                fclose(fp);
                dataCleanup();
                return DATA_ERR_FILE_OPEN;
            }
        }

        fclose(fp);
        dataCleanup();
        return DATA_ERR_RECORD_FORMAT;
    }

    if (fclose(fp) != 0) {
        dataCleanup();
        return DATA_ERR_FILE_OPEN;
    }

    return count;
}

int getCardCount(void)
{
    FILE *fp = NULL;
    int count = 0;

    fp = fopen(CARD_DATA_FILE_PATH, "rb");
    if (fp == NULL) {
        if (errno == ENOENT) {
            return 0;
        }
        return DATA_ERR_FILE_OPEN;
    }

    while (1) {
        Card card;
        size_t readBytes = fread(&card, 1, sizeof(Card), fp);

        if (readBytes == sizeof(Card)) {
            count++;
            continue;
        }

        if (readBytes == 0) {
            if (feof(fp)) {
                break;
            }
            if (ferror(fp)) {
                fclose(fp);
                return DATA_ERR_FILE_OPEN;
            }
        }

        fclose(fp);
        return DATA_ERR_RECORD_FORMAT;
    }

    if (fclose(fp) != 0) {
        return DATA_ERR_FILE_OPEN;
    }

    return count;
}

int isCardExists(const char *cardName)
{
    if (cardName == NULL || *cardName == '\0') {
        return 0;
    }

    return findCardNodeByName(cardName) != NULL;
}

DataResult updateCard(const Card *card)
{
    CardNode *pNode = NULL;
    int readResult = 0;

    if (card == NULL) {
        return DATA_ERR_INVALID_ARG;
    }

    readResult = readCard();
    if (readResult < 0) {
        return (DataResult)readResult;
    }

    pNode = findCardNodeByName(card->aCardName);
    if (pNode == NULL) {
        return DATA_ERR_NOT_FOUND;
    }

    pNode->cardData = *card;
    return rewriteCardFile();
}

void dataCleanup(void)
{
    CardNode *pCurrent = g_pCardListHead;

    while (pCurrent != NULL) {
        CardNode *pNext = pCurrent->pNext;
        free(pCurrent);
        pCurrent = pNext;
    }

    g_pCardListHead = NULL;
    g_cardCount = 0;
}
