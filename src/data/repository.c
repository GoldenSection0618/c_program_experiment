#include "data.h"
#include "card_storage.h"

#include <stdlib.h>
#include <string.h>

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
