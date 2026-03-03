#include "data.h"

static Card g_cards[MAX_CARD_COUNT];
static size_t g_card_count = 0;

static int isCardNameEqual(const char *a, const char *b)
{
    if (a == NULL || b == NULL) {
        return 0;
    }
    return strcmp(a, b) == 0;
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
    size_t i = 0;

    if (cardName == NULL) {
        return NULL;
    }

    for (i = 0; i < g_card_count; i++) {
        if (g_cards[i].nDel == 0 && isCardNameEqual(g_cards[i].aCardName, cardName)) {
            return &g_cards[i];
        }
    }

    return NULL;
}

int dataAddCard(const Card *card)
{
    if (card == NULL) {
        return DATA_ERR_INVALID_ARG;
    }

    if (g_card_count >= MAX_CARD_COUNT) {
        return DATA_ERR_FULL;
    }

    if (dataFindCardByName(card->aCardName) != NULL) {
        return DATA_ERR_DUPLICATE;
    }

    g_cards[g_card_count] = *card;
    g_card_count++;
    return DATA_OK;
}

size_t dataGetCardCount(void)
{
    return g_card_count;
}

const Card *dataGetCardByIndex(size_t index)
{
    if (index >= g_card_count) {
        return NULL;
    }
    return &g_cards[index];
}
