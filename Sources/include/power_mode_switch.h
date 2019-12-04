#ifndef _POWER_MODE_SWITCH_H_
#define _POWER_MODE_SWITCH_H_

/* Definition of power modes indexes, as configured in Power Manager Component
 *  Refer to the Reference Manual for details about the power modes.
 */
#define HSRUN (0u) /* High speed run      */
#define RUN   (1u) /* Run                 */
#define VLPR  (2u) /* Very low power run  */
#define STOP  (3u) /* Stop mode           */
#define STOP1 (4u) /* Stop option 1       */
#define STOP2 (5u) /* Stop option 2       */
#define VLPS  (6u) /* Very low power stop */

#define POWER_MODES (7u)

void power_mode_switch_init(void);
void power_mode_switch_HSRUN(void);
void power_mode_switch_RUN(void);
void power_mode_switch_VLPR(void);
void power_mode_switch_STOP(void);
void power_mode_switch_STOP1(void);
void power_mode_switch_STOP2(void);
void power_mode_switch_VLPS(void);

#endif

