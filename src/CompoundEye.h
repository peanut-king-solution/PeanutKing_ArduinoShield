/*
  CompoundEye.h - description
  Created by name , date .
  Released into the public domain.
*/
#ifndef COMPOUNDEYE_H
#define COMPOUNDEYE_H

#include "Arduino.h"
#include "IICIT.h"

#define compound_eye_data_total 16
#define compound_eye_ir_1 0
#define compound_eye_ir_2 1
#define compound_eye_ir_3 2
#define compound_eye_ir_4 3
#define compound_eye_ir_5 4
#define compound_eye_ir_6 5
#define compound_eye_ir_7 6
#define compound_eye_ir_8 7
#define compound_eye_ir_9 8
#define compound_eye_ir_10 9
#define compound_eye_ir_11 10
#define compound_eye_ir_12 11
#define compound_eye_max_eye_value 12
#define compound_eye_max_eye 13
#define compound_eye_angle 14
#define compound_eye_modes 15

#define compound_eye_mode_single 0
#define compound_eye_mode_double 1

class CompoundEye {
  public:
  /*enum compound_eye_data_Id{
      ir_1=0,ir_2,ir_3,ir_4,ir_5,ir_6,ir_7,ir_8,ir_9,ir_10,ir_11,ir_12,max_eye_value,max_eye,angle,mode
  };*/
  //int compound_eye_data_total=16;
  CompoundEye(void){
    init();
  }
  int store[compound_eye_data_total];
  void init(void);
  void compoundEyeRead(void);
  int compoundEyeRead(uint8_t data_id);
  private:
  IICIT::Handle compoundEyeHandle;
};

extern CompoundEye compoundEye;

#endif