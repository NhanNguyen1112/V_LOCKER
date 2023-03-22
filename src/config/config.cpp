#include "vhitek.h"

namespace VHITEK
{
    namespace Config
    {
        String test(uint32_t IDX, int stastus)
        {
            DynamicJsonDocument doc(10000);
            char rData[2048];

            doc["status"] = stastus;
            doc["MID"] = apSSID;
            doc["IDX"] = IDX;

            serializeJson(doc, rData);
            return String(rData); 
        }
        void sendtest(uint32_t IDX, int status)
        {
            DynamicJsonDocument doc(10000);
            HTTPClient http;
            http.begin("http://192.168.55.65:8001/"); //Specify the URL
            http.addHeader("Content-Type", "application/json");              
            String json_data=VHITEK::Config::test(IDX, status);
            // Serial.println("Gửi data đi: "); Serial.println(json_data);
            int post = http.POST(json_data.c_str());
            // Serial.printf("Post Status: %d\n", post);
            if (post == 1)  //Check for the returning code
            { 
                Serial.printf("Bật Camera: %d\n", post);
            }
            else Serial.printf("Tắt Camera: %d\n", post);
            http.end(); 
        }

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

            doc["totalcabined"] = Tong_so_tu;
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
            for (int i=1;i<=Tong_so_tu;i++)
            {
                if(VHITEK::mo_tu_locker::read_locker_status(i))
                {
                    doc["statuscabined"][total_elm++]=i;
                }
            }

            serializeJson(doc, rData);
            return String(rData);  
        }

        String loadChipID()
        {
            uint32_t chipId = 0;
            for (int i = 0; i < 17; i = i + 8)
            {
                chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
            }
            sprintf(apSSID, "VLK%d", chipId);
            // Serial.printf("\r\n\r\n %s\r\n\r\n", apSSID);
            return String(apSSID);
        }

        void doc_so_cuoi_ID_MAY()
        {
            String IDmay = loadChipID();
            String Socuoi;
            int start = sizeof(IDmay)-4;
            for(int i=start; i<=sizeof(IDmay); i++) Socuoi += IDmay[i];
            VHITEK::socuoiID = Socuoi;
            // Serial.println(VHITEK::socuoiID);
        }      

        /*
        void VD_them_the() //vi du them the
        {
            uint16_t tong_so_tu = 20;
            
            user_setting user_TEST_1;
            user_setting user_TEST_2;
            user_setting user_TEST_3;
            user_setting user_TEST_4;

            user_TEST_1 = {1, {'1','0','0','0','0','1','2','3','4','5','6','7','6'}, {'1','0','0','0','8','6','6','8','4','5'}, 567};
            user_TEST_2 = {2, {'2','0','0','0','1','6','3','4','4','9','2','3','4'}, {'2','0','0','0','1','2','6','4','7','6'}, 2};
            user_TEST_3 = {3, {'3','0','0','0','8','2','2','4','8','6','2','4','7'}, {'3','0','0','0','3','2','3','8','5','9'}, 567};
            user_TEST_4 = {4, {'4','0','0','0','1','2','4','7','5','1','3','2','3'}, {'4','0','0','0','1','1','5','4','6','6'}, 4};

            myMem.put(1000, tong_so_tu); //luu so the dang duoc luu

            myMem.put(1002, user_TEST_1); //luu the 1
            myMem.put(1031, user_TEST_2); //luu the 2
            myMem.put(1060, user_TEST_3); //luu the 3
            myMem.put(1089, user_TEST_4); //luu the 4
        }
        */

        void xem_tung_o()
        {
            for(int i=1000; i<=1120; i++)
            {
                Serial.printf("%d: %d, ", i, myMem.read(i)-'0');
                // Serial.print(i); Serial.print(": "); Serial.print(myMem.read(i)-'0'); Serial.print(", ");
            }
            Serial.println();
        }

