#include "IotaWatt.h"

/******************************************************************************************************** 
 *******************************************************************************************************/

uint32_t cycleSaverService(struct serviceBlock *_serviceBlock) {

  static uint32_t timeThen = millis();
  uint32_t timeNow = millis();
  static int nextChannelToSave = 0;
  enum states { setup,
                nextchannel,
                awaitsamples } static state = setup;

  //log("cycleSaverService: invoked");

  //Serial.printf("cycleSaverService: invoked. state=%d nextChannelToSave=%d lastChannel:%d\n",state, nextChannelToSave, lastChannel);
  
  for (int i = 0; i < maxInputs; i++) {
    if (inputChannel[i]->isActive()) {
      //Serial.printf("active channel:%d type:%d name:%s samples:%d \n", i, inputChannel[i]->_type, inputChannel[i]->_name, samples);
    }

    switch (state) {

    case setup:
      log("cycleSaverService: started.");
      state = nextchannel;
      return (uint32_t)UTCtime() + 1;

    case nextchannel:
        // find the next active channel to save
        //Serial.println("cycleSaverService: nextchannel.");
        for (int i = 0; i < maxInputs; i++) {
          nextChannelToSave = (++nextChannelToSave) % maxInputs;
          if (inputChannel[nextChannelToSave]->isActive())
        break;
        }
        state = awaitsamples;
        // drop through 

    case awaitsamples:      
      // exit and return on next loop if last sample is not the channel to save
      if (nextChannelToSave != sampledChannel) {
        //Serial.println("cycleSaverService: returning to await");
        return (uint32_t)UTCtime();
      }

    }
    if (nextChannelToSave!=sampledChannel) Serial.println("ERROR");

    // Save last set of samples - currently just print
    
    Serial.printf("Saving channel: %d Type: %d Name: %s\n V: %p I: %p",sampledChannel,inputChannel[sampledChannel]->_type, inputChannel[sampledChannel]->_name,Vsample,Isample);
    Serial.printf("s,V,I\n");
    for (int s=0;s<samples;s++) {
      Serial.printf("%d,%d,%d\n",s,Vsample[s],Isample[s]);
    }
    
    state = nextchannel;

    return UTCtime() + cycleSaverServiceInterval;
  }
}
