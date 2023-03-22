#include "vhitek.h"

namespace VHITEK
{
    namespace mo_tu_locker
    {
        bool trang_thai_cua = 0; // 0 la dong, 1 la mo duoc

        String xuatbarcode(uint16_t sotu)
        {
            char buffer[50];
            sprintf(buffer,"%03d%s%02d%02d%02d%02d",sotu,VHITEK::socuoiID.c_str(),thoi_gian.gio ,thoi_gian.phut ,thoi_gian.ngay ,thoi_gian.thang);
            // Serial.println("bufer :");
            // Serial.println(buffer);
            return String(buffer);
        }

        // Hàm check thẻ đúng/sai
        //Đưa vào số tủ và user_check
        // Nếu đúng thì trả về TRUE, Sai thì trả về FALSE
        /*bool kiem_tra_dung_tu(uint16_t so_tu, user_setting user_check)
        {
            user_setting doc_id;

            String ID_EEPROM;
            String idread;
            String masterID = String(ID_Master);

            uint8_t kt_ID = 0;
            uint8_t kt_MK = 0;

            doc_id = VHITEK::EEPROM::read_eeprom_1(so_tu);
            // VHITEK::Config::xem_eeprom_tu_bat_ky(so_tu); //Xem eeprom khi da luu
            // VHITEK::Config::xem_tung_o();

            if (check_read_eeprom_1 == true) // Neu doc duoc du lieu tu EEPROM
            {
                ID_EEPROM = String(doc_id.ID_user);
                idread = String(user_check.ID_user);

                if(idread.equals(ID_EEPROM) or idread.equals(masterID))
                {
                    so_tu = 0;
                    kt_MK = 0;
                    kt_ID = 0; 
                    return true;
                } 
                else
                {
                    so_tu = 0;
                    kt_MK = 0;
                    kt_ID = 0;
                    return false;
                } 

                // Serial.print(" - MK eeprom: ");
                for (int MK = 0; MK < 10; MK++)
                {
                    // Serial.print(doc_id.mat_khau[MK]-'0');
                    if (doc_id.mat_khau[MK] == user_check.mat_khau[MK]) // Neu mat khau dung
                    {
                        kt_MK++;
                    }
                }

                // Serial.printf("KT_ID: %d\n", kt_ID);
            }
            else // Neu khong doc duoc du lieu tu eeprom
            {
                return false;
            }

            if (kt_MK == 10) // ID hoac MK dung
            {
                // Serial.println("Đúng!");
                so_tu = 0;
                kt_MK = 0;
                kt_ID = 0;
                return true;
            }
            else
            {
                // Serial.println("SAI!");
                so_tu = 0;
                kt_MK = 0;
                kt_ID = 0;
                return false;
            }
        }*/

#ifdef mocua
        // Hàm viết cho việc kiểm tra và mở cửa cuốn của Công Ty
        /*void mo_cua_cuon()
        {
            user_setting user_check;
            char ID_doc[13];
            char tagid[13];
            // static int sotu=1;

            tagStr(ID,tagid);
            sprintf(ID_doc,"%s",tagid);

            //Neu co nhap the vao
            if(ID > 0)
            {
                // Serial.printf("ID_doc: %s\n", ID_doc);

                for(int i=0; i<=12; i++)
                {
                    user_check.ID_user[i]=ID_doc[i];
                }
                for(int j=1; j< 50; j++) //Kiểm tra tu the 1 đến the 20
                {
                    if(kiem_tra_dung_tu(j, user_check) == true) //Neu dung the
                    {
                        // Serial.println("Đúng!");
                        digitalWrite(PIN_PUMP, HIGH);
                        digitalWrite(PIN_VAL1, LOW);
                        delay(500);
                        ID = 0;
                        break;
                    }
                    else {
                        // Serial.println("SAI!");
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
        } */
#else 
        //Sử dụng Board Locker 24
        int read_locker_status(uint16_t locker_id)
		{
			uint8_t data1, data2, data3;
			uint8_t bitmaskIndex[24] = {
				0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
				0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
				0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
			digitalWrite(PIN_165_LATCH, LOW);
			delay(5);
			digitalWrite(PIN_165_LATCH, HIGH);
			digitalWrite(PIN_595_CLOCK, LOW);
			data1 = shiftIn(PIN_ENC1, PIN_595_CLOCK, MSBFIRST);
			data2 = shiftIn(PIN_ENC1, PIN_595_CLOCK, MSBFIRST);
			data3 = shiftIn(PIN_ENC1, PIN_595_CLOCK, MSBFIRST);
			Serial.printf("Read locker ID %d \r\n", locker_id);
			// printBinary(data1);
			// printBinary(data2);
			// printBinary(data3);
			uint8_t byte_index = locker_id / 8;
			switch (byte_index)
			{
			case 0:
				if (data3 & bitmaskIndex[locker_id])
					return 1;
				break;
			case 1:
				if (data2 & bitmaskIndex[locker_id])
					return 1;
				break;
			case 2:
				if (data1 & bitmaskIndex[locker_id])
					return 1;
				break;
			}

			return 0;
		}
        bool locker_open(uint8_t id)
        {
            if (id > 24)
                return false;
            digitalWrite(PIN_595_LATCH, LOW);
            uint32_t data = 0;
            bitWrite(data, id, 1);
            shiftOut(PIN_595_DATA, PIN_595_CLOCK, LSBFIRST, (data >> 16) & 0xff);
            shiftOut(PIN_595_DATA, PIN_595_CLOCK, LSBFIRST, (data >> 8) & 0xff);
            shiftOut(PIN_595_DATA, PIN_595_CLOCK, MSBFIRST, data & 0xff);
            // Serial.printf("open locker ID %d || value %02X \r\n", id, data);
            digitalWrite(PIN_595_LATCH, HIGH);
            delay(200);
            digitalWrite(PIN_595_LATCH, LOW);
            shiftOut(PIN_595_DATA, PIN_595_CLOCK, LSBFIRST, 0);
            shiftOut(PIN_595_DATA, PIN_595_CLOCK, LSBFIRST, 0);
            shiftOut(PIN_595_DATA, PIN_595_CLOCK, MSBFIRST, 0);
            digitalWrite(PIN_595_LATCH, HIGH);
            return read_locker_status(id);
        }

        /*int read_locker_status(uint16_t locker_id)
        {
            uint16_t locker_in_value[9] = {0x00, 0x100, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02};
            uint8_t data1, data2;
            digitalWrite(PIN_165_LATCH, LOW);
            delay(5);
            digitalWrite(PIN_165_LATCH, HIGH);
            digitalWrite(PIN_595_CLOCK, LOW);
            data1 = shiftIn(PIN_ENC1, PIN_595_CLOCK, MSBFIRST);
            data2 = shiftIn(PIN_ENC1, PIN_595_CLOCK, MSBFIRST);
            uint16_t io_val = ((unsigned int)data1 << 8) + data2;
            Serial.printf("%04x %04x\r\n", io_val, io_val & locker_in_value[locker_id]);
            if (io_val & locker_in_value[locker_id])
                return 1;
            return 0;
        }

        void shiftOut16b(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint16_t val)
        {
            uint8_t i;
            for (i = 0; i < 16; i++)
            {
                if (bitOrder == LSBFIRST)
                    digitalWrite(dataPin, !!(val & (1 << i)));
                else
                    digitalWrite(dataPin, !!(val & (1 << (15 - i))));
                digitalWrite(clockPin, HIGH);
                digitalWrite(clockPin, LOW);
            }
        }

        bool locker_open(uint8_t id)
        {
            uint16_t locker_out_value[9] = {0x01, 0x100, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02};
            if (id > 8)
                return false;
            digitalWrite(PIN_595_LATCH, LOW);
            shiftOut16b(PIN_595_DATA, PIN_595_CLOCK, MSBFIRST, locker_out_value[id]);
            Serial.printf("open locker ID %d || value %02X \r\n", id, locker_out_value[id]);
            digitalWrite(PIN_595_LATCH, HIGH);
            delay(150);
            digitalWrite(PIN_595_LATCH, LOW);
            shiftOut16b(PIN_595_DATA, PIN_595_CLOCK, MSBFIRST, 0);
            digitalWrite(PIN_595_LATCH, HIGH);
            delay(50);
            return read_locker_status(id);
        } */

        bool Doi_pass(user_setting user_check) //Kiểm tra & ĐỔI MK
        {
            user_setting doc_id;
            doc_id = VHITEK::EEPROM::read_eeprom_1(user_check.so_tu);
            if (check_read_eeprom_1 == true) // Neu doc duoc du lieu tu EEPROM
            {
                doc_id.send_data_check = 1;
                for(int i=0; i<10; i++)
                {
                    doc_id.mat_khau[i] = user_check.mat_khau[i];
                }

                // return true;
                if(VHITEK::EEPROM::write_eeprom_1(doc_id)) return true;
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

        bool xoatu(int sotu)
        {
            user_setting user_clear;
            memset(&user_clear, 0, sizeof(user_clear));
            uint16_t o_luu;
            o_luu = VHITEK::Menu::tinh_o_luu_the(sotu);

            accessI2C1Bus([&]{
                myMem.put(o_luu, user_clear);   
            }, 100);

            VHITEK::Config::KT_tong_tu_chua_SD();

            return true;
        }

        // Hàm viết cho hành động mở tủ
        //Đưa vào số tủ: 0->Mở tất cả các tủ; các số khác mở theo số tủ
        void hanh_dong_mo_tu(uint16_t so_tu)
        {
            if (so_tu == 0) // Mo tat ca cac tu
            {
                u8g2.clearBuffer();
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.setCursor(15, 35);
                u8g2.printf("MO TAT CA CAC TU");
                u8g2.sendBuffer();

                for (int i = 1; i <= Tong_so_tu; i++)
                {
                    locker_open(i);
                }
            }
            else // Mo tu theo so tu
            {
                // String read_status;
                u8g2.clearBuffer();

                if (locker_open(so_tu) == 1) // MỞ được
                {
                    trang_thai_cua = 1;
                    u8g2.drawXBM(0, 0, 128, 64, tudamo_bits);
                    u8g2.sendBuffer();
                }
                else // KHÔNG mở được -> LỖI
                {
                    trang_thai_cua = 0;
                    u8g2.drawXBM(0, 0, 128, 64, khongmoduoccua_bits);
                    u8g2.sendBuffer();
                }
                // // Serial.print("So tu: "); Serial.print(so_tu);
                // // Serial.print(" - Status: "); Serial.println(read_status[read_status.length()-1]);
            }
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
            // Serial.printf("So tu: %d\n", sotu);/
        }

        //Máy in nhiệt và cảm biến QR
        void locker_Barcode()
        {
            char key = VHITEK::Keypad::getKey();
            static user_setting New_user;
            static int step = 0;
            static int pos=0;
            static int start = 0;
            String barcode;
            char buf[50];
            static int sotu;

            auto exit = []() { // Reset pos music
                step = 0;
                start = 0;
                pos=0;
                ID = 0;
            };

            if(key)
            {
                if(key == '1') //nhan gui hang
                {
                    start = 1;
                }
                else if (key == '#') exit();
            }
            else 
            {
                u8g2.clearBuffer();
                u8g2.drawXBM(0, 0, 128, 64, xinchaomoichontu_bits);                       
                u8g2.sendBuffer();

                while (Serial2.available()>0)
                {
                    VHITEK::bufferGM65 = Serial2.readString();
                    // Serial.println(VHITEK::bufferGM65);
                    sotu = xuat_sotu(VHITEK::bufferGM65);

                    if(VHITEK::Menu::kiem_tra_tu_da_co(sotu) == 1)
                    {
                        if( locker_open(sotu)==1 )
                        {
                            u8g2.clearBuffer();
                            u8g2.drawXBM(0, 0, 128, 64, tudamo_bits);
                            u8g2.sendBuffer();
                            xoatu(sotu);
                            sotu=0;
                            delay(2000);
                        }
                        else 
                        {
                            u8g2.clearBuffer();
                            u8g2.drawXBM(0, 0, 128, 64, khongmoduoccua_bits);
                            u8g2.sendBuffer(); 
                            sotu=0;
                            delay(2000);
                        }
                    }
                    else{
                        VHITEK::Display::TB_tu_chua_su_dung();
                        sotu=0;
                        delay(2000);
                    }
                    
                }
            }

            while(start == 1)
            {
                key = VHITEK::Keypad::getKey();

                if(step==0) //Tìm tủ chưa SD
                {
                    for(int i=0; i<= sizeof(tuchuasd); i++)
                    {
                        if(tuchuasd[i]>0)
                        {
                            New_user.so_tu = tuchuasd[i];
                            barcode = xuatbarcode(New_user.so_tu);
                            // Serial.println(barcode);
                            // Serial.println(barcode.c_str());
                            step=1;
                            break;
                        }
                    }
                }
                else if(step==1) //in bill
                {
                    Serial.println(buf);

                    printer.setFont('A');
                    printer.justify('C');
                    printer.setSize('S');
                    printer.println("--------------------------------");

                    // printer.setCodePage(CODEPAGE_WCP1258);
                    printer.setFont('A');
                    printer.justify('C');
                    printer.setSize('L');
                    printer.printf("SO TU: %02d", New_user.so_tu);
                    printer.justify('L');

                    printer.printBarcode(barcode.c_str(), CODE128);
                    
                    printer.setFont('A');
                    printer.justify('C');
                    printer.setSize('S');
                    printer.print("--------------------------------");
                    printer.feed(3);

                    step=2;
                }
                else if(step==2) //mo cua
                {
                    if(locker_open(New_user.so_tu))
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, tudamo_bits);
                        u8g2.sendBuffer();

                        if(VHITEK::EEPROM::write_eeprom_1(New_user))
                        {
                            tuchuasd[New_user.so_tu-1] = 0;
                            delay(2000);
                            step=3;
                        }
                    }
                    else 
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, khongmoduoccua_bits);
                        u8g2.sendBuffer();      
                        delay(2000);
                        step=3;                        
                    }
                }
                else if(step==3)
                {
                    exit();
                    break;
                }

                if (key) // Neu co nhan nut
                {
                    if(key == '#')
                    {
                        exit();
                        break;
                    }
                }
            }
        }

