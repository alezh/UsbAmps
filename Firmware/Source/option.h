#ifndef OPTION_H
#define OPTION_H

#include <stdbool.h>



#define OPTION_AVERAGE_COUNT  60
/* Defines amount of smoothing for readings. Higher value will smooth readings
 * and make them more stable but at the cost of speed. Default value is 60. */

#define OPTION_LONG_PRESS_MULTIPLIER  6
/* Defines ratio of long press to measurement cycle. If you lower
 * SETTINGS_AVERAGE_COUNT then this number has to be increased in order to keep
 * same timing. Default value is 6. */

#define OPTION_LONG_BLINK_MULTIPLIER  12
/* Defines number of measurement cycles before min/max blink will happen
 * (assuming that SETTINGS_BLINK_ON_MIN_MAX is set). If you lower
 * SETTINGS_AVERAGE_COUNT then this number has to be increased in order to keep
 * same timing. Default value is 12. */

#define OPTION_THREE_DIGIT_MILLIAMPS  false
/* Defines whether values between 100 and 999 milliamps will be shown with two
 * digits (e.g. 0.54 when setting is false) or with three (542 when setting is
 * true). Default value is false. */

#define OPTION_BLINK_ON_MIN_MAX  true
/* Defines whether display will occasionally blink when non-actual value (i.e.
 * min/max) is shown. Default value is true. */

#define OPTION_SHOW_CAPACITY  false
/* Defines whether capacity is shown as fourth measurement option. */

#define OPTION_DSHORT_CDP_ENABLED  false
/* defines if one can attempt to enter Charging Downstream Port Hi-power mode
 * by holding outer button during startup. Disabled by default because it works
 * only occasionally (highly timing dependant) and it can get easily triggered
 * by accident. */

#define OPTION_DSHORT_CDP_COUNT  12
/* Defines the duration of the data lines short circuit in tens of milliseconds
 * during the Charging Downstream Port Hi-power mode turn up. Proper value will
 * negotiate Charging Downstream Port functionality (1.5A and data). */

#define OPTION_STARTUP_WAIT_BUTTON_RELEASE_FOREVER  false
/* If value is true, UsbAmps will wait forever for button release on startup.
 * If value is false, UsbAmps will wait for button release or one second,
 * whichever comes first. */


#if OPTION_SHOW_CAPACITY
    #define OPTION_COUNT 4
#else
    #define OPTION_COUNT 3
#endif

#define OPTION_UNIT_CURRENT   0
#define OPTION_UNIT_VOLTAGE   1
#define OPTION_UNIT_POWER     2
#define OPTION_UNIT_CAPACITY  3

#define OPTION_UNITTYPE_CURRENT_AVG       0
#define OPTION_UNITTYPE_CURRENT_MAX       1
#define OPTION_UNITTYPE_CURRENT_MIN       2
#define OPTION_UNITTYPE_VOLTAGE_AVG       3
#define OPTION_UNITTYPE_VOLTAGE_MAX       4
#define OPTION_UNITTYPE_VOLTAGE_MIN       5
#define OPTION_UNITTYPE_POWER_AVG         6
#define OPTION_UNITTYPE_POWER_MAX         7
#define OPTION_UNITTYPE_POWER_MIN         8
#define OPTION_UNITTYPE_CAPACITY_MINUTE   9
#define OPTION_UNITTYPE_CAPACITY_HOUR    10
#define OPTION_UNITTYPE_CAPACITY_NA      11

#endif
