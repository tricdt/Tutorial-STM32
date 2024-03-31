#include "RCSwitch.h"
#include "exti.h"

#define RECEIVE_ATTR

#define PROGMEM
#define PGM_P  const char *
#define PSTR(str) (str)

#define memcpy_P(dest, src, num) memcpy((dest), (src), (num))
 
/* Format for protocol definitions:
 * {pulselength, Sync bit, "0" bit, "1" bit}
 * 
 * pulselength: pulse length in microseconds, e.g. 350
 * Sync bit: {1, 31} means 1 high pulse and 31 low pulses
 *     (perceived as a 31*pulselength long pulse, total length of sync bit is
 *     32*pulselength microseconds), i.e:
 *      _
 *     | |_______________________________ (don't count the vertical bars)
 * "0" bit: waveform for a data bit of value "0", {1, 3} means 1 high pulse
 *     and 3 low pulses, total length (1+3)*pulselength, i.e:
 *      _
 *     | |___
 * "1" bit: waveform for a data bit of value "1", e.g. {3,1}:
 *      ___
 *     |   |_
 *
 * These are combined to form Tri-State bits when sending or receiving codes.
 */

static const struct Protocol PROGMEM proto[] = {

  { 350, {  1, 31 }, {  1,  3 }, {  3,  1 }, false },    // protocol 1
  { 650, {  1, 10 }, {  1,  2 }, {  2,  1 }, false },    // protocol 2
  { 100, { 30, 71 }, {  4, 11 }, {  9,  6 }, false },    // protocol 3
  { 380, {  1,  6 }, {  1,  3 }, {  3,  1 }, false },    // protocol 4
  { 500, {  6, 14 }, {  1,  2 }, {  2,  1 }, false },    // protocol 5
  { 450, { 23,  1 }, {  1,  2 }, {  2,  1 }, true },      // protocol 6 (HT6P20B)
  { 150, {  2, 62 }, {  1,  6 }, {  6,  1 }, false }     // protocol 7 (HS2303-PT, i. e. used in AUKEY Remote)
};

enum {
   numProto = sizeof(proto) / sizeof(proto[0])
};


		char* getCodeWordA(const char* sGroup, const char* sDevice, boolean bStatus);
    char* getCodeWordB(int nGroupNumber, int nSwitchNumber, boolean bStatus);
    char* getCodeWordC(char sFamily, int nGroup, int nDevice, boolean bStatus);
    char* getCodeWordD(char group, int nDevice, boolean bStatus);
    void transmit(struct HighLow pulses);

    //#if not defined( RCSwitchDisableReceiving )
    static void handleInterrupt(void);
    static boolean receiveProtocol(const int p, unsigned int changeCount);
    int nReceiverInterrupt;
    //#endif
    int nTransmitterPin;
    int nRepeatTransmit;
    
    struct Protocol protocol;

		
//#if not defined( RCSwitchDisableReceiving )
volatile unsigned long nReceivedValue = 0;
volatile unsigned int nReceivedBitlength = 0;
volatile unsigned int nReceivedDelay = 0;
volatile unsigned int nReceivedProtocol = 0;
int nReceiveTolerance = 60;
const unsigned int nSeparationLimit = 4300;
// separationLimit: minimum microseconds between received codes, closer codes are ignored.
// according to discussion on issue #14 it might be more suitable to set the separation
// limit to the same time as the 'low' part of the sync signal for the current protocol.
unsigned int timings[RCSWITCH_MAX_CHANGES];
//#endif

void RCSwitch(void) {
  nTransmitterPin = -1;
  setRepeatTransmit(10);
  setProtocol(1);
  //#if not defined( RCSwitchDisableReceiving )
  nReceiverInterrupt = -1;
  setReceiveTolerance(60);
  nReceivedValue = 0;
  //#endif
}

/**
  * Sets the protocol to send.
  */
void setProtocol1(struct Protocol protocol) {
  protocol = protocol;
}

