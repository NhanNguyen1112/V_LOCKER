#include "vhitek.h"

namespace VHITEK
{
    namespace ACTION
    {
        uint16_t addIO, sotuIO;
        bool trang_thai_cua = 0; // 0 la dong, 1 la mo duoc

        // Hàm check thẻ đúng/sai
        //Đưa vào số tủ và user_check
        // Nếu đúng thì trả về TRUE, Sai thì trả về FALSE
        bool kiem_tra_dung_tu(uint16_t so_tu, cabine_config user_check)
        {
            cabine_config doc_id;
            bool checkMK=true;
            String ID_EEPROM;
            String idread;
            String masterID = String(save_config_machine.ID_mas);

            doc_id = VHITEK::EEPROM::read_EEP_1_Cabine(so_tu);
            // VHITEK::Config::xem_eeprom_tu_bat_ky(so_tu); //Xem eeprom khi da luu
            // VHITEK::Config::xem_tung_o();

            ID_EEPROM = String(doc_id.ID_user);
            idread = String(user_check.ID_user);

            if(idread.equals(ID_EEPROM) or idread.equals(masterID))
            {
                return true;
            } 
            else
            {
                for(int i=0; i<10; i++) //so sánh MK
                {
                    if(user_check.mat_khau[i]!=doc_id.mat_khau[i])
                    {
                        checkMK = false;
                        break;
                    }
                }
                if(checkMK==true) return true;
                else return false;
            } 
        }

        int TIM_Tu_CSD() //Tìm xem tủ nào chưa sử dụng -> mở tủ
        {
            // Serial.printf("Tu CSD= ");
            if(Tong_tu_chua_SD > 0)
            {
                for(int i=0; i<=save_config_machine.tongtu; i++)
                {
                    if(tuchuasd[i]>0)
                    {
                        // Serial.printf(" %d ", tuchuasd[i]);
                        //Mở cửa -> Nếu mở được thì break ra / Không mở được thì mở tủ tiếp theo
                        return tuchuasd[i];
                        break;
                    }
                }
            }
            else return 0;     
        }

#ifdef mocua
        // Hàm viết cho việc kiểm tra và mở cửa cuốn của Công Ty
        void mo_cua_cuon()
        {
            cabine_config user_check;
            char ID_doc[15];
            char tagid[15];
            // static int sotu=1;

            tagStr(ID,tagid);
            sprintf(ID_doc,"%s",tagid);

            //Neu co nhap the vao
            if(ID > 0)
            {
                // Serial.printf("ID_doc: %s\n", ID_doc);

                for(int i=0; i<=14; i++)
                {
                    user_check.ID_user[i]=ID_doc[i];
                }
                for(int j=1; j< 50; j++) //Kiểm tra tu the 1 đến the 20
                {
                    if(kiem_tra_dung_tu(j, user_check) == true) //Neu dung the
                    {
                        // Serial.println("Đúng!  MỞ CỬA");
                        digitalWrite(PIN_PUMP, HIGH);
                        digitalWrite(PIN_VAL1, LOW);
                        delay(500);
                        ID = 0;
                        break;
                    }
                    else {
                        // Serial.println("SAI!  KHÔNG MỞ ĐƯỢC");
                    }

                    ID = 0;
                }
            }
            else
            {
                digitalWrite(PIN_PUMP, LOW);
                digitalWrite(PIN_VAL1, HIGH);
                digitalWrite(PIN_VAL2, HIGH);
                ID = 0;
                VHITEK::Menu::end(); //reset lai password
                VHITEK::Display::hien_ngay_gio();
            }
        }
#endif

#ifdef Locker_NoiBo
        bool Doi_pass(cabine_config user_check) //Kiểm tra & ĐỔI MK
        {
            cabine_config doc_id;
            doc_id = VHITEK::EEPROM::read_EEP_1_Cabine(user_check.sotu);
            if (VHITEK::EEPROM::check_read_eeprom_1_Cabine == true) // Neu doc duoc du lieu tu EEPROM
            {
                doc_id.send_data = 1;
                for(int i=0; i<10; i++)
                {
                    doc_id.mat_khau[i] = user_check.mat_khau[i];
                }

                // return true;
                if(VHITEK::EEPROM::write_EEP_1_Cabine(doc_id)) return true;
                else return false;

                /*Serial.printf("Sotu: %d - ", doc_id.so_tu);
                Serial.print("ID: ");
                for(int i=0; i<=12; i++)
                {
                    Serial.print(doc_id.ID_user[i]-'0');
                }
                Serial.print(" - MK: ");
                for(int i=0; i<10; i++)
                {
                    Serial.print(doc_id.mat_khau[i]-'0');
                }
                Serial.printf(" - Send: %d", doc_id.send_data_check);
                Serial.println();*/
            }
            else //KHONG DOC DUOC EEPROM
            {
                return false;
            }
        }

