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

#ifndef __BLE_MOTOR_SERVICE_H__
#define __BLE_MOTOR_SERVICE_H__

class MotorService {
public:
    const static uint16_t MOTOR_SERVICE_UUID                  = 0xA000;
    const static uint16_t MOTOR_DIRECTION_CHARACTERISTIC_UUID = 0xA001;
    const static uint16_t MOTOR_SPEED_CHARACTERISTIC_UUID     = 0xA002;

    MotorService(BLEDevice &_ble, uint8_t initialValueForMotorDirectionCharacteristic, uint8_t initialValueForMotorSpeedCharacteristic) :
        ble(_ble), motorDirection(MOTOR_DIRECTION_CHARACTERISTIC_UUID, &initialValueForMotorDirectionCharacteristic),
        motorSpeed(MOTOR_SPEED_CHARACTERISTIC_UUID, &initialValueForMotorSpeedCharacteristic)
    {
        GattCharacteristic *charTable[] = {&motorDirection, &motorSpeed};
        GattService         motorService(MOTOR_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.addService(motorService);
    }
    
    GattAttribute::Handle_t getDirectionHandle() const {
        return motorDirection.getValueHandle();
    }
    
    GattAttribute::Handle_t getSpeedHandle() const {
        return motorSpeed.getValueHandle();
    }

private:
    BLEDevice                         &ble;
    ReadWriteGattCharacteristic<uint8_t>  motorDirection;
    ReadWriteGattCharacteristic<uint8_t>  motorSpeed; // Will divide by 100.0 to get float
};

#endif /* #ifndef __BLE_MOTOR_SERVICE_H__ */