/**
  * Sets the protocol to send, from a list of predefined protocols
  */
void setProtocol(int nProtocol) {
  if (nProtocol < 1 || nProtocol > numProto) {
    nProtocol = 1;  // TODO: trigger an error, e.g. "bad protocol" ???
  }

  memcpy_P(&protocol, &proto[nProtocol-1], sizeof(struct Protocol));

}

/**
  * Sets the protocol to send with pulse length in microseconds.
  */
void setProtocol2(int nProtocol, int nPulseLength) {
  setProtocol(nProtocol);
  setPulseLength(nPulseLength);
}


/**
  * Sets pulse length in microseconds
  */
void setPulseLength(int nPulseLength) {
  protocol.pulseLength = nPulseLength;
}

/**
 * Sets Repeat Transmits
 */
void setRepeatTransmit(int nRepeatTransmit) {
  nRepeatTransmit = nRepeatTransmit;
}

/**
 * Set Receiving Tolerance
 */
//#if not defined( RCSwitchDisableReceiving )
void setReceiveTolerance(int nPercent) {
  nReceiveTolerance = nPercent;
}
//#endif
  

/**
 * Enable transmissions
 *
 * @param nTransmitterPin    Arduino Pin to which the sender is connected to
 */
void enableTransmit(int nTransmitterPin) {
  nTransmitterPin = nTransmitterPin;
  pinMode(nTransmitterPin, OUTPUT);
}

/**
  * Disable transmissions
  */
void disableTransmit() {
  nTransmitterPin = -1;
}

/**
 * Switch a remote switch on (Type D REV)
 *
 * @param sGroup        Code of the switch group (A,B,C,D)
 * @param nDevice       Number of the switch itself (1..3)
 */
void switchOn(char sGroup, int nDevice) {
  sendTriState( getCodeWordD(sGroup, nDevice, true) );
}

/**
 * Switch a remote switch off (Type D REV)
 *
 * @param sGroup        Code of the switch group (A,B,C,D)
 * @param nDevice       Number of the switch itself (1..3)
 */
void switchOff(char sGroup, int nDevice) {
  sendTriState( getCodeWordD(sGroup, nDevice, false) );
}

/**
 * Switch a remote switch on (Type C Intertechno)
 *
 * @param sFamily  Familycode (a..f)
 * @param nGroup   Number of group (1..4)
 * @param nDevice  Number of device (1..4)
  */
void switchOn1(char sFamily, int nGroup, int nDevice) {
  sendTriState( getCodeWordC(sFamily, nGroup, nDevice, true) );
}

/**
 * Switch a remote switch off (Type C Intertechno)
 *
 * @param sFamily  Familycode (a..f)
 * @param nGroup   Number of group (1..4)
 * @param nDevice  Number of device (1..4)
 */
void switchOff1(char sFamily, int nGroup, int nDevice) {
  sendTriState( getCodeWordC(sFamily, nGroup, nDevice, false) );
}

/**
 * Switch a remote switch on (Type B with two rotary/sliding switches)
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 */
void switchOn4(int nAddressCode, int nChannelCode) {
  sendTriState( getCodeWordB(nAddressCode, nChannelCode, true) );
}

/**
 * Switch a remote switch off (Type B with two rotary/sliding switches)
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 */
void switchOff4(int nAddressCode, int nChannelCode) {
  sendTriState( getCodeWordB(nAddressCode, nChannelCode, false) );
}

/**
 * Deprecated, use switchOn(const char* sGroup, const char* sDevice) instead!
 * Switch a remote switch on (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param nChannelCode  Number of the switch itself (1..5)
 */
void switchOn2(const char* sGroup, int nChannel) {
  const char* code[6] = { "00000", "10000", "01000", "00100", "00010", "00001" };
  switchOn3(sGroup, code[nChannel]);
}

