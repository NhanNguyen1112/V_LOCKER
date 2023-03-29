#include "vhitek.h"

namespace VHITEK
{
    namespace Config
    {
        uint16_t addIO, sotuIO;

        DynamicJsonDocument doc(10000);

        void decodeID(uint16_t sotu_INPUT, uint16_t &id, uint16_t &add);

        void Send_Printer(uint16_t add, uint16_t sotu, String data)
        {
            doc.clear();
            
            doc["add"] = add;
            doc["cmd"] = "printer";
            doc["id"] = sotu;
            doc["barcode"] = data;

            Serial2.write(0x7F);
            serializeJson(doc, Serial2);
            Serial2.write(0x7E);
        }
        void Send_Music(uint16_t add, uint16_t sobaihat, uint16_t timeout)
        {
            doc.clear();
            
            doc["add"] = add;
            doc["cmd"] = "music";
            doc["id"] = sobaihat;
            doc["timeout"] = timeout;

            Serial2.write(0x7F);
            serializeJson(doc, Serial2);
            Serial2.write(0x7E);
        }

        void Json_Open_Locker(uint16_t add, uint16_t sotu)
        {
            doc.clear();
            
            doc["add"] = add;
            doc["cmd"] = "open";
            doc["id"] = sotu;

            Serial2.write(0x7F);
            serializeJson(doc, Serial2);
            Serial2.write(0x7E);
        }
        void Json_Read_Locker(uint16_t add, uint16_t sotu)
        {
            doc.clear();
            
            doc["add"] = add;
            doc["cmd"] = "read";
            doc["id"] = sotu;

            Serial2.write(0x7F);
            serializeJson(doc, Serial2);
            Serial2.write(0x7E);
        }
        String Json_info_shipper(cabine_config data) // Tao Json luu info shipper
        {
            DynamicJsonDocument doc(10000);
            char rData[2048];
            char Time_send[25];

            sprintf(Time_send, "%04d-%02d-%02dT%02d:%02d:00.671Z",
                    data.time_sender.nam, data.time_sender.thang, data.time_sender.ngay, data.time_sender.gio, data.time_sender.phut);

            doc["lockerid"] = data.sotu;
            doc["time"] = Time_send;
            doc["size"] = 0;
            doc["phonesender"] = 0;
            doc["phonereceiver"] = 0;
            doc["lockermachineid"] = apSSID;

            serializeJson(doc, rData);
            return String(rData);
        }

        bool read_locker(uint16_t sotu) //Đọc trạng thái tủ
        {
            decodeID(sotu, addIO, sotuIO);
            Json_Read_Locker(addIO, sotuIO);
            flag_status = 1;
            if(flag_status == 2)
            {
                flag_status=0;
                return ReadIO.status;
            }
        }
        bool openlocker(uint16_t sotu) //Mở tủ
        {
            decodeID(sotu, addIO, sotuIO);
            Json_Open_Locker(addIO, sotuIO);
            return read_locker(sotu);
        }

        // Hàm viết cho hành động mở tủ
        // Đưa vào số tủ: 0->Mở tất cả các tủ; các số khác mở theo số tủ
        bool Open_IO(uint16_t so_tu)
        {
            if (so_tu == 0) // Mo tat ca cac tu
            {
                u8g2.clearBuffer();
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.setCursor(15, 35);
                u8g2.printf("MO TAT CA CAC TU");
                u8g2.sendBuffer();

                for (int i = 1; i <= save_config_machine.tongtu; i++)
                {
                    VHITEK::Config::openlocker(i);
                    delay(10);
                }
                return 1;
            }
            else // Mo tu theo so tu
            {
                u8g2.clearBuffer();

                if (VHITEK::Config::openlocker(so_tu) == 1) // MỞ được
                {
                    u8g2.drawXBM(0, 0, 128, 64, tudamo_bits);
                    u8g2.sendBuffer();
                    return 1;
                }
                else // KHÔNG mở được -> LỖI
                {
                    u8g2.drawXBM(0, 0, 128, 64, khongmoduoccua_bits);
                    u8g2.sendBuffer();
                    return 0;
                }
            }
        }

