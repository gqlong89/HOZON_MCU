/*
 * thres_diag_futian.h
 *  Created on: 20180413
 *      Author: zhouhong
 */

#ifndef _THRES_DIAG_FUTIAN_H_
#define _THRES_DIAG_FUTIAN_H_

#define FUTIAN_DIAG_ENABLE (1U)     //FUTIAN switch

#define BTN_DOWN_750_STD            (1924)                         //SOS 750 on
#define BTN_DEVIA                   (187)
#define BTN_DOWN_750_MAX            (BTN_DOWN_750_STD + BTN_DEVIA)
#define BTN_DOWN_750_MIN            (BTN_DOWN_750_STD - BTN_DEVIA)

//#define BTN_750_470_STD             (1179)                         //SOS 750 470 on
#define BTN_750_470_STD             (1620)                         //SOS 750 470 on

#define BTN_DOWN_750_470_MIN        (BTN_750_470_STD - BTN_DEVIA)
#define BTN_DOWN_750_470_MAX        (BTN_750_470_STD + BTN_DEVIA)


#define BTN_UP_STD                  (3972)                         //BTN off
//#define BTN_UP_MIN                  (3847)
#define BTN_UP_MIN                  (3500)


#define MIC_P_IN_STD                (1044)                          //7mic+ normol
#define MIC_DEVIA                   (200)
#define MIC_P_IN_MIN                (869)
#define MIC_P_IN_MAX                (1179)


#define MIC_P_OPEN_STD              (2290)                          //MIC+ open
#define MIC_P_OPEN_MIN              (1986)
#define MIC_P_OPEN_MAX              (2483)

#define MIC_P_SHORT_GND_STD         (0)
#define MIC_P_SHORT_GND_MAX         (MIC_P_SHORT_GND_STD + MIC_DEVIA)

#define MIC_P_SHORT_POWER_STD       (3630)                          //MIC+ short
#define MIC_P_SHORT_POWER_DEVIA     (400)
#define MIC_P_SHORT_POWER_MIN       (MIC_P_SHORT_POWER_STD - MIC_P_SHORT_POWER_DEVIA)
#define MIC_P_SHORT_POWER_MAX       (MIC_P_SHORT_POWER_STD + MIC_P_SHORT_POWER_DEVIA)

//SPK
#define SPK_P_IN_STD                (1852)
#define SPK_DEVIA                   (100)
#define SPK_P_IN_MAX                (SPK_P_IN_STD + SPK_DEVIA)
#define SPK_P_IN_MIN                (SPK_P_IN_STD - SPK_DEVIA)

#define SPK_N_IN_STD                (1272)
#define SPK_N_IN_MAX                (SPK_N_IN_STD + SPK_DEVIA)
#define SPK_N_IN_MIN                (SPK_N_IN_STD - SPK_DEVIA)

#define SPK_IN_DELTA_MIN            (373)

#define SPK_P_OPEN_STD              (3877)
#define SPK_P_OPEN_MAX              (SPK_P_OPEN_STD + SPK_DEVIA)
#define SPK_P_OPEN_MIN              (SPK_P_OPEN_STD - SPK_DEVIA)

#define SPK_N_OPEN_STD              (0)
#define SPK_N_OPEN_MAX              (SPK_N_OPEN_STD + SPK_DEVIA)
#define SPK_N_OPEN_MIN              (0)

#define SPK_P_SHORT_STD             (1585)
#define SPK_P_SHORT_MAX             (SPK_P_SHORT_STD + SPK_DEVIA)
#define SPK_P_SHORT_MIN             (SPK_P_SHORT_STD - SPK_DEVIA)

#define SPK_N_SHORT_STD             (1411)
#define SPK_N_SHORT_MAX             (SPK_N_SHORT_STD + SPK_DEVIA)
#define SPK_N_SHORT_MIN             (SPK_N_SHORT_STD - SPK_DEVIA)
#define SPK_SHORT_DELTA_MAX         (248)

/*GPS thred*/
#define GPS_P_OPEN_MIN              (3795)
#define GPS_N_OPEN_MIN              (3795)
#define GPS_OPEN_DELTA_MAX          (200)

#define GPS_P_SHORT_MIN             (1365)
#define GPS_P_SHORT_MAX             (1862)
#define GPS_N_SHORT_MAX             (200)
#define GPS_SHORT_DELTA_MIN         (1241)

#define GPS_P_IN_MIN                (3848)
#define GPS_N_IN_MIN                (3848)
#define GPS_IN_DELTA_MIN            (62)
#define GPS_IN_DELTA_MAX            (373)

#endif /* INCLUDE_THRES_DIAG_FUTIAN_H_ */