/**
 * Deprecated, use switchOff(const char* sGroup, const char* sDevice) instead!
 * Switch a remote switch off (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param nChannelCode  Number of the switch itself (1..5)
 */
void switchOff2(const char* sGroup, int nChannel) {
  const char* code[6] = { "00000", "10000", "01000", "00100", "00010", "00001" };
  switchOff3(sGroup, code[nChannel]);
}

/**
 * Switch a remote switch on (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param sDevice       Code of the switch device (refers to DIP switches 6..10 (A..E) where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 */
void switchOn3(const char* sGroup, const char* sDevice) {
  sendTriState( getCodeWordA(sGroup, sDevice, true) );
}

/**
 * Switch a remote switch off (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param sDevice       Code of the switch device (refers to DIP switches 6..10 (A..E) where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 */
void switchOff3(const char* sGroup, const char* sDevice) {
  sendTriState( getCodeWordA(sGroup, sDevice, false) );
}


/**
 * Returns a char[13], representing the code word to be send.
 *
 */
char* getCodeWordA(const char* sGroup, const char* sDevice, boolean bStatus) {
  static char sReturn[13];
  int nReturnPos = 0;
	int i = 0;

  for (i = 0; i < 5; i++) {
    sReturn[nReturnPos++] = (sGroup[i] == '0') ? 'F' : '0';
  }

  for ( i = 0; i < 5; i++) {
    sReturn[nReturnPos++] = (sDevice[i] == '0') ? 'F' : '0';
  }

  sReturn[nReturnPos++] = bStatus ? '0' : 'F';
  sReturn[nReturnPos++] = bStatus ? 'F' : '0';

  sReturn[nReturnPos] = '\0';
  return sReturn;
}

/**
 * Encoding for type B switches with two rotary/sliding switches.
 *
 * The code word is a tristate word and with following bit pattern:
 *
 * +-----------------------------+-----------------------------+----------+------------+
 * | 4 bits address              | 4 bits address              | 3 bits   | 1 bit      |
 * | switch group                | switch number               | not used | on / off   |
 * | 1=0FFF 2=F0FF 3=FF0F 4=FFF0 | 1=0FFF 2=F0FF 3=FF0F 4=FFF0 | FFF      | on=F off=0 |
 * +-----------------------------+-----------------------------+----------+------------+
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 * @param bStatus       Whether to switch on (true) or off (false)
 *
 * @return char[13], representing a tristate code word of length 12
 */
char* getCodeWordB(int nAddressCode, int nChannelCode, boolean bStatus) {
  static char sReturn[13];
  int nReturnPos = 0;
	int i = 0;

  if (nAddressCode < 1 || nAddressCode > 4 || nChannelCode < 1 || nChannelCode > 4) {
    return 0;
  }

  for ( i = 1; i <= 4; i++) {
    sReturn[nReturnPos++] = (nAddressCode == i) ? '0' : 'F';
  }

  for ( i = 1; i <= 4; i++) {
    sReturn[nReturnPos++] = (nChannelCode == i) ? '0' : 'F';
  }

  sReturn[nReturnPos++] = 'F';
  sReturn[nReturnPos++] = 'F';
  sReturn[nReturnPos++] = 'F';

  sReturn[nReturnPos++] = bStatus ? 'F' : '0';

  sReturn[nReturnPos] = '\0';
  return sReturn;
}

/**
 * Like getCodeWord (Type C = Intertechno)
 */
