/**
 * @Title: scom.h
 * @author yuzhimin
 * @date Aug 10, 2017
 * @version V1.0
 */

#ifndef __SCOM_H__
#define __SCOM_H__

typedef int (*scom_custom_msg_cb)(unsigned char msgid, unsigned char *body, unsigned int len);

unsigned int scom_task(unsigned int mask);
unsigned int scom_get_fault_cnt(void);
void scom_clear_fault_cnt(void);
void scom_reg_custom_msg_cb(scom_custom_msg_cb callback);

#endif

