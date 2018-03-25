/* Jonathan Chin
 * Light Seeker bluetooth controlled robot that can detect levels of light
 * 3/25/18
 */

#include "mbed.h"
#include "ble/BLE.h"
#include "MotorService.h"
#include "SensorService.h"

DigitalOut alivenessLED(LED1, 0);

const static char     DEVICE_NAME[] = "Light Seeker";
static const uint16_t uuid16_list[] = {MotorService::MOTOR_SERVICE_UUID};

MotorService *motorServicePtr;
SensorService *sensorServiePtr;
Ticker ticker;

// PWM is speed and gpio controls direction, mode toggles PHASE/ENABLE or IN/IN
mbed::PwmOut pwm_b(P0_8);
mbed::DigitalOut gpio_b(P0_11);
mbed::PwmOut pwm_a(P0_9);
mbed::DigitalOut gpio_a(P0_10);
mbed::DigitalOut mode_pin(P0_5);
mbed::AnalogIn light_sensor(P0_4); // Used by photoresistor to detect levels of light in room
float motorSpeed = 0;

enum Directions{NORTH, EAST, SOUTH, WEST, NORTHEAST, SOUTHEAST, SOUTHWEST, NORTHWEST, STOP};

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising();
}

void toggleLED(void)
{
    alivenessLED = !alivenessLED; /* Do blinky on LED1 to indicate system aliveness. */
}

void periodicLightSensorCallback(void)
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
        
        switch(direction){
            case NORTH:
                gpio_b.write(0);
                gpio_a.write(0);
                pwm_b.write(motorSpeed);
                pwm_a.write(motorSpeed);
                break;
            case EAST:
                gpio_b.write(0);
                gpio_a.write(1);
                pwm_b.write(motorSpeed);
                pwm_a.write(motorSpeed);
                break;
            case SOUTH:
                gpio_b.write(1);
                gpio_a.write(1);
                pwm_b.write(motorSpeed);
                pwm_a.write(motorSpeed);
                break;
            case WEST:
                gpio_b.write(1);
                gpio_a.write(0);
                pwm_b.write(motorSpeed);
                pwm_a.write(motorSpeed);
                break;
            case NORTHEAST:
                gpio_b.write(1);
                pwm_b.write(motorSpeed);
                pwm_a.write(0);
                break;
            case SOUTHEAST:
                gpio_b.write(0);
                pwm_b.write(motorSpeed);
                pwm_a.write(0);
                break;
            case SOUTHWEST:
                gpio_a.write(0);
                pwm_a.write(motorSpeed);
                pwm_b.write(0);
                break;
            case NORTHWEST:
                gpio_a.write(1);
                pwm_a.write(motorSpeed);
                pwm_b.write(0);
                break;
            case STOP:
                pwm_b.write(0);
                pwm_a.write(0);
                break;
            default:  // Should not happen so call stop 
                toggleLED();
                pwm_b.write(0);
                pwm_a.write(0);
        }
  
    }else if((params->handle == motorServicePtr->getSpeedHandle()) && (params->len >= 1)){
        motorSpeed = (*(params->data))/100.0; // data will be an an int from 0 to 100. Divide by 100.0 to get a float
    }else if((params->handle == sensorServicePtr->getLEDHandle()) && (params->len == 1)){
        int enable = *(params->data);

        if enable == 1 {
            ticker.attach(periodicCallback, seconds);
        }else {
            ticker.detach();
        }
    }else if((params->handle == sensorServicePtr->getLightHandle()) && (params->len == 1)){
        int result = light_sensor.read_u16();
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
    int initialValueForLEDToggle = 0;
    int initialValueForLightLevels = 1;
    motorServicePtr = new MotorService(ble, initialValueForMotorDirectionCharacteristic,
        initialValueForMotorSpeedCharacteristic);
    sensorServicePtr = new SensorService(ble, initialValueForLEDToggle,
        initialValueForLightLevels);

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