char* getCodeWordC(char sFamily, int nGroup, int nDevice, boolean bStatus) {
  static char sReturn[13];
  int nReturnPos = 0;

  int nFamily = (int)sFamily - 'a';
  if ( nFamily < 0 || nFamily > 15 || nGroup < 1 || nGroup > 4 || nDevice < 1 || nDevice > 4) {
    return 0;
  }
  
  // encode the family into four bits
  sReturn[nReturnPos++] = (nFamily & 1) ? 'F' : '0';
  sReturn[nReturnPos++] = (nFamily & 2) ? 'F' : '0';
  sReturn[nReturnPos++] = (nFamily & 4) ? 'F' : '0';
  sReturn[nReturnPos++] = (nFamily & 8) ? 'F' : '0';

  // encode the device and group
  sReturn[nReturnPos++] = ((nDevice-1) & 1) ? 'F' : '0';
  sReturn[nReturnPos++] = ((nDevice-1) & 2) ? 'F' : '0';
  sReturn[nReturnPos++] = ((nGroup-1) & 1) ? 'F' : '0';
  sReturn[nReturnPos++] = ((nGroup-1) & 2) ? 'F' : '0';

  // encode the status code
  sReturn[nReturnPos++] = '0';
  sReturn[nReturnPos++] = 'F';
  sReturn[nReturnPos++] = 'F';
  sReturn[nReturnPos++] = bStatus ? 'F' : '0';

  sReturn[nReturnPos] = '\0';
  return sReturn;
}

/**
 * Encoding for the REV Switch Type
 *
 * The code word is a tristate word and with following bit pattern:
 *
 * +-----------------------------+-------------------+----------+--------------+
 * | 4 bits address              | 3 bits address    | 3 bits   | 2 bits       |
 * | switch group                | device number     | not used | on / off     |
 * | A=1FFF B=F1FF C=FF1F D=FFF1 | 1=0FF 2=F0F 3=FF0 | 000      | on=10 off=01 |
 * +-----------------------------+-------------------+----------+--------------+
 *
 * Source: http://www.the-intruder.net/funksteckdosen-von-rev-uber-arduino-ansteuern/
 *
 * @param sGroup        Name of the switch group (A..D, resp. a..d) 
 * @param nDevice       Number of the switch itself (1..3)
 * @param bStatus       Whether to switch on (true) or off (false)
 *
 * @return char[13], representing a tristate code word of length 12
 */
char* getCodeWordD(char sGroup, int nDevice, boolean bStatus) {
  static char sReturn[13];
  int nReturnPos = 0;
	int i = 0;
  // sGroup must be one of the letters in "abcdABCD"
  int nGroup = (sGroup >= 'a') ? (int)sGroup - 'a' : (int)sGroup - 'A';
  if ( nGroup < 0 || nGroup > 3 || nDevice < 1 || nDevice > 3) {
    return 0;
  }

  for (i = 0; i < 4; i++) {
    sReturn[nReturnPos++] = (nGroup == i) ? '1' : 'F';
  }

  for ( i = 1; i <= 3; i++) {
    sReturn[nReturnPos++] = (nDevice == i) ? '1' : 'F';
  }

  sReturn[nReturnPos++] = '0';
  sReturn[nReturnPos++] = '0';
  sReturn[nReturnPos++] = '0';

  sReturn[nReturnPos++] = bStatus ? '1' : '0';
  sReturn[nReturnPos++] = bStatus ? '0' : '1';

  sReturn[nReturnPos] = '\0';
  return sReturn;
}

/**
 * @param sCodeWord   a tristate code word consisting of the letter 0, 1, F
 */
void sendTriState(const char* sCodeWord) {
  // turn the tristate code word into the corresponding bit pattern, then send it
  unsigned long code = 0;
  unsigned int length = 0;
	const char* p ;
  for ( p = sCodeWord; *p; p++) {
    code <<= 2L;
    switch (*p) {
      case '0':
        // bit pattern 00
        break;
      case 'F':
        // bit pattern 01
        code |= 1L;
        break;
      case '1':
        // bit pattern 11
        code |= 3L;
        break;
    }
    length += 2;
  }
  send(code, length);
}

/**
 * @param sCodeWord   a binary code word consisting of the letter 0, 1
 */
void sendx(const char* sCodeWord) {
  // turn the tristate code word into the corresponding bit pattern, then send it
  unsigned long code = 0;
  unsigned int length = 0;
	const char* p ;
  for ( p = sCodeWord; *p; p++) {
    code <<= 1L;
    if (*p != '0')
      code |= 1L;
    length++;
  }
  send(code, length);
}