        // Khách chọn số tủ -> cho nhập MK hoặc thẻ -> mở tủ
        void Locker_NB_RFID()  // Locker nội bội dùng RFID
        {
            char key = VHITEK::Keypad::getKey();
            static cabine_config user_check;
            static uint16_t so_tu_vua_nhap;

            static uint32_t LastTick = millis();

            static char keydata_tu[3];
            static int keyCount_tu = 0;

            static char keydata_mk[10];
            static int keyCount_mk = 0;

            static bool check_mk=0;
            static char mk_moi_1[10];
            static char mk_moi_2[10];

            static int step = 0;
            static int check_pos = 0;

            static char ID_doc[15];
            static char tagid[15];

            auto clearID = [](){
                for (int i = 0; i < 15; i++)
                {
                    user_check.ID_user[i] = 0;
                    ID_doc[i] = 0;
                    tagid[i] = 0;
                }
            };
            auto clearMK = [](){
                for (int i = 0; i < 10; i++)
                {
                    user_check.mat_khau[i] = 0;
                    mk_moi_1[i]=0; 
                    mk_moi_2[i]=0;
                }
            };
            auto exit = [&](){
                VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                clearID();
                clearMK();
                keyCount_tu = 0;
                keyCount_mk = 0;
                check_pos = 0; check_mk=0;
                step = 0;
            };

            if (key)
            {
                keydata_tu[keyCount_tu] = key;
                keyCount_tu++;
                LastTick = millis();
                step = 1;
            }
            else // Man hinh chinh: hien thi Ngay, Gio; Check Update Firmware 30s 1 lan
            {
                ID = 0;
                if (VHITEK::FOTA::check_update_FOTA == true)
                    VHITEK::Display::TB_update_FOTA();
                else
                    VHITEK::Display::hien_ngay_gio();
            }

            while (step == 1)
            {
                key = VHITEK::Keypad::getKey();
                if (check_pos == 0)
                {
                    VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                    clearID();
                    clearMK();
                    keyCount_mk = 0;
                    check_pos = 1;
                }
                else if (check_pos == 1) // Nhap vao so tu
                {
                    // so_tu_vua_nhap = VHITEK::Display::nhap_so_tu(keydata_tu);

                    u8g2.clearBuffer();
                    u8g2.drawXBM(0, 0, 128, 64, moinhapsotu_bits);
                    u8g2.setFont(u8g2_font_resoledbold_tr);
                    u8g2.setCursor(73, 60);
                    u8g2.printf("(Enter)->");
                    u8g2.setFont(u8g2_font_timB24_tf);
                    u8g2.setCursor(40, 43);
                    u8g2.printf("%c%c%c",
                                (keydata_tu[0]) ? keydata_tu[0] : '_',
                                (keydata_tu[1]) ? keydata_tu[1] : '_',
                                (keydata_tu[2]) ? keydata_tu[2] : '_');
                    u8g2.sendBuffer();
                    so_tu_vua_nhap = atoi(keydata_tu);
                    if(so_tu_vua_nhap != 0)
                    {
                        user_check.sotu = so_tu_vua_nhap;
                        tagStr(ID, tagid);
                        sprintf(ID_doc, "%s", tagid);
                        sprintf(user_check.ID_user, "%s", ID_doc);
                        if(ID != 0) check_pos = 5;
                    }
                    if(keydata_tu[0]=='*' && keydata_tu[1]=='1' && keydata_tu[2]=='2') //Qua menu doi MK
                    {
                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        keyCount_tu = 0;
                        check_mk=1;
                        check_pos = 2;
                    }
                    // Serial.printf("%c %c %c \n", keydata_tu[0], keydata_tu[1], keydata_tu[2]);
                }
                else if(check_pos == 2) //Doi MK
                {
                    if(check_mk == 0)
                    {
                        check_pos=3;
                    }  
                    u8g2.clearBuffer();
                    u8g2.drawXBM(0, 0, 128, 64, dmkmoinhapsotu_bits);
                    u8g2.setFont(u8g2_font_profont10_mf);
                    u8g2.setCursor(90, 61);
                    u8g2.printf("Enter->");
                    u8g2.setFont(u8g2_font_timB24_tf);
                    u8g2.setCursor(40, 49);
                    u8g2.printf("%c%c%c",
                                (keydata_tu[0]) ? keydata_tu[0] : '_',
                                (keydata_tu[1]) ? keydata_tu[1] : '_',
                                (keydata_tu[2]) ? keydata_tu[2] : '_');
                    u8g2.sendBuffer();
                    so_tu_vua_nhap = atoi(keydata_tu);
                }
                else if (check_pos == 3) // Kiem tra so tu thoa dieu kien khong
                {
                    if (so_tu_vua_nhap == 0) // nếu số vừa nhập = 0 -> thông báo -> về màn hình
                    {
                        VHITEK::Display::TB_tu_lon_hon_0(); // Thong bao

                        exit();
                        
                        delay(3000);
                        break;
                    }
                    else if (VHITEK::Config::kiem_tra_tu_da_co(so_tu_vua_nhap) == false) // Nếu số tủ vừa nhập chưa được sử dụng
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, tuchuasudung_bits);
                        u8g2.sendBuffer();
                        
                        exit();

                        delay(3000);
                        break;
                    }
                    else // Nếu tủ đã được lưu -> qua cho nhập mật khẩu / thẻ
                    {
                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        keyCount_tu = 0;
                        user_check.sotu = so_tu_vua_nhap;
                        check_pos = 4;
                    }
                }
                else if (check_pos == 4) // Nhap mk/the
                {
                    static int pos = 0;
                    if (pos == 0)
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, moinhapthematkhau_bits);
                        u8g2.setFont(u8g2_font_resoledbold_tr);
                        u8g2.setCursor(30, 50);
                        u8g2.printf("%c %c %c %c %c %c",
                                    (keydata_mk[0]) ? '*' : '_',
                                    (keydata_mk[1]) ? '*' : '_',
                                    (keydata_mk[2]) ? '*' : '_',
                                    (keydata_mk[3]) ? '*' : '_',
                                    (keydata_mk[4]) ? '*' : '_',
                                    (keydata_mk[5]) ? '*' : '_');
                        u8g2.sendBuffer();

