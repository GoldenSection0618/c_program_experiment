#include "data.h"
#include "card_storage.h"

static CardNode *g_pCardListHead = NULL;
static size_t g_cardCount = 0;

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

static CardNode *findCardNodeByIndex(size_t index)
{
    CardNode *pCurrent = g_pCardListHead;
    size_t currentIndex = 0;

    while (pCurrent != NULL) {
        if (currentIndex == index) {
            return pCurrent;
        }
        currentIndex++;
        pCurrent = pCurrent->pNext;
    }

    return NULL;
}

void dataLogOperation(const char *operation)
{
#if ENABLE_LOG
    printf("[数据存储层] 操作记录：%s\n", operation);
#else
    (void)operation;
#endif
}

const Card *dataFindCardByName(const char *cardName)
{
    CardNode *pNode = findCardNodeByName(cardName);

    if (pNode == NULL) {
        return NULL;
    }
    return &pNode->cardData;
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

size_t dataGetCardCount(void)
{
    return g_cardCount;
}

const Card *dataGetCardByIndex(size_t index)
{
    CardNode *pNode = findCardNodeByIndex(index);

    if (pNode == NULL) {
        return NULL;
    }
    return &pNode->cardData;
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