        //Nút vào menu là nút gưi hàng
        void locker_button()
        {
            char key = VHITEK::Keypad::getKey();
            static user_setting New_user;
            static user_setting user_check;
            static int step = 0;
            static int pos=0;
            static int start = 0;
            static char ID_doc[13];
            static char tagid[13];

            auto exit = []() { // Reset pos music
                step = 0;
                start = 0;
                pos=0;
                ID = 0;
            };

            if(key)
            {
                if(key == '1') //nhan gui hang
                {
                    start = 1;
                }
                else if (key == '#') exit();
            }
            else 
            {          
                u8g2.clearBuffer();
                u8g2.drawXBM(0, 0, 128, 64, xinchaomoichontu_bits);                       
                u8g2.sendBuffer();  

                if(step == 0)
                {
                    if(ID>0)
                    {
                        tagStr(ID, tagid);
                        sprintf(ID_doc, "%s", tagid);
                        step=1;
                    }
                }
                else if(step==1)
                {
                    user_check.mat_khau[0] = 1;
                    user_check.mat_khau[1] = 2;
                    user_check.mat_khau[2] = 3;
                    user_check.mat_khau[3] = 4;
                    user_check.mat_khau[4] = 5;
                    user_check.mat_khau[5] = 6;

                    for (int i = 0; i < 13; i++)
                    {
                        user_check.ID_user[i] = ID_doc[i];
                        // Serial.print(user_check.ID_user[i]-'0');
                    }
                    step=2;
                }
                else if(step==2)
                {
                    for(int i=1; i<=24; i++)
                    {
                        if(kiem_tra_dung_tu(i,user_check)==true) //đung tu
                        {
                            if(locker_open(i))
                            {
                                u8g2.clearBuffer();
                                u8g2.drawXBM(0, 0, 128, 64, tudamo_bits);
                                u8g2.sendBuffer();
                                xoatu(i);
                            }
                            else 
                            {
                                u8g2.clearBuffer();
                                u8g2.drawXBM(0, 0, 128, 64, khongmoduoccua_bits);
                                u8g2.sendBuffer();                              
                            }
                            delay(2000);
                            step=3;
                            break;
                        }
                    }
                    exit();
                }
                else if(step==3)
                {
                    exit();
                }
            }

            while(start == 1)
            {
                key = VHITEK::Keypad::getKey();
                if(step==0)
                {
                    u8g2.clearBuffer();
                    u8g2.setFont(u8g2_font_ncenB10_tr);
                    u8g2.setCursor(30, 25);
                    u8g2.printf("VUI LONG");
                    u8g2.setCursor(20, 45);
                    u8g2.printf("DE THE VAO");                                    
                    u8g2.sendBuffer();  

                    if(pos==0)
                    {                       
                        if (ID > 0)
                        {
                            tagStr(ID, tagid);
                            sprintf(ID_doc, "%s", tagid);
                            pos = 1;
                        } 
                    }
                    else 
                    {
                        for (int i = 0; i < 13; i++)
                        {
                            New_user.ID_user[i] = ID_doc[i];
                            // Serial.print(New_user.ID_user[i]-'0');
                        }
                        ID = 0;
                        pos = 0;
                        step=1;
                    }
                }
                else if(step==1)
                {
                    for(int i=0; i<= sizeof(tuchuasd); i++)
                    {
                        if(tuchuasd[i]>0)
                        {
                            New_user.so_tu = tuchuasd[i];
                            if(locker_open(New_user.so_tu)==1)//cua mo
                            {
                                u8g2.clearBuffer();
                                u8g2.drawXBM(0, 0, 128, 64, tudamo_bits);
                                u8g2.sendBuffer();

                                if(VHITEK::EEPROM::write_eeprom_1(New_user))
                                {
                                    tuchuasd[i] = 0;
                                    delay(2000);
                                    step=2;
                                    break;
                                }
                            }
                            else //khong mo duoc
                            {
                                u8g2.clearBuffer();
                                u8g2.drawXBM(0, 0, 128, 64, khongmoduoccua_bits);
                                u8g2.sendBuffer();
                                delay(2000);
                                step=2;
                            }
                        }
                    }
                }
                else if(step==2)
                {
                    exit();
                    break;
                }

                if (key) // Neu co nhan nut
                {
                    if(key == '#')
                    {
                        exit();
                        break;
                    }
                }
            }
        }

