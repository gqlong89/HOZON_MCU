#ifndef __BRDINFO_H__
#define __BRDINFO_H__

void brd_info_init(void);
const char *brd_get_btlVerStr(void);
const char *brd_get_hwVerStr(void);
const char *brd_get_bomVerStr(void);
/* example: x15006, x15003, without leading 'x' or 'X' */
unsigned int brd_get_projectCode(void);
/* example: S0, S1, S2 without leading 's' or 'S' */
unsigned int brd_get_boardRevision(void);
unsigned int brd_get_bomRevision(void);
const char *brd_get_btlVerStr(void);

#endif

