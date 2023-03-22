#include "vhitek.h"

namespace VHITEK
{
    namespace Config
    {
        String toJson(hanh_dong data) //lich su giao dich
        {
            DynamicJsonDocument doc(10000);
            thoigian time;
            char rData[2048];
            char dateBuf[25];

            time.ngay=data.ngay;
            time.thang=data.thang;
            time.nam=data.nam;
            time.gio=data.gio;
            time.phut=data.phut;

            sprintf(dateBuf, "%04d-%02d-%02dT%02d:%02d:00.546715",
                    time.nam+2000, time.thang, time.ngay, time.gio, time.phut);

            doc["IDX"] = data.IDX;
            doc["Status"] = data.trang_thai;
            doc["lockerid"] = data.so_tu;
            doc["DataCheck"] = data.send_data_check;
            doc["IdCard"] = data.ID;
            doc["Time"] = dateBuf;
            doc["MID"] = apSSID;
            serializeJson(doc, rData);
            return String(rData);
        }

        String Json_tong_tu() //Tao Json tong so tu
        {
            DynamicJsonDocument doc(10000);
            char rData[2048];

            // doc["totalcabined"] = Tong_so_tu;
            doc["MID"] = apSSID;

            serializeJson(doc, rData);
            return String(rData);            
        }

        String Json_thong_tin_tu(user_setting user) //Tao Json thong tin tu
        {
            DynamicJsonDocument doc(10000);
            char rData[2048];

            doc["lockerid"] = user.so_tu;
            doc["cardid"] = user.ID_user;
            doc["password"] = user.mat_khau;
            doc["lockermachineid"] = apSSID;

            serializeJson(doc, rData);
            return String(rData);             
        }

        String Json_machine_status() //Tao Json tinh trang may
        {
            DynamicJsonDocument doc(10000);
            char dateBuf[25];
            char rData[2048];
            sprintf(dateBuf, "%04d-%02d-%02d %02d:%02d:%02d",
                    thoi_gian.nam, thoi_gian.thang, thoi_gian.ngay,
                    thoi_gian.gio, thoi_gian.phut, thoi_gian.giay);

            doc["time"] = dateBuf;
            doc["wifi"] = Wifi_RSSI();
            doc["wifiname"] = WiFi.SSID();
            doc["gsm"] = 0;
            doc["error"] = 0;
            doc["emptycabined"] = Tong_tu_chua_SD;
            doc["lockermachineid"] = apSSID;
            doc["version"] = FW;

            int total_elm=0;
            // for (int i=1;i<=Tong_so_tu;i++)
            // {
            //     if(VHITEK::mo_tu_locker::read_locker_status(i))
            //     {
            //         doc["statuscabined"][total_elm++]=i;
            //     }
            // }

            serializeJson(doc, rData);
            return String(rData);  
        }

        String loadChipID()
        {
            String IDmay;
            String Socuoi;
            uint32_t chipId = 0;
            for (int i = 0; i < 17; i = i + 8)
            {
                chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
            }
            sprintf(apSSID, "VLK%d", chipId);
            IDmay = String(apSSID);
            int start = sizeof(IDmay)-4;
            for(int i=start; i<=sizeof(IDmay); i++) Socuoi += IDmay[i];
            VHITEK::socuoiID = Socuoi;
            // Serial.printf("\r\n\r\n %s\r\n\r\n", apSSID);
            Serial.printf("\n%s\n",VHITEK::socuoiID);
            return String(apSSID);
        }    

        void xem_tung_o()
        {
            for(int i=1000; i<=1120; i++)
            {
                Serial.printf("%d: %d, ", i, myMem.read(i)-'0');
                // Serial.print(i); Serial.print(": "); Serial.print(myMem.read(i)-'0'); Serial.print(", ");
            }
            Serial.println();
        }

        /*void xem_eeprom_tu_bat_ky(int so_tu){ //XEM EEPROM 1 tu bat ky
            user_setting doc_id;
            int vitri=0;
            uint16_t o_luu_bat_dau;

            o_luu_bat_dau = VHITEK::Menu::tinh_o_luu_the(so_tu);

            doc_id = VHITEK::EEPROM::read_eeprom_1(so_tu);

            Serial.print("So tu: "); Serial.print(doc_id.so_tu);
            Serial.print(" - ID: ");
            for(int i=0; i< 13; i++)
            {
                // Serial.printf("%02x ",doc_id.ID_user[i]);
                Serial.print(doc_id.ID_user[i]-'0');
            }

            Serial.print(" - MK: ");
            for(int i=0; i< 10; i++)
            {
                Serial.print(doc_id.mat_khau[i]-'0');
            }  
            Serial.print(" - Send data: "); Serial.print(doc_id.send_data_check); //check send data
            Serial.print(" - SUM: "); Serial.print(doc_id.check_sum); //check SUM       
            Serial.println();     
        } */