        void xem_eeprom_tu_bat_ky(int so_tu){ //XEM EEPROM 1 tu bat ky
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
        }

        void cai_dat_mat_khau()
        {
            // uint32_t ID_VD = 12345678;
            // char ID_mass[13];
            // char doc_ID_MASS[13];
            int y=0;

            // tagStr(ID_VD,ID_mass);

            // All_Clear_eeprom(1,50); //XOA toan bo EEPROM

            accessI2C1Bus([&]
                    { myMem.get(0, Vpass); },
            100);

            // for(int i=30; i<= 42; i++)
            // {
            //     accessI2C1Bus([&]
            //         { myMem.put(i, ID_mass[y]); },
            //     100);
            //     y++;
            // }

            // y=0;

            // for(int i=30; i<= 42; i++)
            // {
            //     accessI2C1Bus([&]
            //         { myMem.get(i, ID_Master[y]); },
            //     100);
            //     y++;
            // }

            // Serial.print("ID MASTER EEPROM: ");
            // for(int i=0; i<=12; i++)
            // {
            //     Serial.print(ID_Master[i]);
            // }
            // Serial.printf("\nMKKT: %d - MKKT: %d - MKMASS: %d\n", Vpass.KH, Vpass.KT, Vpass.MASS);

            if(Vpass.KH!=MK_khach or Vpass.KT!=MK_KT)
            {
                Vpass.KH=MK_khach;
                Vpass.KT=MK_KT;
                // Vpass.MASS=MK_MASS;

                accessI2C1Bus([&]
                    { myMem.put(0, Vpass); },
                100);

                accessI2C1Bus([&]
                    { myMem.get(0, Vpass); },
                100);
                // Serial.printf("\nNEW | MKKT: %d - MKKT: %d - MKMASS: %d\n", Vpass.KH, Vpass.KT, Vpass.MASS);
            } //else Serial.printf("\nMKKT: %d - MKKT: %d - MKMASS: %d\n", Vpass.KH, Vpass.KT, Vpass.MASS);
        }

        int so_sanh_mk_menu(char *mk)
        {
            uint32_t MK = atoi(mk);

            if (MK == Vpass.KH) return 1;
            else if (MK == Vpass.KT) return 2;
            // else if (MK == Vpass.MASS) return 3; 
            else return 0;
        }