/**
 * Transmit the first 'length' bits of the integer 'code'. The
 * bits are sent from MSB to LSB, i.e., first the bit at position length-1,
 * then the bit at position length-2, and so on, till finally the bit at position 0.
 */
void send(unsigned long code, unsigned int length) {
	int i=0;
	int nRepeat = 0;
	
	//#if not defined( RCSwitchDisableReceiving )
  // make sure the receiver is disabled while we transmit
  int nReceiverInterrupt_backup = nReceiverInterrupt;
  if (nReceiverInterrupt_backup != -1) {
    disableReceive();
  }
//#endif
	
  if (nTransmitterPin == -1)
    return;



  for ( nRepeat = 0; nRepeat < nRepeatTransmit; nRepeat++) {
    for (i = length-1; i >= 0; i--) {
      if (code & (1L << i))
        transmit(protocol.one);
      else
        transmit(protocol.zero);
    }
    transmit(protocol.syncFactor);
  }

  // Disable transmit after sending (i.e., for inverted protocols)
  digitalWrite(nTransmitterPin, LOW);

//#if not defined( RCSwitchDisableReceiving )
  // enable receiver again if we just disabled it
  if (nReceiverInterrupt_backup != -1) {
    RCSwitch_enableReceive(nReceiverInterrupt_backup);
  }
//#endif
}

/**
 * Transmit a single high-low pulse.
 */
void transmit(struct HighLow pulses) {
  uint8_t firstLogicLevel = (protocol.invertedSignal) ? LOW : HIGH;
  uint8_t secondLogicLevel = (protocol.invertedSignal) ? HIGH : LOW;
  
  digitalWrite(nTransmitterPin, firstLogicLevel);
  delay_us( protocol.pulseLength * pulses.high);
  digitalWrite(nTransmitterPin, secondLogicLevel);
  delay_us( protocol.pulseLength * pulses.low);
}


//#if not defined( RCSwitchDisableReceiving )
/**
 * Enable receiving data
 */
void RCSwitch_enableReceive(int interrupt) {
  nReceiverInterrupt = interrupt;
  enableReceive();
}

void enableReceive() {
  if (nReceiverInterrupt != -1) {
    nReceivedValue = 0;
    nReceivedBitlength = 0;
    //attachInterrupt(nReceiverInterrupt, handleInterrupt, CHANGE);
		EXTIx_Init(GPIOB, GPIO_Pin_14, Trigger_Rising_Falling);
  }
}


void EXTI15_10_IRQHandler(void)
{
	 
	if(EXTI_GetITStatus(EXTI_Line14) != RESET) {
	 EXTI_ClearITPendingBit(EXTI_Line14);  
		handleInterrupt();
	}
}

void noInterrupts(void){
	NVIC_DisableIRQ(EXTI0_IRQn);
	NVIC_DisableIRQ(EXTI1_IRQn);
	NVIC_DisableIRQ(EXTI2_IRQn);
	NVIC_DisableIRQ(EXTI3_IRQn);
	NVIC_DisableIRQ(EXTI4_IRQn);
	NVIC_DisableIRQ(EXTI9_5_IRQn);
	NVIC_DisableIRQ(EXTI15_10_IRQn);
}
/**
 * Disable receiving data
 */
void disableReceive() {

  noInterrupts();//detachInterrupt(nReceiverInterrupt);
  nReceiverInterrupt = -1;
}

boolean RCSwitch_available() {
  return nReceivedValue != 0;
}

void RCSwitch_resetAvailable() {
  nReceivedValue = 0;
}

unsigned long RCSwitch_getReceivedValue() {
  return nReceivedValue;
}

unsigned int RCSwitch_getReceivedBitlength() {
  return nReceivedBitlength;
}

