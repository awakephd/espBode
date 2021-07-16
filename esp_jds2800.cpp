#include "esp_config.h"
#include <string.h>

#if AWG == JDS2800
#warning Compiling for JDS2800

#include "esp_jds2800.h"

volatile SDeviceState gDeviceState;

bool jds2800_write(char* data, uint8_t len)
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
    ok = (Serial.read() == 0x0d,0x0a); // 0x0a == \n 0x0d == \r

    if(!ok){
      telnet.println("Invalid response for command");      
    }
    delay(10);
    return ok;
}

void setCh1Wave(EWaveType wave)
{
    char command[] = ":w21=00.\r\n";
    snprintf(command, 11, ":w21=%02u.\r\n", wave);
    gDeviceState.ch1Wave = wave;
    jds2800_write(command, 10);
    delay(10);
}

void setCh2Wave(EWaveType wave)
{
    char command[] = ":w22=00.\r\n";
    snprintf(command, 11, ":w22=%02u.\r\n", wave);
    gDeviceState.ch1Wave = wave;
    jds2800_write(command, 10);
    delay(10);
}

void setCh1Output(uint32_t output)
{
  gDeviceState.ch1Output = output;
  jds2800_write((char*)(output ? ":w20=1,0.\r\n" : "w20=0,0.\r\n"), 11);
}

void setCh2Output(uint32_t output)
{
  gDeviceState.ch2Output = output;
  jds2800_write((char*)(output ? ":w20=1,1.\r\n" : ":w20=1,0.\r\n"), 11);
}

/* Set frequency in Hz */
void setCh1Freq(uint32_t frequency)
{
    if(gDeviceState.ch1Freq != frequency)
    {
      char command[] = ":w23=0000000000,0.\r\n";
      snprintf(command, 21, ":w23=%10u,0.\r\n", frequency*100);
      gDeviceState.ch1Freq = frequency;
      jds2800_write(command, 20);
    }
    }

/* Set frequency in Hz */
void setCh2Freq(uint32_t frequency)
{
        char command[] = ":w24=0000000000,0.\r\n";
    snprintf(command, 21, ":w24=%10u,0.\r\n", frequency*100);
    gDeviceState.ch2Freq = frequency;
    jds2800_write(command, 20);
}

/* Ampl is in mV: 12.345V = 12345 */
void setCh1Ampl(uint32_t ampl)
{
    char command[] = ":w25=00000.\r\n";
    snprintf(command, 14, ":w25=%05u.\r\n", ampl);
    gDeviceState.ch1Ampl = ampl;
    jds2800_write(command, 13);
}

void setCh2Ampl(uint32_t ampl)
{
    char command[] = ":w26=00000.\r\n";
    snprintf(command, 14, ":w26=%05u.\r\n", ampl);
    gDeviceState.ch2Ampl = ampl;
    jds2800_write(command, 13);
}

/* Phase is in 0.1deg: 12.5deg = 125 */
void setCh1Phase(uint32_t phase)
{
    char command[] = "w:31=0000.\r\n";
    snprintf(command, 13, ":w31=%04u.\r\n", phase);
    gDeviceState.ch1Phase = phase;
    jds2800_write(command, 12);
    delay(10);
}

void setCh2Phase(uint32_t phase)
{
    char command[] = ":w31=0000.\r\n";
    snprintf(command, 13, ":w31=%04u.\r\n", phase);
    gDeviceState.ch2Phase = phase;
    jds2800_write(command, 12);
    delay(10);
}

void setCh1Offset(int32_t offset)
{
    char command[] = ":w27=0000.\r\n";
    gDeviceState.ch1Offset = offset;
    snprintf(command, 13, ":w27=%04u.\r\n", offset/10+1000);
    jds2800_write(command, 12);
 
}

void setCh2Offset(int32_t offset)
{
    char command[] = ":w28=0000.\r\n";
    gDeviceState.ch2Offset = offset;
    snprintf(command, 13, ":w28=%04u.\r\n", offset/10+1000);
    jds2800_write(command, 12);
}

void initDevice(void)
{
    Serial.write((uint8_t*)"\r\n", 3);

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