        //Setup mat khau cho khach hang va KT; Luu vao EEPROM
        /*void cai_dat_mat_khau()
        {
            // VHITEK::Config::xoa_toan_eeprom(1, 100); //XOA toan bo EEPROM
            static uint8_t kiem_tra_co[10];
            static int vitri=0;
            static int demKH; static int demKT;

            // Serial.print("MK KH ee: ");
            for(int i=0; i<So_luong_mk_KH; i++) //MK khach hang 
            {
                accessI2C1Bus([&]{
                        myMem.get(i, kiem_tra_co[i]);       
                }, 100);   
            }
            for(int i=0; i<So_luong_mk_KH; i++)
            {
                // Serial.print(kiem_tra_co[i]);
                if(kiem_tra_co[i] != (MK_khachHang[i]-'0'))
                {
                    demKH++;
                    accessI2C1Bus([&]{
                            myMem.put(i, MK_khachHang[i]-'0');
                    }, 100);                    
                }
            }
            
            // Serial.print(" - MK KT ee: ");
            for(int i=11; i<=So_luong_mk_KT+10; i++)
            {
                accessI2C1Bus([&]{
                        myMem.get(i, kiem_tra_co[vitri]);       
                }, 100);
                // Serial.print(kiem_tra_co[vitri]);
                vitri ++;
            }
            vitri=0;
            // Serial.print(" - ");
            for(int i=11; i<=So_luong_mk_KT+10; i++)
            {
                if(kiem_tra_co[vitri] != (MK_KyThuat[vitri]-'0'))
                {
                    demKT++;
                    // Serial.print(MK_KyThuat[i]-'0');
                    accessI2C1Bus([&]{
                            myMem.put(i, MK_KyThuat[vitri]-'0');
                    }, 100); 
                } 
                vitri++;
            }

            // Serial.printf(" - DemKH: %d - DemKT: %d",demKH, demKT);
            
            if(demKH >= So_luong_mk_KH && demKT >= So_luong_mk_KT)
            {
                accessI2C1Bus([&]{ //Ghi tong tu vao o 1000
                        myMem.put(1000, 100);
                }, 100);                
            }
        }

        int so_sanh_mk_menu(char *mk) //thuc hien so sanh mk nhap vao voi mk luu trong eeprom
        {
            int KT_KH=0;
            int KT_KT=0;
            int vitri_doc=0;

            // Serial.print("MK KH: ");
            for(int i=0; i<So_luong_mk_KH; i++)
            {
                accessI2C1Bus([&]{
                    myMem.get(i, doc_mk_KH[i]);
                }, 100); 
                // Serial.print(doc_mk_KH[i]);
                if((mk[i]-'0') == doc_mk_KH[i])
                {
                    KT_KH++;
                    // Serial.println(KT_KH);
                }                
                // Serial.print(mk[i]);
            } 
            // Serial.print(" - MK KT: ");
            for(int i=11; i<=So_luong_mk_KT+10; i++) //KT
            {
                accessI2C1Bus([&]{
                    myMem.get(i, doc_mk_KT[vitri_doc]);
                }, 100); 
                // Serial.print(doc_mk_KT[vitri_doc]);
                if((mk[vitri_doc]-'0') == doc_mk_KT[vitri_doc])
                {
                    KT_KT++;
                    // Serial.println(KT_KT);
                }
                vitri_doc++;                   
            } 

            // for(int i=0; i<=20; i++)
            // {
            //     Serial.printf("%d: %d, ", i, myMem.read(i));
            // }

            delay(10);
            if(KT_KH>=So_luong_mk_KH) return 1;
            if(KT_KT>=So_luong_mk_KT) return 2;
            if(KT_KH<So_luong_mk_KH && KT_KT<So_luong_mk_KT) return 0;
        } */                     

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

        void doc_tong_so_tu() //Doc tong so tu
        {       
            accessI2C1Bus([&]{
                myMem.get(1000, Tong_so_tu);       
            }, 100);

            if(Tong_so_tu>500 or Tong_so_tu<0)
            {
                Tong_so_tu = 24;
                accessI2C1Bus([&]{
                    myMem.put(1000, Tong_so_tu);       
                }, 100);
            }

            Serial.printf("Tong so tu: %d \n", Tong_so_tu);
        }

        int Wifi_RSSI() //RSSI cua Wifi
        {
            if(2*(WiFi.RSSI()+100) >= 100) return 100;
            else return(2*(WiFi.RSSI()+100)); 
        }

        uint16_t KT_tong_tu_chua_SD() //Tinh tong so tu chua su dung
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
        }

        void Read_ID_master() //doc the master
        {
            int y=0;
            for(int i=30; i<= 42; i++)
            {
                accessI2C1Bus([&]
                    { myMem.get(i, ID_Master[y]); },
                100);
                y++;
            }
            Serial.printf("%s\n", ID_Master);
        }

        void InNhiet(String data) // Config::InNhiet("123456789123");
        {
            printer.setFont('A');
            printer.justify('C');
            printer.setSize('S');
            printer.println("--------------------------------");

            printer.setCodePage(CODEPAGE_WCP1258);
            printer.setFont('A');
            printer.justify('C');
            printer.setSize('L');
            printer.println("SO TU: 23");
            printer.justify('L');

            char text[20];
            sprintf(text, "%s", data);
            printer.printBarcode(text, CODE128);
            
            printer.setFont('A');
            printer.justify('C');
            printer.setSize('S');
            printer.print("--------------------------------");
            printer.feed(3);
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