unsigned int RCSwitch_getReceivedDelay() {
  return nReceivedDelay;
}

unsigned int RCSwitch_getReceivedProtocol() {
  return nReceivedProtocol;
}

unsigned int* RCSwitch_getReceivedRawdata() {
  return timings;
}

/* helper function for the receiveProtocol method */
static unsigned int diff(int A, int B) {
  return abs(A - B);
}

/**
 *
 */
boolean RECEIVE_ATTR receiveProtocol(const int p, unsigned int changeCount) {

     struct Protocol pro=protocol;
	
		unsigned long code = 0;
    //Assuming the longer pulse length is the pulse captured in timings[0]
    const unsigned int syncLengthInPulses =  ((pro.syncFactor.low) > (pro.syncFactor.high)) ? (pro.syncFactor.low) : (pro.syncFactor.high);
    const unsigned int delay = timings[0] / syncLengthInPulses;
    const unsigned int delayTolerance = delay * nReceiveTolerance / 100;
    
    /* For protocols that start low, the sync period looks like
     *               _________
     * _____________|         |XXXXXXXXXXXX|
     *
     * |--1st dur--|-2nd dur-|-Start data-|
     *
     * The 3rd saved duration starts the data.
     *
     * For protocols that start high, the sync period looks like
     *
     *  ______________
     * |              |____________|XXXXXXXXXXXXX|
     *
     * |-filtered out-|--1st dur--|--Start data--|
     *
     * The 2nd saved duration starts the data
     */
    const unsigned int firstDataTiming = (pro.invertedSignal) ? (2) : (1);
		
		unsigned int i =0;
		//printf("%d----%d\n",delay,syncLengthInPulses);
    memcpy_P(&pro, &proto[p-1], sizeof(protocol));//struct Protocol
    

    for (i = firstDataTiming; i < changeCount - 1; i += 2) {
        code <<= 1;
        if (diff(timings[i], delay * pro.zero.high) < delayTolerance &&
            diff(timings[i + 1], delay * pro.zero.low) < delayTolerance) {
            // zero
        } else if (diff(timings[i], delay * pro.one.high) < delayTolerance &&
                   diff(timings[i + 1], delay * pro.one.low) < delayTolerance) {
            // one
            code |= 1;
        } else {
            // Failed
            return false;
        }
    }

    if (changeCount > 7) {    // ignore very short transmissions: no device sends them, so this must be noise
        nReceivedValue = code;
        nReceivedBitlength = (changeCount - 1) / 2;
        nReceivedDelay = delay;
        nReceivedProtocol = p;
        return true;
    }

    return false;
}

void RECEIVE_ATTR handleInterrupt() {

  static unsigned int changeCount = 0;
  static unsigned long lastTime = 0;
  static unsigned int repeatCount = 0;
	unsigned int i=0;
  const long time = micros();
  const unsigned int duration = time - lastTime;
		//printf("a\n");
  if (duration > nSeparationLimit) {
    // A long stretch without signal level change occurred. This could
    // be the gap between two transmission.
    if (diff(duration, timings[0]) < 200) {
      // This long signal is close in length to the long signal which
      // started the previously recorded timings; this suggests that
      // it may indeed by a a gap between two transmissions (we assume
      // here that a sender will send the signal multiple times,
      // with roughly the same gap between them).
      repeatCount++;
      if (repeatCount == 2) {
        for( i = 1; i <= numProto; i++) {
					//printf("%d----%d\n",i,changeCount);//1--50
          if (receiveProtocol(i, changeCount)) {
            // receive succeeded for protocol i						
            break;
          }
        }
        repeatCount = 0;
      }
    }
    changeCount = 0;
  }
 
  // detect overflow
  if (changeCount >= RCSWITCH_MAX_CHANGES) {
    changeCount = 0;
    repeatCount = 0;
  }

  timings[changeCount++] = duration;
  lastTime = time;  
}
//#endif