        int so_sanh_mk_menu(char *mk)
        {
            uint32_t MK = atoi(mk);

            if (MK == MK_khach) return 1;
            else if (MK == MK_KT or MK == MK_MASS) return 2;
            else return 0;
        }                    

        void All_Clear_eeprom(int eep, uint16_t Max_diachi) // Xoa toan bo IC eeprom luu hanh dong mo cua
        {
            if (eep == 0) // xoa ca 2 eeprom
            {
                for (uint16_t i = 0; i <= Max_diachi; i++)
                {
                    myMem.write(i, 0);
                    myMem2.write(i, 0);
                }
            }
            else if (eep == 1) // xoa eeprom 1
            {
                for (uint16_t i = 0; i <= Max_diachi; i++)
                {
                    myMem.write(i, 0);
                }
            }
            else if (eep == 2) // xoa eeprom 2
            {
                for (uint16_t i = 0; i <= Max_diachi; i++)
                {
                    myMem2.write(i, 0);
                }
            }
            // Serial.println("Đã xóa EEPROM xong");
        }      

        uint16_t tinh_o_luu_the(uint16_t so_tu) //tinh o bat dau luu the theo so tu
        {
            return ( 2000 + ((so_tu*sizeof(cabine_config)) - sizeof(cabine_config)) ) + 1;
        }

        bool kiem_tra_tu_da_co(uint16_t so_tu) //Kiem tra xem so tu nay da luu chua
        {
            static uint16_t doc_so_tu;
            accessI2C1Bus([&]{
                    myMem.get(tinh_o_luu_the(so_tu), doc_so_tu);              
            }, 100);                  
            
            if(doc_so_tu == so_tu) return true; //Neu the da duoc luu
            else return false; //neu the chua duoc luu
        } 

        int Wifi_RSSI() //RSSI cua Wifi
        {
            if(WiFi.RSSI()==0) return 0;
            else 
            {
                if ((2 * (WiFi.RSSI() + 100)) >= 100) return 100;
                else return (2 * (WiFi.RSSI() + 100));
            }
        }

        /*uint16_t KT_tong_tu_chua_SD() //Tinh tong so tu chua su dung
        {
            uint16_t kiem_tra_co=0;
            Tong_tu_chua_SD = 0;
            int vitri=0;

            // Serial.printf("Tu SD: ");
            for(int sotu=1; sotu<=Tong_so_tu; sotu++) //dò từ 1 đến tổng số tủ
            {
                accessI2C1Bus([&]{
                        myMem.get(VHITEK::Menu::tinh_o_luu_the(sotu), kiem_tra_co);       
                }, 100);          

                if(kiem_tra_co == 0)
                {
                    tuchuasd[vitri] = sotu;
                    // Serial.printf("%d ", tuchuasd[vitri]);
                    Tong_tu_chua_SD++;
                    vitri++;
                }
            }
            // Serial.printf("Tong tu chua sd: %d \n", Tong_tu_chua_SD);
        } */

        setting_machine get_setting_machine() //đọc cài đặt máy
        {
            return save_config_machine = VHITEK::EEPROM::read_EEP_1_Machine();
        }

        bool Save_Set_Machine() //Lưu setting máy, ô 1000
        {
            if(VHITEK::EEPROM::write_EEP_1_Machine(save_config_machine)==true)
                return true; 
            else return false;
        }

        void begin()
        {

        }

        void Test_led()
        {
            // static uint64_t last_time=0;
            // if(millis() - last_time > 500)
            // {
            //     digitalWrite(PIN_PUMP, HIGH);
            //     digitalWrite(PIN_VAL1, !digitalRead(PIN_VAL1));
            //     digitalWrite(PIN_VAL2, !digitalRead(PIN_VAL2));
            //     digitalWrite(PIN_TOTAL, !digitalRead(PIN_TOTAL));
            //     last_time = millis();
            // }            
        }
    }
}