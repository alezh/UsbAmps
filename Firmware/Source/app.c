#include <pic16f1934.h>
#include <pic.h>
#include <limits.h>
#include "calibrate.h"
#include "config.h"
#include "lcd.h"
#include "measure.h"
#include "touch.h"
#include "settings.h"
#include "io.h"


void measure();
void resetMinMax();
void showMeasurement(unsigned char unitIndex, unsigned char typeIndex);
unsigned char getButtonMask();


void main() {
    init();
    lcd_init();
    measure_init();
    touch_init();
    io_init();

    lcd_writeLoading();


    if (settings_getAdcCurrentOffset() == INT_MAX) { //on first run you are still connected to PICkit; just wait
        clrwdt();
        wait_250ms(); //we ignore short power-up/down events
        settings_setAdcCurrentOffset(INT_MAX-1);
        while(true) {
            clrwdt();
            lcd_clear();
            wait_250ms();
            clrwdt();
            lcd_writeLoading();
            wait_250ms();
        }
    } else if (settings_getAdcCurrentOffset() == INT_MAX-1) { //on first real run do calibration
        while (touch_inner_pressed() || touch_outer_pressed()) {  clrwdt(); } //both buttons have to be released
        lcd_writeCalibration();
        wait_250ms();
        calibrate();
        measure_reinit();
    } else if (touch_outer_pressed() && touch_inner_pressed()) { //both keys are needed to enter calibrate
        while (touch_outer_pressed()) {  clrwdt(); } //outer button needs to be released first
        if (touch_inner_pressed()) { //innter is still presed
            lcd_writeCalibration();
            while (touch_inner_pressed() || touch_outer_pressed()) {  clrwdt(); } //wait for b2 to be released also
            calibrate();
            measure_reinit();
        }
    } else if (touch_inner_pressed()) { //inner key signifies High power mode (no USB connectivity)
        lcd_writeHighPower();
        io_dshort_on();
        while (touch_inner_pressed() || touch_outer_pressed()) {  clrwdt(); } //both keys must be released
    }


    unsigned char unitIndex = 0; //0:current; 1:voltage; 2:power
    unsigned char typeIndex = 0; //0:avg; 1:max; 2:min

    while (true) {
        measure();

        unsigned char buttons = getButtonMask();
        if (buttons != 0) {
            unsigned char counter = 0;
            unsigned char nextButtons = buttons;
            while (nextButtons != 0) {
                counter = (counter + 1) % 2;
                measure();
                nextButtons = getButtonMask();
                buttons |= nextButtons;
                switch (buttons) {
                    case 0x01: lcd_writeUnitAndType((unitIndex + 1) % 3, 0); break; //current/vout/powerout
                    case 0x02: lcd_writeUnitAndType(unitIndex, (typeIndex + 1) % 3); break; //avg/max/min
                    case 0x03: //reset min/max
                        if (counter == 0) {
                            lcd_clear();
                        } else {
                            lcd_writeUnitAndType(unitIndex, typeIndex);
                        }
                        break;
                }
            }
            switch (buttons) {
                case 0x01: //current/vout/powerout
                    unitIndex = (unitIndex + 1) % 3;
                    typeIndex = 0;
                    break;
                case 0x02: //avg/max/min
                    typeIndex = (typeIndex + 1) % 3;
                    break;
                case 0x03: //reset min/max
                    resetMinMax();
                    break;
            }
        } else {
            showMeasurement(unitIndex, typeIndex);
        }
    }
}

unsigned char getButtonMask() {
    unsigned char mask = 0x00;
    if (touch_outer_pressed()) { mask |= 0x01; }
    if (touch_inner_pressed()) { mask |= 0x02; }
    return mask;
}


void processMinMax(unsigned int value, unsigned int *min, unsigned int *max) {
    if (value == INT_MAX) { return; }
    if (*min == INT_MAX) { *min = value; } else if (value < *min) { *min = value; }
    if (*max == INT_MAX) { *max = value; } else if (value > *max) { *max = value; }
}

void processAvg(unsigned long sum, unsigned int count, unsigned int *avg) {
    if ((sum == INT_MAX) || (count == 0)) {
        *avg = INT_MAX;
    } else {
        unsigned long value = sum / count;
        *avg = (unsigned int)value;
    }
}


unsigned int AvgCurrent = INT_MAX, MinCurrent = INT_MAX, MaxCurrent = INT_MAX;
unsigned int AvgVoltage = INT_MAX, MinVoltage = INT_MAX, MaxVoltage = INT_MAX;
unsigned int AvgPower   = INT_MAX, MinPower   = INT_MAX, MaxPower   = INT_MAX;

void measure() {
    clrwdt();
    unsigned long sumCurrent = 0, sumVoltage = 0, sumPower = 0;
    unsigned int  cntCurrent = 0, cntVoltage = 0, cntPower = 0;
    for (unsigned char i=0; i<SETTINGS_AVERAGE_COUNT; i++) {
        unsigned int current = measure_getCurrent_1m();
        unsigned int voltage = measure_getVoltageOut_1m();
        unsigned int power   = ((current == INT_MAX) || (voltage == INT_MAX)) ? INT_MAX : (unsigned int)((unsigned long)current * (unsigned long)voltage / 1000L);
        processMinMax(current, &MinCurrent, &MaxCurrent);
        processMinMax(voltage, &MinVoltage, &MaxVoltage);
        processMinMax(power,   &MinPower,   &MaxPower);
        if (current < INT_MAX) { sumCurrent += current; cntCurrent += 1; }
        if (voltage < INT_MAX) { sumVoltage += voltage; cntVoltage += 1; }
        if (power   < INT_MAX) { sumPower   += power;   cntPower   += 1; }
    }
    processAvg(sumCurrent, cntCurrent, &AvgCurrent);
    processAvg(sumVoltage, cntVoltage, &AvgVoltage);
    processAvg(sumPower  , cntPower,   &AvgPower);
}

void resetMinMax() {
    MinCurrent = INT_MAX;
    MaxCurrent = INT_MAX;
    MinVoltage = INT_MAX;
    MaxVoltage = INT_MAX;
    MinPower   = INT_MAX;
    MaxPower   = INT_MAX;
}

void showMeasurement(unsigned char unitIndex, unsigned char typeIndex) {
    switch (unitIndex * 3 + typeIndex) {
        case 0: lcd_writeMilliValue(AvgCurrent); break;
        case 1: lcd_writeMilliValue(MaxCurrent); break;
        case 2: lcd_writeMilliValue(MinCurrent); break;
        case 3: lcd_writeValue(AvgVoltage); break;
        case 4: lcd_writeValue(MaxVoltage); break;
        case 5: lcd_writeValue(MinVoltage); break;
        case 6: lcd_writeValue(AvgPower); break;
        case 7: lcd_writeValue(MaxPower); break;
        case 8: lcd_writeValue(MinPower); break;
    }
}
