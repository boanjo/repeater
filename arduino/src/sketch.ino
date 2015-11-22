
#include <NewRemoteReceiver.h> 
#include <NewRemoteTransmitter.h> 

struct NewCode {
  unsigned long millis;
  unsigned long address;
  byte unit;
  boolean groupBit;
  boolean on;
};

#define MAX_REC_CODES       16

NewCode recCodeSwitch[MAX_REC_CODES];
byte recCodeSwitchCnt = 0;
byte recCodeSwitchIndex = 0;
byte recCodeSwitchSendIndex = 0;
byte recCnt = 0;
byte sendCnt = 0;


#define TX_PIN             11
#define LED_DEVICE_SEND     7
#define LED_DEVICE_RECEIVE  6
#define LED_ERROR           5

void setup()
{

  pinMode(LED_DEVICE_SEND, OUTPUT);
  pinMode(LED_DEVICE_RECEIVE, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);

  NewRemoteReceiver::init(0, 1, recCodeCallback);
}

// Callback function is called only when a valid code is received.
void recCodeCallback(NewRemoteCode receivedCode) {
// Note: interrupts are disabled. You can re-enable them if needed.

  if(recCodeSwitchCnt >= MAX_REC_CODES)
  {

    digitalWrite(LED_ERROR, HIGH);
  }
  else
  {
    digitalWrite(LED_DEVICE_RECEIVE, HIGH);
      
    recCodeSwitch[recCodeSwitchIndex].millis = millis();
    recCodeSwitch[recCodeSwitchIndex].address = receivedCode.address;
    recCodeSwitch[recCodeSwitchIndex].unit = receivedCode.unit;
    recCodeSwitch[recCodeSwitchIndex].groupBit = receivedCode.groupBit;
    if(receivedCode.switchType == NewRemoteCode::off)
      recCodeSwitch[recCodeSwitchIndex].on = false;
    else
      recCodeSwitch[recCodeSwitchIndex].on = true;
    recCodeSwitchCnt++;
    recCnt++;
    recCodeSwitchIndex = (recCodeSwitchIndex + 1) % MAX_REC_CODES;
  }
}
void loop() 
{
  if(sendCnt != recCnt)
  {
    unsigned long now = millis();
    unsigned long diff = 0;
    if(now < recCodeSwitch[recCodeSwitchSendIndex].millis)
    {
      diff = recCodeSwitch[recCodeSwitchSendIndex].millis - now;
    }
    else 
    {
      diff = now - recCodeSwitch[recCodeSwitchSendIndex].millis;
    }
    
    if(diff > 100)
    {
      digitalWrite(LED_DEVICE_SEND, HIGH);
      NewRemoteReceiver::disable();    
      NewRemoteTransmitter 
	transmitter(recCodeSwitch[recCodeSwitchSendIndex].address, 
		    TX_PIN, 260, 1);
      transmitter.sendUnit(recCodeSwitch[recCodeSwitchSendIndex].unit,
			   recCodeSwitch[recCodeSwitchSendIndex].groupBit);
      digitalWrite(LED_DEVICE_SEND, LOW);
      
      
      recCodeSwitchSendIndex = (recCodeSwitchSendIndex + 1) % MAX_REC_CODES;
      recCodeSwitchCnt--;
      sendCnt++;
      NewRemoteReceiver::enable();    
      digitalWrite(LED_DEVICE_RECEIVE, LOW);
    }
  }
} 

