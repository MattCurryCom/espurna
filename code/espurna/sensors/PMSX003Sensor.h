// -----------------------------------------------------------------------------
// PMSX003 Dust Sensor
// Uses SoftwareSerial library
// Contribution by Òscar Rovira López
// -----------------------------------------------------------------------------

#pragma once

#include "Arduino.h"
#include "BaseSensor.h"

#include <PMS.h>
#include <SoftwareSerial.h>

class PMSX003Sensor : public BaseSensor {

    public:

        // ---------------------------------------------------------------------
        // Public
        // ---------------------------------------------------------------------

        PMSX003Sensor(): BaseSensor() {
            _count = 3;
        }

        void setGPIO(unsigned char pin_rx, unsigned char pin_tx) {
            _pin_rx = pin_rx;
            _pin_tx = pin_tx;
        }

        // ---------------------------------------------------------------------
        // Sensor API
        // ---------------------------------------------------------------------

        // Initialization method, must be idempotent
        void begin() {

            if (_serial) delete _serial;
            if (_pms) delete _pms;

            _serial = new SoftwareSerial(_pin_rx, _pin_tx, false, 256);
            _serial->begin(9600);
            _pms = new PMS(* _serial);
            _pms->passiveMode();

            _startTime = millis();

        }

        // Descriptive name of the sensor
        String name() {
            char buffer[28];
            snprintf(buffer, sizeof(buffer), "PMSX003 @ SwSerial(%i,%i)", _pin_rx, _pin_tx);
            return String(buffer);
        }

        // Descriptive name of the slot # index
        String slot(unsigned char index) {
            if (index < _count) {
                _error = SENSOR_ERROR_OK;
                char buffer[36];
                if (index == 0) snprintf(buffer, sizeof(buffer), "PM1.0 @ PMSX003 @ SwSerial(%i,%i)", _pin_rx, _pin_tx);
                if (index == 1) snprintf(buffer, sizeof(buffer), "PM2.5 @ PMSX003 @ SwSerial(%i,%i)", _pin_rx, _pin_tx);
                if (index == 2) snprintf(buffer, sizeof(buffer), "PM10 @ PMSX003 @ SwSerial(%i,%i)", _pin_rx, _pin_tx);
                return String(buffer);
            }
            _error = SENSOR_ERROR_OUT_OF_RANGE;
            return String();
        }

        // Type for slot # index
        magnitude_t type(unsigned char index) {
            if (index < _count) {
                _error = SENSOR_ERROR_OK;
                if (index == 0) return MAGNITUDE_PM1dot0;
                if (index == 1) return MAGNITUDE_PM2dot5;
                if (index == 2) return MAGNITUDE_PM10;
            }
            _error = SENSOR_ERROR_OUT_OF_RANGE;
            return MAGNITUDE_NONE;
        }

        void pre() {
            if(millis() - _startTime > 30000) {
                _error = SENSOR_ERROR_OK;
            } else {
                _error = SENSOR_ERROR_WARM_UP;
            }
            _pms->requestRead();
        }

        void tick() {
            if(_pms->read(_data)) {
                _pm1dot0 = _data.PM_AE_UG_1_0;
                _pm2dot5 = _data.PM_AE_UG_2_5;
                _pm10 = _data.PM_AE_UG_10_0;
            }
        }

        // Current value for slot # index
        double value(unsigned char index) {
            if (index < _count) {
                _error = SENSOR_ERROR_OK;
                if(index == 0) return _pm1dot0;
                if(index == 1) return _pm2dot5;
                if(index == 2) return _pm10;
            }
            _error = SENSOR_ERROR_OUT_OF_RANGE;
            return 0;
        }

    protected:
        unsigned int _pm1dot0;
        unsigned int _pm2dot5;
        unsigned int _pm10;
        unsigned int _pin_rx;
        unsigned int _pin_tx;
        unsigned long _startTime;
        SoftwareSerial * _serial;
        PMS * _pms;
        PMS::DATA _data;

};
