#if AWG == FY3200

#ifndef _ESP_FY3200_H_
#define _ESP_FY3200_H_

#include <ESP8266WiFi.h>

typedef enum
{
    EWaveType_Sine          = 0,
    EWaveType_Square        = 1,
    EWaveType_Pulse         = 2,
    EWaveType_TriangleWave  = 3,
    EWaveType_SawtoothR     = 4,
    EWaveType_SawtoothL     = 5,
    EWaveType_DC            = 6,
    EWaveType_LorentzPulses = 7,
    EWaveType_MultiTone     = 8,
    EWaveType_RandomNoise   = 9,
    EWaveType_HeartBeat     = 10,
    EWaveType_Trapezoid     = 11,
    EWaveType_SyncPulse     = 12,
    EWaveType_VariPulses    = 13,
    EWaveType_WhiteNoise    = 14,
    EWaveType_AmplitudeMod  = 15,
    EWaveType_FrequencyMod  = 16,
    EWaveType_Sine2         = 17,
    EWaveType_TriangleWave2 = 18,
    EWaveType_SawtoothR2    = 19,
    EWaveType_SawtoothL2    = 20,
    EWaveType_Last
}EWaveType;

typedef struct
{
    uint8_t     ch1Output;
    uint8_t     ch2Output;
    EWaveType   ch1Wave;
    EWaveType   ch2Wave;
    uint32_t    ch1Freq;
    uint32_t    ch2Freq;
    uint32_t    ch1Ampl;
    uint32_t    ch2Ampl;
    uint32_t    ch1Phase;
    uint32_t    ch2Phase;
    uint32_t    ch1Offset;
    uint32_t    ch2Offset;

}SDeviceState;

extern volatile SDeviceState gDeviceState;

void setCh1Wave(EWaveType wave);
void setCh2Wave(EWaveType wave);

/* 0 - off; 1 - on */
void setCh1Output(uint32_t output);
void setCh2Output(uint32_t output);

/* Set frequency in Hz */
void setCh1Freq(uint32_t frequency);
void setCh2Freq(uint32_t frequency);

/* Ampl is in mV: 12.34V = 1234? */
void setCh1Ampl(uint32_t ampl);
void setCh2Ampl(uint32_t ampl);

/* Phase is in 1deg: = phase / 10 */
void setCh1Phase(uint32_t phase);
void setCh2Phase(uint32_t phase);

/* Offset is in mV: 12.34V = 1234? */
void setCh1Offset(int32_t offset);
void setCh2Offset(int32_t offset);

/* Can be used to set some default parameters */
void initDevice(void);

#endif //_ESP_FY3200_H_

#endif
