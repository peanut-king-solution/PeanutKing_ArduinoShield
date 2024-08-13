#include "CompoundEye.h"

void CompoundEye::init(){
    compoundEyeHandle=gIIC->RegisterDevice(0x13, 1, IICIT::Speed::SLOW);
    //Wire.setClock(100000);
    //Wire.begin();
    Serial.begin(115200);
}

int CompoundEye::compoundEyeRead(uint8_t data_id){
    //send data_id
    //wried but no bug
    uint8_t temp={data_id};
    uint8_t _status = gIIC->Write(compoundEyeHandle,&temp, 1);
    //request data
    if(gIIC->Read(compoundEyeHandle,&temp,1)==IICIT::status_t::STATUS_OK){
        
        if(temp==255){
            store[data_id]=-1;
        }else{
            store[data_id]=temp;
            if(data_id==compound_eye_angle){
                store[data_id]*=2;
            }else if(data_id==compound_eye_max_eye){
                store[data_id]+=1;
            }
        }
    }
    return store[data_id];
}

void CompoundEye::compoundEyeRead(){
    for(int i=0;i<compound_eye_data_total;i++){
        compoundEyeRead(i);
    }
    if(store[compound_eye_modes]==compound_eye_mode_double){
        Serial.println(
            " IR1 = " + String(store[compound_eye_ir_1]) + "\t"+
            " IR2 = " + String(store[compound_eye_ir_2]) + "\t"+
            " IR3 = " + String(store[compound_eye_ir_3]) +"\t"+
            " IR4 = " + String(store[compound_eye_ir_4]) +"\t"+
            " IR5 = " + String(store[compound_eye_ir_5]) +"\t"+
            " IR6 = " + String(store[compound_eye_ir_6]) +"\t"+
            " IR7 = " + String(store[compound_eye_ir_7]) +"\t"+
            " IR8 = " + String(store[compound_eye_ir_8]) +"\t"+
            " IR9 = " + String(store[compound_eye_ir_9]) +"\t"+
            " IR10 = " + String(store[compound_eye_ir_10]) +"\t"+
            " IR11 = " + String(store[compound_eye_ir_11]) +"\t"+
            " IR12 = " + String(store[compound_eye_ir_12]) +"\t"+
            " max_eye_value" + String(store[compound_eye_max_eye_value]) +"\t"+
            " max_eye" + String(store[compound_eye_max_eye]) +"\t"+
            " angle" + String(store[compound_eye_angle]) +"\t"+
            " mode" + String(store[compound_eye_modes]) +"\t"
        );
    }else{
        Serial.println(
            " IR1 = " + String(store[compound_eye_ir_1]) + "\t"+
            " IR2 = " + String(store[compound_eye_ir_2]) + "\t"+
            " IR3 = " + String(store[compound_eye_ir_3]) +"\t"+
            " IR4 = " + String(store[compound_eye_ir_4]) +"\t"+
            " IR5 = " + String(store[compound_eye_ir_5]) +"\t"+
            " IR6 = " + String(store[compound_eye_ir_6]) +"\t"+
            " IR7 = " + String(store[compound_eye_ir_7]) +"\t"+
            " max_eye_value" + String(store[compound_eye_max_eye_value]) +"\t"+
            " max_eye" + String(store[compound_eye_max_eye]) +"\t"+
            " angle" + String(store[compound_eye_angle]) +"\t"+
            " mode" + String(store[compound_eye_modes]) +"\t"
        );
    }
}

//CompoundEye compoundEye;