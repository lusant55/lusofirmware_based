#pragma once
#include <helpers/NRF52Board.h>
#include <Wire.h>


// Arduino indices, NOT raw Nordic GPIO numbers.
#define P_LORA_SCLK 12  // P1.11, marking 111
#define P_LORA_MOSI 14  // P1.15, marking 115
#define P_LORA_MISO 15  // P0.02, marking 002
#define P_LORA_NSS 13   // P1.13, marking 113
#define P_LORA_BUSY 16  // P0.29, marking 029
#define P_LORA_DIO_1 3  // P0.20, marking 020
#define P_LORA_RESET 4  // P0.22, marking 022
#define SX126X_TXEN 5   // P0.24, marking 024
#define SX126X_RXEN 2   // P0.17, marking 017
#define SX126X_POWER_EN 21 // P0.13, external ME6217 enable (not a radio signal)
#define SX126X_DIO2_AS_RF_SWITCH false
#define SX126X_DIO3_TCXO_VOLTAGE (0.0f)

class DxLr30Board : public NRF52Board {
public:
  DxLr30Board() : NRF52Board("Luso_DXLR30") {}
  void begin() override {
    NRF52Board::begin();
    // EnvironmentSensorManager scans Wire even when no sensors are enabled.
    // Initialize TWIM first so endTransmission() cannot wait on a disabled bus.
    Wire.begin();
    digitalWrite(SX126X_TXEN, LOW);
    digitalWrite(SX126X_RXEN, LOW);
    pinMode(SX126X_TXEN, OUTPUT);
    pinMode(SX126X_RXEN, OUTPUT);
    // Enable the board's external 3.3 V supply before radio initialization.
    digitalWrite(SX126X_POWER_EN, HIGH);
    pinMode(SX126X_POWER_EN, OUTPUT);
    delay(10);
  }
  uint16_t getBattMilliVolts() override {
    // USB supplies VDDH on this board: it is not then a battery measurement.
    // Zero retains the existing unavailable-value convention.
    if (isExternalPowered() ||
        !(NRF_POWER->MAINREGSTATUS & POWER_MAINREGSTATUS_MAINREGSTATUS_Msk)) return 0;

    analogReference(AR_INTERNAL_1_2);
    analogReadResolution(12);
    analogOversampling(1);
    analogSampleTime(10);
    (void)analogReadVDDHDIV5(); // Discard first conversion after changing reference.
    uint32_t total = 0;
    for (unsigned i = 0; i < 8; ++i) total += analogReadVDDHDIV5();
    // 12-bit ADC, 1.2 V full scale, internal VDDH / 5 input.
    const uint16_t mv = (total * 6000UL + 16384UL) / 32768UL;
    analogReference(AR_DEFAULT);
    analogReadResolution(10);
    analogSampleTime(3);
    return isExternalPowered() ? 0 : mv;
  }
  const char* getManufacturerName() const override {
    return "TENSTAR nRF52840 / DX-LR30";
  }
};
