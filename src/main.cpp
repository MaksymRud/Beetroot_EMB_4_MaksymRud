#include <Arduino.h>
#include <OneButton.h>

constexpr uint8_t Relay_Control_Pin = 4;
constexpr uint8_t Button_Pin = 6; // ms
OneButton button(Button_Pin, true, true);
volatile bool FanWorks = false;
volatile bool FanIdle = false;
hw_timer_t *fanTimer = nullptr;
constexpr long Fan_Working_Time = 1000000;
constexpr long Fan_Idle_Time = 5000000;
bool fanProcessActive = false;
unsigned long lastSerialPrintTime = 0;


void ARDUINO_ISR_ATTR onStopFanTimer() {
	if (FanWorks) {
        FanWorks = false;
        FanIdle = true;
        timerAlarmWrite(fanTimer, Fan_Idle_Time, true);
    } else if (FanIdle) {
        FanIdle = false;
        FanWorks = true;
        timerAlarmWrite(fanTimer, Fan_Working_Time, true);
    }
}

void startFanProcess() {
	if (!fanProcessActive) {
		fanTimer = timerBegin(0, 80, true);
		timerAttachInterrupt(fanTimer, &onStopFanTimer, true);
		timerAlarmWrite(fanTimer, Fan_Working_Time, true);
		timerAlarmEnable(fanTimer);
        FanWorks = true;
		fanProcessActive = true;
		Serial.println("Timer started...");
	}
}

void stopFanProcess() {
	if (fanProcessActive) {
		timerDetachInterrupt(fanTimer);
		timerEnd(fanTimer);
		fanTimer = nullptr;
		fanProcessActive = false;
		Serial.println("!!! Timer stopped by user !!!");
	}
}

void toggleTimer() {
	if (fanProcessActive) {
		stopFanProcess();
	} else {
		startFanProcess();
	}
}

void setup() {
	Serial.begin(115200);
	button.attachClick(toggleTimer);
    pinMode(Relay_Control_Pin, OUTPUT);
}

void loop() {
    unsigned long currentTime = millis();
	button.tick();
    if (FanWorks) {
        if (digitalRead(Relay_Control_Pin) == LOW) {
            digitalWrite(Relay_Control_Pin, HIGH);
            Serial.printf("Try to open relay!\n");
            Serial.print("Fan is working\n");
        }
    } else if (FanIdle) {
        if (digitalRead(Relay_Control_Pin) == HIGH) {
            digitalWrite(Relay_Control_Pin, LOW);
            Serial.printf("Try to close relay!\n");
            Serial.print("Fan is idle\n");
        }
    }
}
