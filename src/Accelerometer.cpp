#include "Accelerometer.h"
#include <stddef.h>

int16_t Accelerometer::i2cRead(uint8_t reg){
    uint8_t temp={reg};
    uint8_t _status=gIIC->Write(accelerometerHandle,&temp, 1);
    gIIC->Read(accelerometerHandle,&temp,1);
    return temp;
}

int16_t Accelerometer::readData(uint8_t reg){
    int16_t h = i2cRead(reg);
    int16_t l = i2cRead(reg + 1);
    int16_t value = (h << 8) + l;

    if (value >= 0x8000) {
        return -((65535 - value) + 1);
    } else {
        return value;
    }
}

void Accelerometer::updateAcceleration(accelSen sensitivity,int16_t &xAccel,int16_t &yAccel,int16_t &zAccel) {
    int32_t accelRange = 0;
    switch (sensitivity) {
        case range_2_g: accelRange = 16384; break;
        case range_4_g: accelRange = 8192; break;
        case range_8_g: accelRange = 4096; break;
        case range_16_g: accelRange = 2048; break;
    }
    xAccel = readData(xAccelAddr) / accelRange;
    yAccel = readData(yAccelAddr) / accelRange;
    zAccel = readData(zAccelAddr) / accelRange;
}

void Accelerometer::updateGyroscope(gyroSen sensitivity,int16_t &xGyro,int16_t &yGyro,int16_t &zGyro) {
    double gyroRange = 0;
    switch (sensitivity) {
        case range_250_dps: gyroRange = 131; break;
        case range_500_dps: gyroRange = 65.5; break;
        case range_1000_dps: gyroRange = 32.8; break;
        case range_2000_dps: gyroRange = 16.4; break;
    }
    xGyro = readData(xGyroAddr) / gyroRange;
    yGyro = readData(yGyroAddr) / gyroRange;
    zGyro = readData(zGyroAddr) / gyroRange;
}

void Accelerometer::initMPU6050() {
    uint8_t temp[2] = { power_mgmt, 0 };
    gIIC->Write(accelerometerHandle,temp, 2);
}

float Accelerometer::gyroscope(axisXYZ axis, gyroSen sensitivity) {
    int16_t xGyro = 0,yGyro = 0,zGyro = 0;
    updateGyroscope(sensitivity,xGyro,yGyro,zGyro);
    if (axis == axisXYZ::axis_x) {
        return xGyro;
    } else if (axis == axisXYZ::axis_y) {
        return yGyro;
    } else {
        return zGyro;
    }
}

float Accelerometer::axisRotation(axisXYZ axis, accelSen sensitivity) {
    int16_t xAccel = 0,yAccel = 0,zAccel = 0;
    updateAcceleration(sensitivity,xAccel,yAccel,zAccel);
    double radians;
    if (axis == axisXYZ::axis_x) {
        radians = atan2(yAccel, dist(xAccel, zAccel));
    } else if (axis == axisXYZ::axis_y) {
        radians = -atan2(xAccel, dist(yAccel, zAccel));
    } else if (axis == axisXYZ::axis_z) {
        radians = atan2(zAccel, dist(xAccel, yAccel));
    }

    // Convert radian to degrees and return
    const double pi = M_PI;
    double degrees = radians * (180 / pi);
    return degrees;
}


float Accelerometer::axisAcceleration(axisXYZ axis, accelSen sensitivity) {
    int16_t xAccel = 0,yAccel = 0,zAccel = 0;
    updateAcceleration(sensitivity,xAccel,yAccel,zAccel);
    // Return acceleration of specific axis
    if (axis == axisXYZ::axis_x) {
        return xAccel;
    } else if (axis == axisXYZ::axis_y) {
        return yAccel;
    } else {
        return zAccel;
    }
}

float Accelerometer::readTemperature() {
    double rawTemp = readData(Accelerometer::tempAddr);
    return 36.53 + rawTemp / 340;
}

