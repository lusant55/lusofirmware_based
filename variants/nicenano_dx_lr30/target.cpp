#include "target.h"

DxLr30Board board;
RADIO_CLASS radio = new Module(P_LORA_NSS, P_LORA_DIO_1,
                              P_LORA_RESET, P_LORA_BUSY, SPI);
WRAPPER_CLASS radio_driver(radio, board);
VolatileRTCClock rtc_clock;
EnvironmentSensorManager sensors;

bool radio_init() {
  return radio.std_init(&SPI);
}

mesh::LocalIdentity radio_new_identity() {
  RadioNoiseListener rng(radio);
  return mesh::LocalIdentity(&rng);
}