        // Hàm viết cho khách hàng mở tủ
        // Khách chọn số tủ -> cho nhập MK hoặc thẻ -> mở tủ
        void mo_tu_locker()
        {
            char key = VHITEK::Keypad::getKey();
            static user_setting user_check;
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

            static char ID_doc[13];
            static char tagid[13];

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
                if (check_update_FOTA == true)
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
                    VHITEK::Keypad::clearKeypad(ID_doc, 13);
                    VHITEK::Keypad::clearKeypad(tagid, 13);
                    for (int i = 0; i < 13; i++)
                    {
                        user_check.ID_user[i] = 0;
                        user_check.mat_khau[i] = 0;
                        mk_moi_1[i]=0; mk_moi_2[i]=0;
                    }
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
                        user_check.so_tu = so_tu_vua_nhap;
                        tagStr(ID, tagid);
                        sprintf(ID_doc, "%s", tagid);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = ID_doc[i];
                            // Serial.print(user_check.ID_user[i]-'0');
                        }
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

                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                        }
                        keyCount_tu = 0;
                        keyCount_mk = 0;
                        check_pos = 0; check_mk=0;
                        step = 0;
                        delay(3000);
                        break;
                    }
                    else if (VHITEK::Menu::kiem_tra_tu_da_co(so_tu_vua_nhap) == false) // Nếu số tủ vừa nhập chưa được sử dụng
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, tuchuasudung_bits);
                        u8g2.sendBuffer();
                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                        }
                        keyCount_tu = 0;
                        keyCount_mk = 0;
                        check_pos = 0; check_mk=0;
                        step = 0;
                        delay(3000);
                        break;
                    }
                    else // Nếu tủ đã được lưu -> qua cho nhập mật khẩu / thẻ
                    {
                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        keyCount_tu = 0;
                        user_check.so_tu = so_tu_vua_nhap;
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
                        user_check.mat_khau[0] = keydata_mk[0];
                        user_check.mat_khau[1] = keydata_mk[1];
                        user_check.mat_khau[2] = keydata_mk[2];
                        user_check.mat_khau[3] = keydata_mk[3];
                        user_check.mat_khau[4] = keydata_mk[4];
                        user_check.mat_khau[5] = keydata_mk[5];

                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = ID_doc[i];
                            // Serial.print(user_check.ID_user[i]-'0');
                        }
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
                    if(kiem_tra_dung_tu(user_check.so_tu, user_check))
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
                            mk_moi_1[0] = keydata_mk[0];
                            mk_moi_1[1] = keydata_mk[1];
                            mk_moi_1[2] = keydata_mk[2];
                            mk_moi_1[3] = keydata_mk[3];
                            mk_moi_1[4] = keydata_mk[4];
                            mk_moi_1[5] = keydata_mk[5];
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

                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                        }
                        ID = 0;
                        keyCount_tu = 0;
                        keyCount_mk = 0;
                        check_pos = 0; check_mk=0;
                        step = 0;
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
                        mk_moi_2[0] = keydata_mk[0];
                        mk_moi_2[1] = keydata_mk[1];
                        mk_moi_2[2] = keydata_mk[2];
                        mk_moi_2[3] = keydata_mk[3];
                        mk_moi_2[4] = keydata_mk[4];
                        mk_moi_2[5] = keydata_mk[5];
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
                        user_check.mat_khau[0] = mk_moi_2[0];
                        user_check.mat_khau[1] = mk_moi_2[1];
                        user_check.mat_khau[2] = mk_moi_2[2];
                        user_check.mat_khau[3] = mk_moi_2[3];
                        user_check.mat_khau[4] = mk_moi_2[4];
                        user_check.mat_khau[5] = mk_moi_2[5];
                        pos = 0;
                        check_pos = 8;
                    }
                    else if(pos == 2) //NEU KHONG KHOP
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, matkhauvuanhapkhongkhop_bits);
                        u8g2.sendBuffer();

                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                        }
                        ID = 0;
                        pos = 0;
                        keyCount_tu = 0;
                        keyCount_mk = 0;
                        check_pos = 0; check_mk=0;
                        step = 0;
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

                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                            mk_moi_1[i]=0; mk_moi_2[i]=0;
                        }
                        ID = 0;
                        keyCount_tu = 0;
                        keyCount_mk = 0;
                        check_pos = 0; check_mk=0;
                        step = 0;
                        delay(3000);
                        break;
                    }
                    else  //Không đổi được MK 
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, loikhongluuduocdulieu_bits);                      
                        u8g2.sendBuffer();

                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                            mk_moi_1[i]=0; mk_moi_2[i]=0;
                        }
                        ID = 0;
                        keyCount_tu = 0;
                        keyCount_mk = 0;
                        check_pos = 0; check_mk=0;
                        step = 0;
                        delay(3000);
                        break;                        
                    }
                }

                // Kiem tra MO TU
                // Neu so tu nhap vao, the hoac MK dung thi mo tu
                else if (check_pos == 5 && check_mk==0) // Kiem tra MK va The de MO CUA
                {
                    if (kiem_tra_dung_tu(user_check.so_tu, user_check) == true) // Neu dung -> MO CUA
                    {
                        hanh_dong_mo_tu(so_tu_vua_nhap); 
                        VHITEK::transaction::luu_hanh_dong(so_tu_vua_nhap, trang_thai_cua); // 0 la ĐÓNG, 1 la MỞ

                        check_update_machine = true;
                        check_his_send = true;

                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                        }
                        ID = 0;
                        keyCount_tu = 0;
                        keyCount_mk = 0;
                        check_pos = 0; check_mk=0;
                        step = 0;
                        delay(2000);
                        break;
                    }
                    else // Neu SAI
                    {
                        u8g2.clearBuffer();
                        u8g2.drawXBM(0, 0, 128, 64, sai_thehoacmatkhau_bits);
                        u8g2.sendBuffer();

                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                        }
                        ID = 0;
                        keyCount_tu = 0;
                        keyCount_mk = 0;
                        check_pos = 0; check_mk=0;
                        step = 0;
                        delay(2000);
                        break;
                    }
                }

                if(check_pos != 0)
                {
                    if((uint32_t)(millis() - LastTick) > 30000)
                    {
                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                        }
                        keyCount_tu = 0;
                        keyCount_mk = 0;

                        check_pos = 0;
                        step = 0;
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
                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                            mk_moi_1[i]=0; mk_moi_2[i]=0;
                        }
                        keyCount_tu = 0;
                        keyCount_mk = 0;
                        check_mk=0;
                        check_pos = 0;
                        step = 0;
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
                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata_mk, 10);
                        VHITEK::Keypad::clearKeypad(ID_doc, 13);
                        VHITEK::Keypad::clearKeypad(tagid, 13);
                        for (int i = 0; i < 13; i++)
                        {
                            user_check.ID_user[i] = 0;
                            user_check.mat_khau[i] = 0;
                        }
                        keyCount_tu = 0;
                        keyCount_mk = 0;

                        ID = 0;
                        check_pos = 0;
                        step = 0;
                        break;                        
                    }
                }
            }
        }

#endif

    }
}