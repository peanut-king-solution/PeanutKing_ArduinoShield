#include <PeanutKingArduinoShield.h>

PeanutKingArduinoShield robot;

double x_Acceleration=0,y_Acceleration=0,z_Acceleration=0;
double x_Gyroscope=0,y_Gyroscope=0,z_Gyroscope=0;
double x_Rotation=0,y_Rotation=0,z_Rotation=0;

void setup() {
  Serial.begin(9600); // start serial for output
}

void loop() {

    x_Acceleration=robot.getAxisAcceleration(axis_x,range_4_g); //get gyroscope axis Acceleration by input axis and sensitivity 
    y_Acceleration=robot.getAxisAcceleration(axis_y,range_8_g); //4 choices of sensitivity : range_2_g/range_4_g/range_8_g/range_16_g                                                                                   
    z_Acceleration=robot.getAxisAcceleration(axis_z,range_16_g);                   

    x_Rotation=robot.getAxisRotation(axis_x,range_2_g);         //get gyroscope axis Rotation by input axis and sensitivity
    y_Rotation=robot.getAxisRotation(axis_y,range_4_g);         //4 choices of sensitivity : range_2_g/range_4_g/range_8_g/range_16_g
    z_Rotation=robot.getAxisRotation(axis_z,range_8_g);                                       

    x_Gyroscope=robot.getGyroscope(axis_x,range_500_dps);       //get gyroscope value by input axis and sensitivity     
    y_Gyroscope=robot.getGyroscope(axis_y,range_1000_dps);      //4 choices of sensitivity : range_250_dps/range_500_dps/range_1000_dps/range_2000_dps   
    z_Gyroscope=robot.getGyroscope(axis_z,range_2000_dps);      //dps: degree per second

                                                                //serial print out all the result
    Serial.print("x_Acceleration: ");Serial.println(x_Acceleration);
    Serial.print("y_Acceleration: ");Serial.println(y_Acceleration);
    Serial.print("z_Acceleration: ");Serial.println(z_Acceleration);

    Serial.print("x_Rotation: ");Serial.println(x_Rotation);
    Serial.print("y_Rotation: ");Serial.println(y_Rotation);
    Serial.print("z_Rotation: ");Serial.println(z_Rotation);

    Serial.print("x_Gyroscope: ");Serial.println(x_Gyroscope);
    Serial.print("y_Gyroscope: ");Serial.println(y_Gyroscope);
    Serial.print("z_Gyroscope: ");Serial.println(z_Gyroscope);
  
}