        String Json_his(cabine_transac data) //lich su giao dich
        {
            DynamicJsonDocument doc(10000);
            thoigian time;
            char rData[2048];
            char dateBuf[25];

            time = thoi_gian;

            sprintf(dateBuf, "%04d-%02d-%02dT%02d:%02d:00.546715",
                    time.nam+2000, time.thang, time.ngay, time.gio, time.phut);

            doc["IDX"] = data.IDX;
            doc["Status"] = data.status_cabine;
            doc["lockerid"] = data.so_tu;
            doc["DataCheck"] = data.send_data;
            doc["IdCard"] = data.RFID;
            doc["Time"] = dateBuf;
            doc["MID"] = apSSID;
            serializeJson(doc, rData);
            return String(rData);
        }

        String Json_tong_tu() //Tao Json tong so tu
        {
            DynamicJsonDocument doc(10000);
            char rData[2048];

            doc["totalcabined"] = save_config_machine.tongtu;
            doc["MID"] = apSSID;

            serializeJson(doc, rData);
            return String(rData);            
        }

        /*String Json_thong_tin_tu(user_setting user) //Tao Json thong tin tu
        {
            DynamicJsonDocument doc(10000);
            char rData[2048];

            doc["lockerid"] = user.so_tu;
            doc["cardid"] = user.ID_user;
            doc["password"] = user.mat_khau;
            doc["lockermachineid"] = apSSID;

            serializeJson(doc, rData);
            return String(rData);             
        }*/

        String Json_machine_status() //Tao Json tinh trang may
        {
            doc.clear();

            char dateBuf[25];
            char rData[2048];
            sprintf(dateBuf, "%04d-%02d-%02dT%02d:%02d:%02d.265Z",
                    thoi_gian.nam, thoi_gian.thang, thoi_gian.ngay,
                    thoi_gian.gio, thoi_gian.phut, thoi_gian.giay);

            doc["time"] = dateBuf;
            doc["wifi"] = Wifi_RSSI();
            doc["wifiname"] = WiFi.SSID();
            doc["gsm"] = 0;
            doc["error"] = 0;
            doc["emptycabined"] = Tong_tu_chua_SD;
            doc["lockermachineid"] = apSSID;
            doc["version"] = FW_VERSION;

            int total_elm=0;
            for (int i=1;i<=save_config_machine.tongtu;i++)
            {
                doc["statuscabined"][total_elm++]=1;
                // if(VHITEK::Config::read_locker(i)==1) //đang mở
                // {
                //     doc["statuscabined"][total_elm++]=i;
                // }
            }

            serializeJson(doc, rData);
            return String(rData);  
        }

        String Json_vnpay_neworder(uint32_t IDX, int32_t tongtien, int32_t random) //Tạo giao dich VNPAY moi 
        {
            DynamicJsonDocument doc(10000);
            char rData[2048];

            doc["mTransactionId"] = IDX;
            doc["transactionId"] = random;
            doc["amount"] = tongtien;
            doc["col"] = 0;
            doc["mID"] = apSSID;

            serializeJson(doc, rData);
            return String(rData);
        }
        String Json_vnpay_check(uint32_t IDX, int32_t random) //Thực hiện thanh toán
        {
            DynamicJsonDocument doc(10000);
            char rData[2048];

            doc["mTransactionId"] = IDX;
            doc["transactionId"] = random;
            doc["mID"] = apSSID;

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
            #if defined(Locker_Shipping) || defined(Locker_Ship_Barcode)
            sprintf(apSSID, "VLS%d", chipId);
            #else
            sprintf(apSSID, "VLK%d", chipId);
            #endif
            
            IDmay = String(apSSID);
            int start = sizeof(IDmay)-4;
            for(int i=start; i<=sizeof(IDmay); i++) Socuoi += IDmay[i];
            VHITEK::socuoiID = Socuoi;
            // Serial.printf("\r\n\r\n %s\r\n\r\n", apSSID);
            // Serial.printf("\n%s\n",VHITEK::socuoiID);
            return String(apSSID);
        }    

