#ifndef CARD_VIEW_H
#define CARD_VIEW_H

#include <stddef.h>

#include "model.h"

void viewShowCardSummary(const Card *card);
void viewShowQueryCardDetails(const Card *card);
void viewShowFuzzyQueryResults(const char *keyword, const Card *cards, size_t count);

#endif
