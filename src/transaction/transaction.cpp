#include "vhitek.h"

namespace VHITEK
{
    namespace transaction
    {       
        void save_trans(uint16_t so_tu, uint8_t cabine_status) //lưu trans
        {
            cabine_transac giao_dich; 
            cabine_config user_read = VHITEK::EEPROM::read_EEP_1_Cabine(so_tu);

            giao_dich.IDX = IDX_hien_tai;
            giao_dich.status_cabine = cabine_status; //trang thai cua
            giao_dich.so_tu = so_tu;
            giao_dich.send_data = 1; //1: Chua gui; 0: da gui
            sprintf(giao_dich.RFID, "%s", user_read.ID_user);
            sprintf(giao_dich.mat_khau, "%s", user_read.mat_khau);
            giao_dich.time_receive = thoi_gian;

            /*Serial.print("IDX: "); Serial.print(giao_dich.IDX);
            Serial.print(" - Status: "); Serial.print(giao_dich.status_cabine);
            Serial.print(" - So tu: "); Serial.print(giao_dich.so_tu);
            Serial.print(" - Send data: "); Serial.print(giao_dich.send_data);
            Serial.printf(" - ID: %s", giao_dich.RFID);
            Serial.printf(" - MK: %s", giao_dich.mat_khau);       
            Serial.print(" - Ngay: "); Serial.print(giao_dich.time_receive.ngay);
            Serial.print(" - Thang: "); Serial.print(giao_dich.time_receive.thang);
            Serial.print(" - Nam: "); Serial.print(giao_dich.time_receive.nam);
            Serial.print(" | Gio: "); Serial.print(giao_dich.time_receive.gio);
            Serial.print(" - Phut: "); Serial.println(giao_dich.time_receive.phut); */

            if(VHITEK::EEPROM::write_eeprom_2(giao_dich)==true) //Neu ghi duoc vao EEPROM
            {
                // Serial.println("LƯU THÀNH CÔNG");
                last_trans = giao_dich;
                IDX_hien_tai += 1;
                VHITEK::diachi_giaodich = dia_chi_IDX_hien_tai;
                if(dia_chi_IDX_hien_tai >= 63000) dia_chi_IDX_hien_tai = 0;
                else dia_chi_IDX_hien_tai += sizeof(cabine_transac);
            }
            else  //KHONG GHI vao EEPROM DUOC
            {
                // Serial.println("SAI........!");
                ;;
            }
            // Serial.print("IDX hien tai: "); Serial.print(IDX_hien_tai);
            // Serial.print(" - Dia chi hien tai: "); Serial.println(dia_chi_IDX_hien_tai);                    
        }

        //ham viet de load du lieu tu EEPROM moi khi khoi dong lai ESP32
        void load_du_lieu()
        {    
            cabine_transac read_trans; 
            uint32_t doc_idx;    
            uint32_t last_IDX=0;

            static float phantram;     
            uint32_t startTick = millis();     

            /*dia_chi_IDX = dia_chi_IDX*sizeof(giao_dich);

            for(uint32_t i=0; i<=dia_chi; i+=sizeof(giao_dich)) //XOA toan bo EEPROM 2
            {
                myMem2.put(i, 0);
            }

            uint32_t idx_test=1;
            for(uint32_t i=0; i<=63000; i+=sizeof(giao_dich))
            {
                accessI2C1Bus([&]{
                        myMem2.put(i, idx_test);       
                }, 100);                  
              idx_test++;
            }

            uint32_t idx_test=1;
            for(uint32_t i=1; i<=4; i++)
            {
              myMem2.put( ((i-1)*sizeof(giao_dich)), idx_test );
              idx_test++;
            }            

            uint32_t test_3 = 1230;
            myMem2.put(56, test_3);

            Serial.print("Doc- ");
            for(uint32_t i=1; i<=5; i++)
            {
                myMem2.get( ((i-1)*sizeof(giao_dich)), doc_idx ); 
                Serial.print( ((i-1)*sizeof(giao_dich)) ); Serial.print(": "); Serial.print(doc_idx); Serial.print(", ");
            }
            Serial.println(); */

            //IDX Chạy từ 1 đến  Max IDX là 2285
            //Nếu không tìm thấy ô 0 nào thì cho IDX=0 và dia_chi_idx = 1;
            for(uint32_t i=0; i<= 64000; i+=sizeof(cabine_transac)) //i là IDX từ 0-2285
            {
                if((uint32_t)(millis() - startTick) > 100)
                {
                    phantram = (float)i/64000*100;
                    u8g2.clearBuffer();
                    u8g2.setFont(u8g2_font_resoledbold_tr);
                    u8g2.setCursor(15,25);
                    u8g2.printf("DANG LOAD DU LIEU"); 
                    u8g2.setFont(u8g2_font_ncenB10_tr);
                    u8g2.setCursor(85,45);
                    u8g2.print("%");                 
                    u8g2.setFont(u8g2_font_ncenB10_tr);
                    u8g2.setCursor(45,45);
                    u8g2.printf("%0.2f", phantram);               
                    u8g2.sendBuffer();   
                    startTick = millis();
                }     

                accessI2C1Bus([&]{
                        EEPROM::myMem2.get(i, read_trans);       
                }, 100);  
                
                // Serial.printf("last idx: %d - readIDX: %d\n", last_IDX, read_trans.IDX);
                if(read_trans.IDX>=0)
                {
                    if(last_IDX>read_trans.IDX) //5>3
                    {
                        IDX_hien_tai = last_IDX+1;
                        if(i>=63000) dia_chi_IDX_hien_tai = 0;
                        else dia_chi_IDX_hien_tai = i+1;
                        // Serial.print(" - doc IDX OK: "); Serial.print(doc_idx);
                        // Serial.print(" - diachi OK: "); Serial.println(i);                       
                        break;
                    }
                    else 
                    {
                        // Serial.print(" - doc IDX: "); Serial.print(doc_idx);
                        // Serial.print(" - diachi: "); Serial.println(i);  
                        last_IDX = read_trans.IDX;
                    }
                }
            }
            Serial.print("IDX: "); Serial.print(IDX_hien_tai);
            Serial.print(" - Dia chi: "); Serial.println(dia_chi_IDX_hien_tai);
        }

    }
}