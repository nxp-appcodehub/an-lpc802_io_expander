/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef __BM_MQ_H__
#define __BM_MQ_H__

#ifdef __cplusplus
extern "C" {
#endif
    

#include <stdint.h>

typedef struct
{
    uint8_t     cmd;
    uint8_t     reg;
    uint8_t     data;
    uint16_t    len;
    void*       payload;
}msg_t;


void mq_init(void);
uint8_t mq_exist(void);
msg_t *mq_pop(void);
uint8_t mq_push(msg_t pMsg);
uint8_t mq_get_empty(void);


#ifdef __cplusplus
}
#endif


#endif
