#include "vhitek.h"

namespace VHITEK
{
    namespace transaction
    {       
        void XEM_eeprom_2(uint32_t IDX)
        {
            hanh_dong user;

            accessI2C1Bus([&]{
                myMem2.get( ((IDX-1)*sizeof(user)), user );
            }, 100);  

            Serial.print("IDX: "); Serial.print(user.IDX);
            Serial.print(" - Status: "); Serial.print(user.trang_thai);
            Serial.print(" - So tu: "); Serial.print(user.so_tu);
            Serial.print(" - Send data: "); Serial.print(user.send_data_check);
            Serial.print(" - Ngay: "); Serial.print(user.ngay);
            Serial.print(" - Thang: "); Serial.print(user.thang);
            Serial.print(" - Nam: "); Serial.print(user.nam);
            Serial.print(" | Gio: "); Serial.print(user.gio);
            Serial.print(" - Phut: "); Serial.print(user.phut);  

            Serial.print(" - ID: ");          
            for(int i=0; i<13; i++)
            {
                Serial.print(user.ID[i]-'0');
            }
            Serial.print(" - CHECK SUM: "); Serial.print(user.check_sum);
            Serial.println();
        }

        void luu_hanh_dong(uint16_t so_tu, uint8_t trang_thai)
        {
            hanh_dong giao_dich; 
            user_setting user_ID;
            uint16_t o_luu_ID = VHITEK::Menu::tinh_o_luu_the(so_tu);
            accessI2C1Bus([&]{
                    myMem.get(o_luu_ID, user_ID);
            }, 100);             

            giao_dich.IDX = IDX_hien_tai;
            giao_dich.trang_thai = trang_thai; //trang thai cua
            giao_dich.so_tu = so_tu;
            giao_dich.send_data_check = 1; //1: Chua gui; 0: da gui
            for(int i=0; i<=12; i++)
            {
                giao_dich.ID[i] = user_ID.ID_user[i];
            }

            giao_dich.ngay = thoi_gian.ngay;
            giao_dich.thang = thoi_gian.thang;
            giao_dich.nam = thoi_gian.nam-2000;
            giao_dich.gio = thoi_gian.gio;
            giao_dich.phut = thoi_gian.phut;

            /*Serial.print("IDX: "); Serial.print(giao_dich.IDX);
            Serial.print(" - Status: "); Serial.print(giao_dich.trang_thai);
            Serial.print(" - So tu: "); Serial.print(giao_dich.so_tu);
            Serial.print(" - Send data: "); Serial.print(giao_dich.send_data_check);
            Serial.print(" - ID: ");
            for(int i=0; i<=12; i++)
            {
                Serial.print(giao_dich.ID[i]-'0');
                // Serial.printf("%02X ",giao_dich.ID[i]);
            }            

            Serial.print(" - Ngay: "); Serial.print(giao_dich.ngay);
            Serial.print(" - Thang: "); Serial.print(giao_dich.thang);
            Serial.print(" - Nam: "); Serial.print(giao_dich.nam);
            Serial.print(" | Gio: "); Serial.print(giao_dich.gio);
            Serial.print(" - Phut: "); Serial.println(giao_dich.phut); */

            if(VHITEK::EEPROM::write_eeprom_2(giao_dich)==true) //Neu ghi duoc vao EEPROM
            {
                // Serial.println("Dung");
                // XEM_eeprom_2(IDX_hien_tai); //Xem tai dia chi vua luu
                diachi_giaodich = dia_chi_IDX_hien_tai;
                IDX_hien_tai ++;
                if(dia_chi_IDX_hien_tai >= 63000) dia_chi_IDX_hien_tai = 0;
                else dia_chi_IDX_hien_tai += sizeof(giao_dich);
            }
            else  //KHONG GHI vao EEPROM DUOC
            {
                // Serial.println("SAI........!");
            }
            // Serial.print("IDX hien tai: "); Serial.print(IDX_hien_tai);
            // Serial.print(" - Dia chi hien tai: "); Serial.println(dia_chi_IDX_hien_tai);                    
        }

        //ham viet de load du lieu tu EEPROM moi khi khoi dong lai ESP32
        void load_du_lieu()
        {    
            hanh_dong giao_dich; 
            uint32_t doc_idx;    
            uint32_t last_IDX=0;

            static float phantram;     
            uint32_t startTick = millis();   

            VHITEK::Config::Read_ID_master();  

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
            for(uint32_t i=0; i<= 64000; i+=sizeof(giao_dich)) //i là IDX từ 0-2285
            {
                if((uint32_t)(millis() - startTick) > 200)
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
                        myMem2.get(i, doc_idx);       
                }, 100);  
                
                // Serial.print("last idx: "); Serial.print(last_IDX);
                if(last_IDX>doc_idx) //5>3
                {
                    IDX_hien_tai = last_IDX+1;
                    if(i>=63000) dia_chi_IDX_hien_tai = 0;
                    else dia_chi_IDX_hien_tai = i;
                    // Serial.print(" - doc IDX OK: "); Serial.print(doc_idx);
                    // Serial.print(" - diachi OK: "); Serial.println(i);                       
                    break;
                }
                else 
                {
                    // Serial.print(" - doc IDX: "); Serial.print(doc_idx);
                    // Serial.print(" - diachi: "); Serial.println(i);  
                    last_IDX = doc_idx;
                }
            }
            // Serial.print("IDX: "); Serial.print(IDX_hien_tai);
            // Serial.print(" - Dia chi: "); Serial.println(dia_chi_IDX_hien_tai);
        }

    }
}