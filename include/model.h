#ifndef MODEL_H
#define MODEL_H

#include <time.h>

#define CARD_NAME_MAX_LEN 18
#define CARD_PWD_MAX_LEN 8
#define MAX_CARD_COUNT 1024

typedef enum CardStatus {
    CARD_STATUS_OFFLINE = 0,
    CARD_STATUS_ONLINE = 1,
    CARD_STATUS_CANCELED = 2,
    CARD_STATUS_INVALID = 3
} CardStatus;

typedef struct Card {
    char aCardName[CARD_NAME_MAX_LEN + 1];
    char aPwd[CARD_PWD_MAX_LEN + 1];
    int nStatus;
    time_t tStart;
    time_t tEnd;
    float fTotalUse;
    time_t tLast;
    int nUseCount;
    float fBalance;
    int nDel;
} Card;

typedef struct Billing {
    char aCardName[CARD_NAME_MAX_LEN + 1];
    time_t tStart;
    time_t tEnd;
    float fAmount;
    int nStatus;
    int nDel;
} Billing;

typedef struct LogonInfo {
    char aCardName[CARD_NAME_MAX_LEN + 1];
    time_t tStart;
    int nStatus;
    float fBalance;
} LogonInfo;

typedef struct SettleInfo {
    char aCardName[CARD_NAME_MAX_LEN + 1];
    time_t tStart;
    time_t tEnd;
    float fAmount;
    float fBalance;
} SettleInfo;

typedef struct Money {
    char aCardName[CARD_NAME_MAX_LEN + 1];
    time_t tTime;
    int nStatus;
    float fMoney;
    int nDel;
} Money;

typedef struct Rate {
    int starttime;
    int endtime;
    int unit;
    float charge;
    int ratetype;
    int del;
} Rate;

typedef struct Admin {
    char name[32];
    char pwd[32];
    int privilege;
    int del;
} Admin;

#endif