                        tagStr(ID, tagid);
                        sprintf(ID_doc, "%s", tagid);

                        if (ID > 0 or keyCount_mk >= 6)
                            pos = 1;
                    }
                    else
                    {
                        sprintf(user_check.ID_user, "%s", ID_doc);
                        sprintf(user_check.mat_khau, "%s", keydata_mk);
                        // Serial.printf("ID read: %s\n", user_check.ID_user);
                        ID = 0;
                        pos = 0;
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        keyCount_mk = 0;
                        check_pos = 5;
                    }
                }
                else if(check_pos == 5 && check_mk==1) //NHAP MK MỚI lần 1
                {
                    static int pos = 0;
                    if(kiem_tra_dung_tu(user_check.sotu, user_check))
                    {
                        if(pos == 0)
                        {
                            u8g2.clearBuffer();
                            u8g2.drawXBM(0, 0, 128, 64, nhapmatkhaumoi_bits);
                            u8g2.setCursor(73, 60);
                            u8g2.printf("(Enter)->");
                            u8g2.setCursor(30, 40);
                            u8g2.printf("%c %c %c %c %c %c",
                                        (keydata_mk[0]) ? '*' : '_',
                                        (keydata_mk[1]) ? '*' : '_',
                                        (keydata_mk[2]) ? '*' : '_',
                                        (keydata_mk[3]) ? '*' : '_',
                                        (keydata_mk[4]) ? '*' : '_',
                                        (keydata_mk[5]) ? '*' : '_');
                            u8g2.sendBuffer();
                            if (keyCount_mk >= 6) pos = 1;
                        }
                        else 
                        {
                            sprintf(mk_moi_1, "%s", keydata_mk);
                            VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                            pos = 0;
                            keyCount_mk = 0;
                            check_pos = 6;
                        }
                    }
                    else  //Sai THE hoac MK
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, sai_thehoacmatkhau_bits);
                        u8g2.sendBuffer();

                        exit();

                        delay(2000);
                        break;                        
                    }
                }
                else if(check_pos == 6) //Nhap lai MK moi
                {
                    static int pos = 0;
                    if(pos == 0)
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, nhaplaimatkhau_bits);
                        u8g2.setCursor(73, 60);
                        u8g2.printf("(Enter)->");
                        u8g2.setCursor(30, 40);
                        u8g2.printf("%c %c %c %c %c %c",
                                    (keydata_mk[0]) ? '*' : '_',
                                    (keydata_mk[1]) ? '*' : '_',
                                    (keydata_mk[2]) ? '*' : '_',
                                    (keydata_mk[3]) ? '*' : '_',
                                    (keydata_mk[4]) ? '*' : '_',
                                    (keydata_mk[5]) ? '*' : '_');
                        u8g2.sendBuffer();
                        if (keyCount_mk >= 6) pos = 1;
                    }
                    else 
                    {
                        sprintf(mk_moi_2, "%s", keydata_mk);
                        pos = 0;
                        check_pos = 7;
                    }
                }
                else if(check_pos == 7)
                {
                    static int pos = 0;
                    if(pos == 0) //Kiem tra 2 lan nhap MK moi co khop khong
                    {
                        for(int i=0; i<=5; i++)
                        {
                            if(mk_moi_1[i] == mk_moi_2[i]) pos = 1;
                            else pos = 2; //SAI
                        }
                    }
                    else if(pos == 1) //NEU KHOP
                    {
                        sprintf(user_check.mat_khau, "%s", mk_moi_2);
                        pos = 0;
                        check_pos = 8;
                    }
                    else if(pos == 2) //NEU KHONG KHOP
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, matkhauvuanhapkhongkhop_bits);
                        u8g2.sendBuffer();

                        exit();

                        delay(2000);
                        break;                        
                    }
                }
                else if(check_pos==8) //Lưu MK mới
                {
                    if(Doi_pass(user_check)) //Đổi MK thành công
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, matkhaudaduocthaydoi_bits);
                        u8g2.sendBuffer();

                        exit();

                        delay(3000);
                        break;
                    }
                    else  //Không đổi được MK 
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, loikhongluuduocdulieu_bits);                      
                        u8g2.sendBuffer();

                        exit();

                        delay(3000);
                        break;                        
                    }
                }

                // Kiem tra MO TU
                // Neu so tu nhap vao, the hoac MK dung thi mo tu
                else if (check_pos == 5 && check_mk==0) // Kiem tra MK va The de MO CUA
                {
                    if (kiem_tra_dung_tu(user_check.sotu, user_check) == true) // Neu dung -> MO CUA
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, tudamo_bits);
                        u8g2.sendBuffer();

                        VHITEK::transaction::save_trans(user_check.sotu, 1);
                        // VHITEK::transaction::luu_hanh_dong(so_tu_vua_nhap, trang_thai_cua); // 0 la ĐÓNG, 1 la MỞ

                        check_update_machine = true;

                        exit();
                        
                        delay(2000);
                        break;
                    }
                    else // Neu SAI
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, sai_thehoacmatkhau_bits);
                        u8g2.sendBuffer();

                        exit();

                        delay(2000);
                        break;
                    }
                }

                if(check_pos != 0)
                {
                    if((uint32_t)(millis() - LastTick) > 30000)
                    {
                        exit();
                        break;                        
                    }
                }

                if (key) // Neu co nhan nut
                {
                    LastTick = millis();
                    if (key == '*')
                    {
                        check_pos++;                      
                    }
                    else if (key == '#') //Nhan cancel
                    {
                        exit();
                        break;
                    }

                    if (check_pos==1 or check_pos==2) // Tại trang nhập số tủ
                    {
                        if (keyCount_tu < 3)
                        {
                            keydata_tu[keyCount_tu] = key;
                            keyCount_tu++;
                        }
                        else
                        {
                            VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                            keyCount_tu = 0;
                        }
                    }
                    else if (check_pos==4 or ((check_pos==5 or check_pos==6) && check_mk==1)) // Tại trang nhập MK
                    {
                        if (keyCount_mk < 6)
                        {
                            keydata_mk[keyCount_mk] = key;
                            keyCount_mk++;
                        }
                        else
                        {
                            VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                            keyCount_mk = 0;
                        }
                    }
                    else if(check_pos == 5 && check_mk==0) //Tai trang KT MK va The de mo cua
                    {
                        exit();
                        break;                        
                    }
                }
            }
        }
