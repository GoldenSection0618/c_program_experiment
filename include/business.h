#ifndef BUSINESS_H
#define BUSINESS_H

#include "model.h"

int bizAddCard(Card *createdCard);
int bizQueryCard(Card *queriedCard);
void bizStartBilling(void);
void bizStopBilling(void);
void bizRecharge(void);
void bizRefund(void);
void bizStatistics(void);
void bizCancelCard(void);
void bizShutdown(void);

#endif
