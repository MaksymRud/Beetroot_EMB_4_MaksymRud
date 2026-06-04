#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <string>

#define ADC_INPUT_PIN 4  // YD-ESP32-S3 onboard WS2812 (IO48)

struct Params_pair {
    int resolution;
    adc_attenuation_t attenuation;
};
bool started = false;
unsigned long nextSwitchMs = 0;
int arr_index;
int resolutions[] = {9, 10, 11, 12}; // Possible ADC resolutions (9 to 12 bits)
int attenuations[] = {ADC_0db, ADC_2_5db, ADC_6db, ADC_11db, ADC_ATTENDB_MAX}; // Possible attenuation levels
float max_measured_voltages[] = {0.95, 1.25, 1.75, 3.1, 3.3}; // Max voltage corresponding to the highest ADC value (depends on attenuation)
constexpr int resolution_length = sizeof(resolutions) / sizeof(resolutions[0]);
constexpr int attenuation_length = sizeof(attenuations) / sizeof(attenuations[0]);
const int params_len = resolution_length * attenuation_length;
unsigned long now = millis();
unsigned long before = millis();

Params_pair params_arr[params_len];

void calc_volage_on(int adc_pin, int current_resolution, float max_measured_voltage, int &adc_value, float &calculated_voltage, float &actual_voltage);
std::string attenuationToString(adc_attenuation_t att);

void calc_volage_on(int adc_pin, int current_resolution, float max_measured_voltage, int &adc_value, float &calculated_voltage, float &actual_voltage) {
    adc_value = analogRead(adc_pin);
    calculated_voltage = (adc_value / (pow(2, current_resolution) - 1)) * max_measured_voltage; // Assuming a 12-bit ADC and 3.3V reference
    actual_voltage = analogReadMilliVolts(adc_pin) / 1000.0; // Convert mV to V
}

std::string attenuationToString(adc_attenuation_t att) {
    switch (att) {
        case ADC_0db:
            return "0dB";
        case ADC_2_5db:
            return "2.5dB";
        case ADC_6db:
            return "6dB";
        case ADC_11db:
            return "11dB";
        case ADC_ATTENDB_MAX:
            return "Max Attenuation";
        default:
            return "Unknown";
    }
}

void setup() {
    Serial.begin(115200);
    arr_index = 0;

    for (int i = 0; i < resolution_length; i++) {
        for (int j = 0; j < attenuation_length; j++) {
            params_arr[i * attenuation_length + j] = {resolutions[i], (adc_attenuation_t)attenuations[j]};
        }
    }
    pinMode(ADC_INPUT_PIN, INPUT);
}

void loop() {
  unsigned long now = millis();
  int adc_value = 0;
  float calculated_voltage = 0.0;
  float actual_voltage = 0.0;

  calc_volage_on(ADC_INPUT_PIN, 
                params_arr[arr_index].resolution, 
                max_measured_voltages[params_arr[arr_index].attenuation], 
                adc_value, 
                calculated_voltage, 
                actual_voltage);
  delay(1000);
  Serial.printf("ADC Value: %d, Calculated Voltage: %.3f V, Actual Voltage: %.3f V, Error: %.3f%% V\n", adc_value, calculated_voltage, actual_voltage, std::abs((calculated_voltage - actual_voltage) / actual_voltage * 100.0));

  // Time to switch config?
  if ((now - before) >= 10000) {
    before = now;

    arr_index++;

    if (arr_index >= params_len) {
      Serial.println("Done. Reached end of params_arr. Starting from the first params pair.");
      arr_index = 0; // Reset to the first params pair
    }

    int resBits = params_arr[arr_index].resolution;
    adc_attenuation_t att = params_arr[arr_index].attenuation;
    delay(500);
    analogReadResolution(resBits);
    analogSetPinAttenuation(ADC_INPUT_PIN, att);

    (void)analogRead(ADC_INPUT_PIN);
    delayMicroseconds(50);
    Serial.println("--------------------------------------------------");
    Serial.printf("Switched to resolution=%d bits, attenuation=%s\n", resBits, attenuationToString(att).c_str());
  }
}
