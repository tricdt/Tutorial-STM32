#ifndef __RCSwitch_H
#define __RCSwitch_H 			   
#include "sys.h"  
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////////	 
#define RCSWITCH_MAX_CHANGES 67
		
void RCSwitch(void);
    
    void switchOn4(int nGroupNumber, int nSwitchNumber);
    void switchOff4(int nGroupNumber, int nSwitchNumber);
    void switchOn2(const char* sGroup, int nSwitchNumber);
    void switchOff2(const char* sGroup, int nSwitchNumber);
    void switchOn1(char sFamily, int nGroup, int nDevice);
    void switchOff1(char sFamily, int nGroup, int nDevice);
    void switchOn3(const char* sGroup, const char* sDevice);
    void switchOff3(const char* sGroup, const char* sDevice);
    void switchOn(char sGroup, int nDevice);
    void switchOff(char sGroup, int nDevice);

    void sendTriState(const char* sCodeWord);
    void send(unsigned long code, unsigned int length);
    void sendx(const char* sCodeWord);
    
    //#if not defined( RCSwitchDisableReceiving )
    void RCSwitch_enableReceive(int interrupt);
    void enableReceive(void);
    void disableReceive(void);
    boolean RCSwitch_available(void);
    void RCSwitch_resetAvailable(void);

    unsigned long RCSwitch_getReceivedValue(void);
    unsigned int RCSwitch_getReceivedBitlength(void);
    unsigned int RCSwitch_getReceivedDelay(void);
    unsigned int RCSwitch_getReceivedProtocol(void);
    unsigned int* RCSwitch_getReceivedRawdata(void);
    //#endif
  
    void enableTransmit(int nTransmitterPin);
    void disableTransmit(void);
    void setPulseLength(int nPulseLength);
    void setRepeatTransmit(int nRepeatTransmit);
    //#if not defined( RCSwitchDisableReceiving )
    void setReceiveTolerance(int nPercent);
    //#endif

    /**
     * Description of a single pule, which consists of a high signal
     * whose duration is "high" times the base pulse length, followed
     * by a low signal lasting "low" times the base pulse length.
     * Thus, the pulse overall lasts (high+low)*pulseLength
     */
    struct HighLow {
        uint8_t high;
        uint8_t low;
    };

    /**
     * A "protocol" describes how zero and one bits are encoded into high/low
     * pulses.
     */
    struct Protocol {
        /** base pulse length in microseconds, e.g. 350 */
        uint16_t pulseLength;

        struct HighLow syncFactor;
        struct HighLow zero;
        struct HighLow one;

        /**
         * If true, interchange high and low logic levels in all transmissions.
         *
         * By default, RCSwitch assumes that any signals it sends or receives
         * can be broken down into pulses which start with a high signal level,
         * followed by a a low signal level. This is e.g. the case for the
         * popular PT 2260 encoder chip, and thus many switches out there.
         *
         * But some devices do it the other way around, and start with a low
         * signal level, followed by a high signal level, e.g. the HT6P20B. To
         * accommodate this, one can set invertedSignal to true, which causes
         * RCSwitch to change how it interprets any HighLow struct FOO: It will
         * then assume transmissions start with a low signal lasting
         * FOO.high*pulseLength microseconds, followed by a high signal lasting
         * FOO.low*pulseLength microseconds.
         */
        boolean invertedSignal;
    };

    void setProtocol1(struct Protocol protocol);
    void setProtocol(int nProtocol);
    void setProtocol2(int nProtocol, int nPulseLength);

////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////  

		
////////////////////////////////////////////////////////////////////////////////// 	 


#endif


