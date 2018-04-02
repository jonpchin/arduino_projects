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

#ifndef __BLE_SENSOR_SERVICE_H__
#define __BLE_SENSOR_SERVICE_H__

#include "ble/BLE.h"

class SensorService {
public:
    const static uint16_t SENSOR_SERVICE_UUID                = 0xA003;
    const static uint16_t LED_TOGGLE_CHARACTERISTIC_UUID     = 0xA004;
    const static uint16_t LIGHT_LEVELS_CHARACTERISTIC_UUID   = 0xA005;

    SensorService(BLEDevice &_ble, uint8_t initialValueLEDToggleCharacteristic, uint8_t initialValueForLightLevelsCharacteristic) :
        ble(_ble), ledToggle(LED_TOGGLE_CHARACTERISTIC_UUID, &initialValueLEDToggleCharacteristic),
        lightLevel(LIGHT_LEVELS_CHARACTERISTIC_UUID, &initialValueForLightLevelsCharacteristic)
    {
        GattCharacteristic *charTable[] = {&ledToggle, &lightLevel};
        GattService         sensorService(SENSOR_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.addService(sensorService);
    }
    
    GattAttribute::Handle_t getLEDHandle() const {
        return ledToggle.getValueHandle();
    }
    
    void updateLightLevel(uint8_t level) {
        newLevel = level;
        ble.gattServer().write(lightLevel.getValueHandle(), &newLevel, 1);
    }

private:
    uint8_t newLevel;
    BLEDevice                         &ble;
    ReadWriteGattCharacteristic<uint8_t>  ledToggle;
    ReadWriteGattCharacteristic<uint8_t>   lightLevel; // Will divide by 100.0 to get float
};

#endif /* #ifndef __BLE_SENSOR_SERVICE_H__ */
