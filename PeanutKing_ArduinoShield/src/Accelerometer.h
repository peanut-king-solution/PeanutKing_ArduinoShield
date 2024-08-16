#include <Arduino.h>
#include <stddef.h>
#include "IICIT.h"

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H
typedef enum{
    axis_x,
    axis_y,
    axis_z
}axisXYZ;

typedef enum{
    range_2_g,
    range_4_g,
    range_8_g,
    range_16_g
}accelSen;

typedef enum{
    range_250_dps,
    range_500_dps,
    range_1000_dps,
    range_2000_dps
}gyroSen;

class Accelerometer {
    public:
        Accelerometer(void) {
            accelerometerHandle=gIIC->RegisterDevice(i2cAddress, 1, IICIT::Speed::SLOW);
        }
    
        static const uint8_t i2cAddress = 0x68;
        static const uint8_t power_mgmt = 0x6b;
        static const uint8_t xAccelAddr = 0x3b;
        static const uint8_t yAccelAddr = 0x3d;
        static const uint8_t zAccelAddr = 0x3f;
        static const uint8_t xGyroAddr = 0x43;
        static const uint8_t yGyroAddr = 0x45;
        static const uint8_t zGyroAddr = 0x47;
        static const uint8_t tempAddr = 0x41;

        int16_t i2cRead(uint8_t reg);

        int16_t readData(uint8_t reg);

        double dist(double a, double b) {
            return sqrt((a * a) + (b * b));
        }
    private:
        bool inited[8];
        
        void updateAcceleration(accelSen sensitivity,int16_t &xAccel,int16_t &yAccel,int16_t &zAccel);

        void updateGyroscope(gyroSen sensitivity,int16_t &xGyro,int16_t &yGyro,int16_t &zGyro);
    public:
        void initMPU6050();
        float gyroscope(axisXYZ axis, gyroSen sensitivity);

        float axisRotation(axisXYZ axis, accelSen sensitivity);


        float axisAcceleration(axisXYZ axis, accelSen sensitivity);

        float readTemperature();
    IICIT::Handle accelerometerHandle;
};  

#endif