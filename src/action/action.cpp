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
#else 
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

    }
}