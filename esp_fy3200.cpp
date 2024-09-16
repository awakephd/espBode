#include "esp_config.h"
#include <string.h>

#if AWG == FY3200
#warning Compiling for FY3200

#include "esp_fy3200.h"

extern uint32_t HULP, AMP_HULP1, AMP_HULP2;

volatile SDeviceState gDeviceState;

bool fy3200_write(char* data, uint8_t len)
{
    uint32_t timeout = 0;
    Serial.write((uint8_t*)data, len);
    telnet.println("[");
    telnet.print(data);
    telnet.println("]");
    while(!Serial.available())
    {
        delay(1);
        if(timeout++>1000) return false;
    }
    bool ok = false;
    ok = (Serial.read() == 0x0a); // 0x0a == \n

    if(!ok){
      telnet.println("Invalid response for command");      
    }
    return ok;
}

void setCh1Wave(EWaveType wave)
{
    char command[] = "bw0\n";
    snprintf(command, 7, "bw%01u\n", wave);
    gDeviceState.ch1Wave = wave;
    fy3200_write(command, 4);
    setCh2Wave(wave);// also set CH2
}

void setCh2Wave(EWaveType wave)
{
    char command[] = "dw0\n";
    snprintf(command, 7, "dw%01u\n", wave);
    gDeviceState.ch1Wave = wave;
    fy3200_write(command, 4);
}

void setCh1Output(uint32_t output)
{
  uint32_t hulp =AMP_HULP1 / 10;
  if(output == 1)
  {
    char command[] = "ba00.00\n";
    snprintf(command, 9, "ba%02u.%02u\n", hulp/100, hulp%100);
    gDeviceState.ch1Ampl = hulp;
    fy3200_write(command, 8);
  }
  else
  {
    char command[] = "da00.00\n";
    gDeviceState.ch1Ampl = hulp;
    fy3200_write(command, 8);
  }
}

void setCh2Output(uint32_t output)
{
  uint32_t hulp =AMP_HULP2 / 10;
  if(output == 1)
  {
    char command[] = "da00.00\n";
    snprintf(command, 9, "da%02u.%02u\n", hulp/100, hulp%100);
    gDeviceState.ch2Ampl = hulp;
    fy3200_write(command, 8);
  }
  else
  {
    char command[] = "da00.00\n";
    gDeviceState.ch2Ampl = hulp;
    fy3200_write(command, 8);
  }
}

/* Set frequency in Hz */
void setCh1Freq(uint32_t frequency)
{
// SDS1104X-E sends freq. commands twice, following code prevents second time
    if(HULP != frequency)
    {
        HULP = frequency;

        setCh2Freq(frequency); // send CH2 first

        char command[] = "bf000000000\n";
        snprintf(command, 13, "bf%07lu\n", frequency * 100);
        gDeviceState.ch1Freq = frequency;
        fy3200_write(command, 12);
    }
}

/* Set frequency in Hz */
void setCh2Freq(uint32_t frequency)
{
        char command[] = "df000000000\n";
        snprintf(command, 13, "df%07lu\n", frequency * 100);
    gDeviceState.ch2Freq = frequency;
    fy3200_write(command, 12);
}

/* Ampl is in mV: 12.34V = 1234? */
void setCh1Ampl(uint32_t ampl)
{
    uint32_t hulp = ampl / 10;
    AMP_HULP1 = ampl;
    char command[] = "ba00.00\n";
    snprintf(command, 9, "ba%02u.%02u\n", hulp/100, hulp%100);
    gDeviceState.ch1Ampl = hulp;
    fy3200_write(command, 8);
    setCh2Ampl(ampl);// Also set amplitude CH2
}

void setCh2Ampl(uint32_t ampl)
{
    uint32_t hulp = ampl / 10;
    AMP_HULP2 = ampl;
    char command[] = "da00.00\n";
    snprintf(command, 9, "da%02u.%02u\n", hulp/100, hulp%100);
    gDeviceState.ch1Ampl = hulp;
    fy3200_write(command, 8);
}

/* Phase is in 0.1deg: 12.5deg = 125 FY6900*/
/* Phase is in 1deg: = phase / 10 FY3200*/
void setCh1Phase(uint32_t phase)
{
/*    char command[] = "dp000\n";
    snprintf(command, 7, "WFP%03u\n", phase/10);
    gDeviceState.ch1Phase = phase;
    fy3200_write(command, 6);
*/
}

/* Phase is in 0.1deg: 12.5deg = 125 FY6900*/
/* Phase is in 1deg: = phase / 10 FY3200*/
void setCh2Phase(uint32_t phase)
{
    char command[] = "dp000\n";
    snprintf(command, 7, "WFP%03u\n", phase/10);
    gDeviceState.ch2Phase = phase;
    fy3200_write(command, 6);
}

void setCh1Offset(int32_t offset)
{
    int32_t hulp = offset / 10;
    char command[] = "bo00.0\n";
    gDeviceState.ch1Offset = offset;
    if(offset>=0)
    {
        snprintf(command, 8, "b0%02u.%01u\n", offset/10, offset%10);
        fy3200_write(command, 7);
    }
    else
    {
        snprintf(command, 8, "b0%02u.%01u\n", offset/10, offset%10);
        fy3200_write(command, 7);
    }
//    setCh2Offset(offset);
}

void setCh2Offset(int32_t offset)
{
    int32_t hulp = offset / 10;
    char command[] = "do00.0\n";
    gDeviceState.ch1Offset = offset;
    if(offset>=0)
    {
        snprintf(command, 8, "d0%02u.%01u\n", offset/10, offset%10);
        fy3200_write(command, 7);
    }
    else
    {
        snprintf(command, 8, "d0%02u.%01u\n", offset/10, offset%10);
        fy3200_write(command, 7);
    }
}

void initDevice(void)
{
    Serial.write((uint8_t*)"\n", 1);

    setCh1Output(0);
    setCh1Wave(EWaveType_Sine);
    setCh1Freq(1000);
    setCh1Ampl(1000);
    setCh1Offset(0);

    setCh2Output(0);
    setCh2Wave(EWaveType_Sine);
    setCh2Freq(1000);
    setCh2Ampl(1000);
    setCh2Offset(0);
}

#endif