        int get_temp() //đọc nhiệt độ ESP32
        {
            return ((temprature_sens_read() - 32) / 1.8);
            // Serial.print("Temperature: ");
            // // Convert raw temperature in F to Celsius degrees
            // Serial.print((temprature_sens_read() - 32) / 1.8);
            // Serial.println(" C");
            // delay(1000);
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
                    EEPROM::myMem.write(i, 0);
                    EEPROM::myMem2.write(i, 0);
                }
            }
            else if (eep == 1) // xoa eeprom 1
            {
                for (uint16_t i = 0; i <= Max_diachi; i++)
                {
                    EEPROM::myMem.write(i, 0);
                }
            }
            else if (eep == 2) // xoa eeprom 2
            {
                for (uint16_t i = 0; i <= Max_diachi; i++)
                {
                    EEPROM::myMem2.write(i, 0);
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
            cabine_config readcabine;
            // readcabine = VHITEK::EEPROM::read_EEP_1_Cabine(so_tu);
            uint16_t add;
            add = VHITEK::Config::tinh_o_luu_the(so_tu);

            accessI2C1Bus([&]{
                    EEPROM::myMem.get(add, readcabine); //Doc Check SUM trong eeprom              
            }, 100);
            
            if(readcabine.sotu == so_tu) return true; //Neu the da duoc luu
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

        void KT_tong_tu_chua_SD() //Tinh tong so tu chua su dung
        {
            cabine_config readcabine;
            uint16_t add;
            
            Tong_tu_chua_SD = 0;
            int vitri=0;

            // Serial.printf("Tu SD: ");
            for(int sotu=1; sotu<=save_config_machine.tongtu; sotu++) //dò từ 1 đến tổng số tủ
            {
                add = VHITEK::Config::tinh_o_luu_the(sotu);

                accessI2C1Bus([&]{
                    EEPROM::myMem.get(add, readcabine); //Doc Check SUM trong eeprom              
                }, 100);

                if(readcabine.sotu==0)
                {
                    tuchuasd[vitri] = sotu;
                    // Serial.printf("%d ", tuchuasd[vitri]);
                    Tong_tu_chua_SD++;
                    vitri++;
                }
            }
            // Serial.printf("Tong tu chua sd: %d \n", Tong_tu_chua_SD);
        }

        void get_setting_machine() //đọc cài đặt máy
        {
            save_config_machine = VHITEK::EEPROM::read_EEP_1_Machine();
        }

        bool Save_Set_Machine() //Lưu setting máy, ô 1000
        {
            if(VHITEK::EEPROM::write_EEP_1_Machine()==true) return true; 
            else return false;
        }

        void decodeID(uint16_t sotu_INPUT, uint16_t &id, uint16_t &add)
        {
            id = (sotu_INPUT / 24)+1;
            add = sotu_INPUT % 24;
        }
    
        int xuat_sotu(String data)
        {
            char buf[5];
            int sotu;
            for(int i=0; i<=2; i++)
            {
                buf[i] = data[i];
                // Serial.println(buf[i]);
            }
            sotu = atoi(buf);
            return sotu;
            Serial.printf("So tu: %d\n", sotu);
        }

        String xuatbarcode(uint16_t sotu)
        {
            char buffer[50];
            sprintf(buffer,"%03d%s%02d%02d%02d%02d",sotu,VHITEK::socuoiID.c_str(),thoi_gian.gio ,thoi_gian.phut ,thoi_gian.ngay ,thoi_gian.thang);
            // Serial.println("bufer :");
            // Serial.println(buffer);
            return String(buffer);
        }

        void QR_VNP(String QR) //Hiện QR VNP
        {
            QRCode qrcode;
            uint8_t LCDType = 1;
            uint8_t qrcodeData[qrcode_getBufferSize(11)];
            char wQR[2000];
            sprintf(wQR, "%s", QR.c_str());
            qrcode_initText(&qrcode, qrcodeData, 9, ECC_MEDIUM, wQR);

            u8g2.clearBuffer();
            if (LCDType)    u8g2.drawBox(0, 0, 64, 64);
            const uint8_t y0 = (64 - qrcode.size) / 2;
            const uint8_t x0 = (64 - qrcode.size) / 2;
            for (uint8_t y = 0; y < qrcode.size; y++)
            {
                for (uint8_t x = 0; x < qrcode.size; x++)
                {
                    if (qrcode_getModule(&qrcode, x, y))
                    {
                        if (LCDType == 0)
                            u8g2.setColorIndex(1);
                        else
                            u8g2.setColorIndex(0);
                    }
                    else
                    {
                        if (LCDType == 0)
                            u8g2.setColorIndex(0);
                        else
                            u8g2.setColorIndex(1);
                    }
                    u8g2.drawPixel(x0 + x, y0 + y);
                }
            }
            u8g2.setColorIndex(1);
        }

        void HT_QR(String QR)
        {
            QRCode qrcode;
            uint8_t LCDType = 1;
            uint8_t qrcodeData[qrcode_getBufferSize(11)];
            char wQR[2000];
            sprintf(wQR, "%s", QR.c_str());
            qrcode_initText(&qrcode, qrcodeData, 9, ECC_MEDIUM, wQR);

            u8g2.clearBuffer();
            if (LCDType)    u8g2.drawBox(0, 0, 64, 64);
            const uint8_t y0 = (64 - qrcode.size) / 2;
            const uint8_t x0 = (64 - qrcode.size) / 2;
            for (uint8_t y = 0; y < qrcode.size; y++)
            {
                for (uint8_t x = 0; x < qrcode.size; x++)
                {
                    if (qrcode_getModule(&qrcode, x, y))
                    {
                        if (LCDType == 0)
                            u8g2.setColorIndex(1);
                        else
                            u8g2.setColorIndex(0);
                    }
                    else
                    {
                        if (LCDType == 0)
                            u8g2.setColorIndex(0);
                        else
                            u8g2.setColorIndex(1);
                    }
                    u8g2.drawPixel(x0 + x, y0 + y);
                }
            }
            u8g2.setColorIndex(1);
        }

        void begin()
        {
            VHITEK::EEPROM::begin();
            Display::setup();
            Keypad::setup();
            RFID::begin();
            Ds1307::begin();
            VHITEK::OTA::WifiBegin();
            VHITEK::FOTA::FOTAbegin();
            loadChipID();

            // VHITEK::Config::All_Clear_eeprom(1, 64000);

            get_setting_machine();
            if(save_config_machine.BILL.payout==0) save_config_machine.BILL.payout = 10000;
            if(save_config_machine.BILL.billmin==0 or save_config_machine.BILL.billmax==0)
            {
                save_config_machine.BILL.billmin = 5000;
                save_config_machine.BILL.billmax = 50000;
            }

            // save_config_machine.Gia_Thue_Tu.block1_minutes = 120;
            // save_config_machine.Gia_Thue_Tu.block1_price = 10000;
            // save_config_machine.Gia_Thue_Tu.block2_minutes = 60;
            // save_config_machine.Gia_Thue_Tu.block2_price = 5000;

            if (save_config_machine.Gia_Thue_Tu.block1_minutes == 0)
            {
                save_config_machine.Gia_Thue_Tu.block1_minutes = 120;
                save_config_machine.Gia_Thue_Tu.block1_price = 10000;
                save_config_machine.Gia_Thue_Tu.block2_minutes = 180;
                save_config_machine.Gia_Thue_Tu.block2_price = 5000;
            }

            if(save_config_machine.tongtu>1000) save_config_machine.tongtu = 24;
            else save_config_machine.Sub_boar.tongboard = save_config_machine.tongtu/24;

            if(Save_Set_Machine()==true) 
            {
                Serial.printf("\nTong tu: %d\n", save_config_machine.tongtu);
            }

            KT_tong_tu_chua_SD();
        }
    }
}