#endif

/////////////////////////////////// LOCKER SHIPPER SỬ DỤNG QR + MÁY IN NHIỆT + BILL /////////////////////////////////////////////////////////////////////////////////////////////
#ifdef Locker_Ship_Barcode

        cabine_config sender; cabine_config rece;
        cabine_transac new_trans;
        int step=0;
        int start_funct=0;
        uint32_t tick_timeOUT=0;
        int CheckSend_Data=0;
        
        char keydata[6];
        uint8_t keyCount = 0;

        String QRVNpay;
        int32_t amount;
        int32_t ran;
        int status = -1;
        int32_t tienthoi = 0;
        int32_t soTienDaTraLai = 0;

        void VNP_NewOrder(int &stateVNP) //Tạo giao dịch VNPAY
        {
            DynamicJsonDocument doc(10000);
            if((WiFi.status() == WL_CONNECTED)) //Giao dịch VNPAY
            {
                char url[1024];
                HTTPClient http;
                sprintf(url, "http://%s:8000/payments/vnpay/newOrders", API);
                http.begin(url); //API
                http.addHeader("Content-Type", "application/json");              
                String json_data=VHITEK::Config::Json_vnpay_neworder(IDX_hien_tai, new_trans.money, ran);
                // Serial.println(VHITEK::Config::Json_vnpay_neworder(IDX_hien_tai, new_trans.money, ran).c_str());

                int post = http.POST(json_data.c_str());
                String payload = http.getString();
                // Serial.print("Tạo VNP: "); Serial.println(payload);

                if (post == 200)  //Check for the returning code
                { 
                    DeserializationError error = deserializeJson(doc, payload);
                    if (error == 0)
                    {
                        if(doc["qrData"].as<String>()) //NEU da gui duoc
                        {
                            // Serial.println(doc["qrData"].as<String>());
                            QRVNpay = doc["qrData"].as<String>();
                            stateVNP = 1;        
                        }
                    }
                }
                http.end();
            }
            stateVNP = 1; 
        }
        void VNP_Check() //Kiểm tra giao dịch VNPAY
        {
            DynamicJsonDocument doc(10000);
            if((WiFi.status() == WL_CONNECTED)) //Giao dịch VNPAY
            {
                char url[1024];
                HTTPClient http;
                sprintf(url, "http://%s:8000/payments/vnpay/checkTransaction", API);
                http.begin(url); //API
                http.addHeader("Content-Type", "application/json");              
                String json_data=VHITEK::Config::Json_vnpay_check(IDX_hien_tai, ran);
                // Serial.println(VHITEK::Config::Json_vnpay_check(IDX_hien_tai, ran).c_str());

                int post = http.POST(json_data.c_str());
                String payload = http.getString();
                // Serial.print("Check VNP: "); Serial.println(payload);

                if (post == 200)  //Check for the returning code
                { 
                    DeserializationError error = deserializeJson(doc, payload);
                    if (error == 0)
                    {
                        if(doc["amount"].as<int>() != -1) //NEU thanh toan thanh cong
                        {
                            // Serial.println("Thanh toán bằng VNPAY thành công");
                            amount = doc["amount"].as<int>();  
                            new_trans.typepayment = 1;
                            step=4;
                        }
                        // else Serial.println(doc["amount"].as<int>());
                    }
                }
                http.end();  
            }
        }

        void Send_His(cabine_transac data, uint32_t diachi)
        {
            if ((WiFi.status() == WL_CONNECTED))  //GUI lich su hanh dong mo cua len Server
            {
                if(data.send_data == 1) //chưa gửi
                {
                    DynamicJsonDocument doc(10000);
                    char url[1024];
                    HTTPClient http;
                    sprintf(url, "http://%s:8000/shipperlocker/sethistorytransactionlockershipper", API);
                    http.begin(url); //Specify the URL
                    http.addHeader("Content-Type", "application/json");              
                    String json_data=VHITEK::Config::Json_His_Shipping(data);
                    // Serial.println(VHITEK::Config::Json_His_Shipping(data).c_str());

                    int post = http.POST(json_data.c_str());
                    String payload = http.getString();
                    // Serial.print("Send Trans: "); Serial.println(payload);          

                    if (post == 200)  //Check for the returning code
                    { 
                        DeserializationError error = deserializeJson(doc, payload);
                        if (error == 0)
                        {
                            // Serial.println(doc["status"].as<String>());
                            if(doc["status"].as<boolean>() == true) //NEU da gui duoc
                            {
                                data.send_data = 0;

                                if(VHITEK::EEPROM::write_eeprom_2(data, diachi)==true)
                                {
                                    Serial.println("Da gui duoc transaction");
                                    // VHITEK::Config::xem_eep_TranSac(diachi_giaodich);
                                }             
                            }
                        }
                    }
                    http.end();
                }
            }
        }

        /*void Server_info_shipper(cabine_config sender) //gửi thông tin Sender
        {
            DynamicJsonDocument doc(10000);
            if((WiFi.status() == WL_CONNECTED))
            {
                char url[1024];
                HTTPClient http;
                sprintf(url, "http://%s:8000/shipperlocker/saveinforlockershipper", API);
                http.begin(url); //info shipper
                http.addHeader("Content-Type", "application/json");              
                String json_data=VHITEK::Config::Json_info_shipper(sender);
                Serial.println(VHITEK::Config::Json_info_shipper(sender).c_str());

                int post = http.POST(json_data.c_str());
                String payload = http.getString();
                Serial.println(payload); 

                if (post == 200)  //Check for the returning code
                { 
                    DeserializationError error = deserializeJson(doc, payload);
                    if (error == 0)
                    {
                        if(doc["status"].as<boolean>() == true) //NEU da gui duoc
                        {
                            if(doc["status"].as<String>() == "success")
                            {
                                Serial.printf("Đã gửi được thông tin Sender\n");
                            }
                            else Serial.printf("KHÔNG GỬI được thông tin Sender\n");
                        }
                    }
                }
                http.end();
            }
        } */

        void clear_cabine(uint16_t sotu) //xóa tủ khi nhận hàng xong
        {
            cabine_config clear;
            memset(&clear, 0, sizeof(cabine_config));

            uint16_t o_luu= VHITEK::Config::tinh_o_luu_the(sotu);

            if(VHITEK::Config::kiem_tra_tu_da_co(sotu) == 1)
            {
                accessI2C1Bus([&]{
                    EEPROM::myMem.put(o_luu, clear);   
                }, 100);
                VHITEK::Config::KT_tong_tu_chua_SD();
            }
        }

        void back()
        {
            auto clearBILL = [&](){
                VHITEK::BILL::soTienDaNhan = 0;
                tienthoi=0; 
                soTienDaTraLai = 0;
            };

            clearBILL();
            step=0; 
            start_funct=0;
            CheckSend_Data = 0;
        }
        void GuiHang()
        {
            char key = VHITEK::Keypad::getKey();

            if(step==0) //Tìm tủ chưa sử dụng
            {                               
                if(CheckSend_Data == 0) //Kiểm tra máy in còn giấy không
                {
                    VHITEK::Config::Send_Printer(save_config_machine.Sub_boar.Add_Module_Printer, sender.sotu, "CHECK");
                    Display::dang_tao_GD();
                    delay(3000);
                }
                else if(CheckSend_Data == 2) //Hết giấy
                {
                    u8g2.clearBuffer();
                    u8g2.drawFrame(0, 0, 128, 64);
                    u8g2.setFont(u8g2_font_resoledbold_tr);
                    u8g2.setCursor(30, 30);
                    u8g2.printf("MAY IN NHIET");
                    u8g2.setCursor(40, 45);
                    u8g2.printf("HET GIAY");
                    u8g2.sendBuffer();
                    delay(3000);
                    back();
                }
                else //Còn giấy
                {
                    // Serial.printf("Máy in còn giấy\n");
                    // Serial.printf("\n %d \n", TIM_Tu_CSD());

                    if(TIM_Tu_CSD()==0)
                    {
                        Display::Khong_con_tu_trong();
                        delay(3000);
                        back();
                    }
                    else
                    {
                        sender.sotu = TIM_Tu_CSD();
                        String code = Config::xuatbarcode(sender.sotu);
                        for(int i=0; i<15; i++) sender.barcode[i] = code[i];
                        VHITEK::Config::Send_Printer(save_config_machine.Sub_boar.Add_Module_Printer, sender.sotu, code);
                        CheckSend_Data = 0;
                        step=1;
                    } 
                }
            }
            else if(step==1) //in Barcode
            {
                if(CheckSend_Data == 1)
                {
                    step=2;
                }
                else if(CheckSend_Data == 2)
                {
                    u8g2.clearBuffer();
                    u8g2.drawFrame(0, 0, 128, 64);
                    u8g2.setFont(u8g2_font_resoledbold_tr);
                    u8g2.setCursor(40, 35);
                    u8g2.printf("LOI...!!!");
                    u8g2.sendBuffer();
                    delay(3000);
                    back();
                }                     
            }
            else if(step==2) //Mở cửa
            {
                Display::mo_cua(sender.sotu);
                delay(3000);
                Display::de_hang_vao();
                delay(3000);
                step=3;
            }
            else if(step==3) //Lưu thông tin Sender
            {
                sender.send_data = 1;
                sender.time_sender = thoi_gian;

                // for(int i=0; i<15; i++) Serial.print(sender.barcode[i]-'0');

                if(VHITEK::EEPROM::write_EEP_1_Cabine(sender)==true)
                {
                    VHITEK::Config::KT_tong_tu_chua_SD();
                    Serial.printf("\nLưu Shipper thành công\n");
                    back();
                }
            }
        
            if(key)
            {
                if (key == '#') //nhấn Cancel
                {
                    back();
                }
            }
        }
        void NhanHang()
        {
            char key = VHITEK::Keypad::getKey();
            
            if(step==0)
            {
                rece.sotu = VHITEK::Config::xuat_sotu(QRread.data);
                if(VHITEK::Config::kiem_tra_tu_da_co(rece.sotu) == true)
                {
                    rece = VHITEK::EEPROM::read_EEP_1_Cabine(rece.sotu);
                    if(String(rece.barcode) == QRread.data)
                    {
                        // Serial.println("Đúng Barcode");
                        new_trans.IDX = IDX_hien_tai;
                        new_trans.so_tu = rece.sotu;
                        new_trans.time_sender = rece.time_sender;
                        new_trans.time_receive = thoi_gian;
                        new_trans.send_data = 1;
                        new_trans.money = VHITEK::BILL::tinh_tien(new_trans.time_sender, new_trans.time_receive);
                        ran = random(-999999, 999999);
                        step=1;
                    }
                    else back();                                    
                } else back();
            }
            else if(step==1) //Gửi thông tin tạo thanh toán VNP
            {
                static int stateVNP=0;
                if(stateVNP==0) VNP_NewOrder(stateVNP);
                else if(stateVNP==1) step=2;
            }
            else if(step==2) //Kiểm tra thanh toán bằng VNP hay tiền mặt
            {              
                VNP_Check(); //Kiểm tra thanh toán bằng VNPAY

                VHITEK::Display::Trang_thanh_toan(QRVNpay, new_trans); //Trang thanh toán

                #ifdef CoSanTien
                VHITEK::BILL::soTienDaNhan = new_trans.money;
                delay(5000);
                #endif
                
                if (VHITEK::BILL::soTienDaNhan >= new_trans.money) 
                {
                    new_trans.typepayment = 0;
                    tienthoi = VHITEK::BILL::soTienDaNhan - new_trans.money;
                    // Serial.printf("Tien Thoi: %d\n", tienthoi);
                    delay(1000);
                    step=3;
                }
                else 
                {
                    if (VHITEK::BILL::soTienChuaNhan)
                    {
                        accessMDBBus([&]
                        { Validator.BV->acceptBill(); },
                        2000);                          
                    }
                }
            }
            else if(step==3) //thối tiền
            {
                if(tienthoi>0)
                {
                    accessMDBBus([&]
                            {         
                                VHITEK::Display::TB_thoi_tien();

                                // Serial.printf("Dang thoi tien\n");
                                status = Validator.BV->payoutValue(tienthoi, &soTienDaTraLai);
                                // Serial.printf("soTienDaTraLai: %d - Status: %d\n", soTienDaTraLai, status);

                                if (status == BV_CMD_SUCCESS)
                                {
                                    // Serial.printf("Da thoi XONG\n");
                                    // soTienDaNhan = tienthoi-soTienDaTraLai;
                                    // Serial.printf("So tien da nhan: %ld - So tien tra lai: %ld\n", soTienDaNhan, soTienDaTraLai);
                                    // soTienDaTraLai=0;
                                    tienthoi=0;
                                    delay(1000);
                                    step=4;
                                }
                                else{
                                    VHITEK::BILL::soTienDaNhan = tienthoi;
                                    delay(1000);
                                    step=4;
                                }
                            },
                    2000);
                }
                else 
                {
                    tienthoi=0;
                    // soTienDaNhan = 0;
                    step=4;
                }
            }
            else if(step==4) //Mở cửa
            {               
                Display::mo_cua(new_trans.so_tu);
                delay(3000);
                Display::nhan_hang_dong_cua();
                delay(3000);

                if(VHITEK::transaction::save_trans(new_trans.so_tu, 1) == true)
                {
                  Send_His(new_trans, Last_DC_GD);
                  clear_cabine(new_trans.so_tu);
                  back();
                } 
            }

            if(key)
            {
                if (key == '#') //nhấn Cancel
                {   
                    while(VHITEK::BILL::soTienDaNhan>0)
                    {
                        VHITEK::Display::TB_thoi_tien();
                        accessMDBBus([&]
                        {         
                            // Serial.printf("Dang thoi tien\n");
                            status = Validator.BV->payoutValue(VHITEK::BILL::soTienDaNhan, &soTienDaTraLai);
                            // Serial.printf("soTienDaTraLai: %d - Status: %d\n", soTienDaTraLai, status);

                            if (status == BV_CMD_SUCCESS)
                            {
                                // Serial.printf("Da thoi XONG\n");
                                VHITEK::BILL::soTienDaNhan = VHITEK::BILL::soTienDaNhan-soTienDaTraLai;
                                // Serial.printf("So tien da nhan: %ld - So tien tra lai: %d - Tienthoi: %d\n", soTienDaNhan, soTienDaTraLai, tienthoi);
                                soTienDaTraLai=0;
                                tienthoi=0;
                                delay(2000);
                                back();                                               
                            }
                        },
                        2000);
                    }
                    back();
                }
            }
        }
        void MoTu()
        {
            char key = VHITEK::Keypad::getKey();
            static uint32_t MK;
            static uint16_t sotu;

            auto back = [](){
                VHITEK::Keypad::clearKeypad(keydata, 6);
                keyCount = 0;step=0;MK=0;start_funct=0;
            };

            if(step == 0) //Nhập Pass
            {
                u8g2.clearBuffer();
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.enableUTF8Print();
                u8g2.setFontDirection(0);
                u8g2.setFont(u8g2_font_resoledbold_tr);            
                u8g2.setCursor(8,20);
                u8g2.printf("NHAP PASSWORD MO TU");
                #ifdef logoVHITEK
                u8g2.setCursor(35, 60);
                u8g2.printf(logoVHITEK);  
                #else
                u8g2.setCursor(32, 60);    
                u8g2.printf(logoTSE); 
                #endif          

                u8g2.setCursor(30,40);
                u8g2.printf("%c %c %c %c %c %c",
                            (keydata[0]) ? '*' : '_',
                            (keydata[1]) ? '*' : '_',
                            (keydata[2]) ? '*' : '_',
                            (keydata[3]) ? '*' : '_',
                            (keydata[4]) ? '*' : '_',
                            (keydata[5]) ? '*' : '_');
                u8g2.sendBuffer();
                MK = atoi(keydata);

                if(keyCount>=6)
                {
                    if(MK == save_config_machine.PW_Control)
                    {
                        VHITEK::Keypad::clearKeypad(keydata, 6);
                        keyCount = 0; 
                        step=1;
                    } 
                    else 
                    {
                        u8g2.clearBuffer();
                        u8g2.setCursor(25,35);
                        u8g2.printf("SAI PASSWORD!");
                        u8g2.sendBuffer();   
                        delay(2000);  
                        back();
                    }
                }
            }
            else if(step==1)
            {
                u8g2.clearBuffer();
                u8g2.setCursor(35,10);
                u8g2.printf("NHAP SO TU");   
                u8g2.setCursor(45,20);
                u8g2.printf("CAN MO");                               
                u8g2.setCursor(5,45);
                u8g2.printf("TU SO: %c%c%c",  
                            (keydata[0]) ? keydata[0] : '_',
                            (keydata[1]) ? keydata[1] : '_',
                            (keydata[2]) ? keydata[2] : '_');                 
                u8g2.setCursor(45,60);  
                u8g2.printf("(ENTER) DE MO");                                                                
                u8g2.sendBuffer(); 

                sotu = atoi(keydata); 
            }
            else if(step==2)
            {
                // VHITEK::Config::Open_IO(sotu);
                u8g2.clearBuffer();
                u8g2.drawXBM(0, 0, 128, 64, DaMoTu_bits);
                u8g2.setFont(u8g2_font_timB24_tf);
                u8g2.setCursor(45, 50);
                u8g2.printf("%02d", sotu);
                u8g2.sendBuffer();
                delay(3000);
                back();
            }

            if(key)
            {
                if(key == '#') back();
                if(key == '*') step++;

                if(step==1)
                {
                    if (keyCount < 3)
                    {
                        keydata[keyCount] = key;
                        keyCount++;
                    }   
                    else {
                        VHITEK::Keypad::clearKeypad(keydata, 6);
                        keyCount = 0;
                    } 
                }
                else 
                {
                    if (keyCount < 6)
                    {
                        keydata[keyCount] = key;
                        keyCount++;
                    }   
                    else {
                        VHITEK::Keypad::clearKeypad(keydata, 6);
                        keyCount = 0;
                    } 
                }
            }
        }

        void Locker_Ship_BARCODE()
        {           
            char key = VHITEK::Keypad::getKey();
            auto clear = [](){
                VHITEK::Keypad::clearKeypad(keydata, 6);
                keyCount = 0;
            };

            if(digitalRead(PIN_ENC1)==0 && start_funct==0) //Nhấn nút gửi
            {
                if(start_funct==0) start_funct=1;
            }
            else // Man hinh chinh: hien thi Ngay, Gio; Check Update Firmware 30s 1 lan
            {
                if (VHITEK::FOTA::check_update_FOTA == true) VHITEK::Display::TB_update_FOTA();
                else
                {
                    if(start_funct == 0)
                    {
                        tick_timeOUT = millis();
                        VHITEK::Display::hien_ngay_gio();
                        if(keydata[0]=='*' && keydata[1]=='1' && keydata[2]=='2')
                        {
                            start_funct = 3;
                            clear();
                        } 
                        if(key)
                        {
                            if (keyCount < 3)
                            {
                                keydata[keyCount] = key;
                                keyCount++;
                            }   
                            else clear();
                        }
                    } 
                    else 
                    {                       
                        while(start_funct==1) //gửi hàng
                        {
                            if((uint32_t)(millis() - tick_timeOUT) >= 60000*5) back(); //sau 5 phút
                            GuiHang();
                        }
                        
                        while(start_funct==2) //Nhận hàng
                        {   
                            if((uint32_t)(millis() - tick_timeOUT) >= 60000*5) back(); //sau 5 phút
                            NhanHang();
                        }

                        while(start_funct==3) //Nhập MK để mở tủ
                        {
                            if((uint32_t)(millis() - tick_timeOUT) >= 60000*5) back(); //sau 5 phút
                            MoTu();
                        }
                    }
                } 
            }
        }

#endif //

    }
}