/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "ble/BLE.h"
#include "MotorService.h"

DigitalOut alivenessLED(LED1, 0);

const static char     DEVICE_NAME[] = "Light Seeker";
static const uint16_t uuid16_list[] = {MotorService::MOTOR_SERVICE_UUID};

MotorService *motorServicePtr;

// PWM is speed and gpio controls direction, mode toggles PHASE/ENABLE or IN/IN
mbed::PwmOut pwm_b(P0_8);
mbed::DigitalOut gpio_b(P0_11);
mbed::PwmOut pwm_a(P0_9);
mbed::DigitalOut gpio_a(P0_10);
mbed::DigitalOut mode_pin(P0_5);
float motorSpeed = 0;

enum Directions{NORTH, EAST, SOUTH, WEST, NORTHEAST, SOUTHEAST, SOUTHWEST, NORTHWEST};

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising();
}

void toggleLED(void)
{
    alivenessLED = !alivenessLED; /* Do blinky on LED1 to indicate system aliveness. */
}

/**
 * This callback allows the LEDService to receive updates to the ledState Characteristic.
 *
 * @param[in] params
 *     Information about the characterisitc being updated.
 */
void onDataWrittenCallback(const GattWriteCallbackParams *params) {
    if ((params->handle == motorServicePtr->getDirectionHandle()) && (params->len >= 1)) {
        int direction = *(params->data);

        pwm_b.write(motorSpeed);
        gpio_b.write(0);
        pwm_a.write(motorSpeed);
        gpio_a.write(0);
  
    }else if((params->handle == motorServicePtr->getSpeedHandle()) && (params->len >= 1)){
        motorSpeed = *(params->data);
    }else{
        //Else an error occured, toggle LED and send message back to Android phone saying invalid message
        toggleLED();
    }
}

/**
 * This function is called when the ble initialization process has failed
 */
void onBleInitError(BLE &ble, ble_error_t error)
{
    /* Initialization error handling should go here */
    // Turn LED to indicate an error has occured
    toggleLED();
}

/**
 * Callback triggered when the ble initialization process has finished
 */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        /* In case of error, forward the error handling to onBleInitError */
        onBleInitError(ble, error);
        return;
    }

    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }
 
    ble.gap().onDisconnection(disconnectionCallback);
    ble.gattServer().onDataWritten(onDataWrittenCallback);

    int initialValueForMotorDirectionCharacteristic = 0;
    float initialValueForMotorSpeedCharacteristic = 0.0;
    motorServicePtr = new MotorService(ble, initialValueForMotorDirectionCharacteristic,
        initialValueForMotorSpeedCharacteristic);

    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms. */
    ble.gap().startAdvertising();
}

int main(void)
{
    // Phase/Enable mode
    mode_pin.write(1);
    
    // Defaults to moving forward
    gpio_a.write(0);
    gpio_b.write(0);
    // Defaults to moving at zero speed
    pwm_b.write(0);
    pwm_a.write(0);
    
    BLE &ble = BLE::Instance();
    ble.init(bleInitComplete);
    
    /* SpinWait for initialization to complete. This is necessary because the
     * BLE object is used in the main loop below. */
    while (ble.hasInitialized()  == false) { /* spin loop */ }

    while (true) {
        ble.waitForEvent();
    }
}
