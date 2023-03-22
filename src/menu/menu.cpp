#include "vhitek.h"

namespace VHITEK
{
    namespace Menu
    {
        typedef std::function<void()> MenuFuncTion;
        typedef struct
        {
            MenuFuncTion fn;
            char menuName[25];
            char userLevel;
            char menuIdx;
            bool enabled;
        } MenuType;        
        MenuType menuELM[so_luong_menu];
        MenuType mainMenuELM[so_luong_menu];
        MenuFuncTion subMenuFunc;
        int MenuID = 0, totalMenuPage, menuPage = 0;
        bool reInit = true;
        int userLevel = 0;        

        user_setting user_ID;

        void gotoMenu(int menuId)
        {
            if (menuId)
            {
                for (int i = 0; i < so_luong_menu; i++)
                {
                    if (menuELM[i].menuIdx == menuId)
                    {
                        subMenuFunc = menuELM[i].fn;
                        break;
                    }
                }
            }
            MenuID = menuId;
            reInit = true;
            VHITEK::Keypad::clearKeypad();
        }
        void add_menu(const char *name, uint8_t idx, uint8_t level, MenuFuncTion fn)
        {
            for (int i = 0; i < so_luong_menu; i++) 
            {
                if (menuELM[i].menuIdx == idx)
                {
                    // Serial.println("Trung");
                    return;
                }
            }
            for (int i = 0; i < so_luong_menu; i++)
            {
                if (menuELM[i].menuIdx == 0) // kiem tra id co chua
                {
                    sprintf(menuELM[i].menuName, "%s", name);
                    // Serial.printf("%s", name);
                    menuELM[i].menuIdx = idx;
                    menuELM[i].userLevel = level;
                    menuELM[i].fn = fn;
                    // Serial.println("da luu");
                    return;
                }
            }
        }

        uint16_t tinh_o_luu_the(int so_tu) //tinh o bat dau luu the theo so tu
        {
            return 1002 + ((so_tu*sizeof(user_setting)) - sizeof(user_setting));
        }
        bool kiem_tra_tu_da_co(int so_tu) //Kiem tra xem so tu nay da luu chua
        {
            static uint16_t doc_so_tu;
            accessI2C1Bus([&]{
                    myMem.get(tinh_o_luu_the(so_tu), doc_so_tu);              
            }, 100);                  
            
            if(doc_so_tu == so_tu) return true; //Neu the da duoc luu
            else return false; //neu the chua duoc luu
        }  

        bool nhap_the() //luu so tu va moi nhap the
        {
            static uint8_t step=0;
            static char tagid[13];
          
            if(step == 0)
            {
                u8g2.clearBuffer();    
                u8g2.setCursor(30,30);
                u8g2.printf("MOI NHAP THE");  
                u8g2.setCursor(45,40);
                u8g2.printf("******");    
                u8g2.sendBuffer(); 

                tagStr(ID,tagid); //chuyen int sang string
                if(ID!= 0) step = 1;
            }
            else{
                for(int i=0; i<=12; i++) //Luu ID vao struct user_ID
                {
                    user_ID.ID_user[i]=tagid[i];
                    // Serial.print(tagid[i]);
                    // Serial.print(user_ID.ID_user[i]-'0');
                }
                step=0; ID=0;
                return true;
            } 
        }
        void nhap_mk(char *key)
        {
            u8g2.clearBuffer();    
            u8g2.setCursor(35,10);
            u8g2.printf("DA LUU THE");  
            u8g2.setCursor(15,20);
            u8g2.printf("MOI NHAP MAT KHAU"); 
            u8g2.setCursor(30,40);
            u8g2.printf("%c %c %c %c %c %c",
                        (key[0]) ? key[0] : '_',
                        (key[1]) ? key[1] : '_',
                        (key[2]) ? key[2] : '_',
                        (key[3]) ? key[3] : '_',
                        (key[4]) ? key[4] : '_',
                        (key[5]) ? key[5] : '_');              
            u8g2.setCursor(40,60);  
            u8g2.printf("(ENTER) DE LUU");    
            u8g2.sendBuffer();   

            user_ID.mat_khau[0] = key[0];
            user_ID.mat_khau[1] = key[1];
            user_ID.mat_khau[2] = key[2];
            user_ID.mat_khau[3] = key[3];
            user_ID.mat_khau[4] = key[4];
            user_ID.mat_khau[5] = key[5];
        }
        
        //MENU KHACH HANG
        void menu_them_tu()
        {
            char key = Keypad::getKey();

            static char keydata_tu[3];
            static uint8_t keyCount_tu = 0;

            static char keydata[6];
            static uint8_t keyCount = 0; 

            static uint8_t step=0;
            static uint16_t so_tu_vua_nhap; //so tu khach nhap vao          

            if (reInit == true)
            {
                Keypad::clearKeypad(keydata, 6);
                Keypad::clearKeypad(keydata_tu, 3);
                step = 0;
                keyCount = 0; keyCount_tu = 0;
                reInit = false;
            }            

            if(step == 0){ //nhap so tu va kiem tra
                ID=0;
                so_tu_vua_nhap = VHITEK::Display::nhap_so_tu(keydata_tu);
            } else if (step == 1) //nhap the
            { 
                if(so_tu_vua_nhap == 0) //Nếu số tủ nhập vào = 0 -> thông báo và về menu
                {
                    VHITEK::Display::TB_tu_lon_hon_0();
                    gotoMenu(menuMainId);
                    delay(2000);                    
                }
                else 
                {
                    if(kiem_tra_tu_da_co(so_tu_vua_nhap) == true) //Tu da duoc luu: Hien thong bao -> ve Menu
                    {
                        VHITEK::Display::thong_bao_tu_da_co();
                        gotoMenu(menuMainId);
                        delay(2000);
                    }
                    //kiểm tra số tủ vừa nhập có lơn hơn tổng số tủ đã cài đặt không
                    //Lớn hơn thì thông báo -> về menu
                    // Nằm trong tổng số tủ đã cài thì cho nhập thẻ
                    if(so_tu_vua_nhap > Tong_so_tu)
                    {
                        VHITEK::Display::TB_lon_hon_tong_TU();
                        gotoMenu(menuMainId);
                        delay(2000);
                    }
                    else { //Luu so tu va moi nhap the
                        user_ID.so_tu = so_tu_vua_nhap;
                        // Serial.print("So tu: "); Serial.print(user_ID.so_tu);              
                        if(nhap_the() == true){
                            Keypad::clearKeypad(keydata, 6);
                            Keypad::clearKeypad(keydata_tu, 3);
                            keyCount = 0; keyCount_tu = 0;
                            step = 2;
                        }
                    }
                }
            } else if(step == 2){ //Nhap mat khau   
                nhap_mk(keydata);
            } else if(step == 3){ //luu vao EEPROM va ve MENU    
                if(keyCount==0) //Neu khong nhao gi vao MK
                {
                    u8g2.clearBuffer();
                    u8g2.setCursor(35,28);
                    u8g2.printf("KHONG DUOC!");
                    u8g2.setCursor(15,43);
                    u8g2.printf("BO TRONG MAT KHAU");                    
                    u8g2.sendBuffer();
                    Keypad::clearKeypad(keydata, 6);
                    keyCount = 0;
                    delay(3000);
                    step = 2;
                }   
                else if(keyCount<6) //Neu nhap MK khong du 6 so
                {
                    u8g2.clearBuffer();
                    u8g2.setCursor(40,28);
                    u8g2.printf("MAT KHAU ");
                    u8g2.setCursor(30,43);
                    u8g2.printf("PHAI DU 6 SO");                    
                    u8g2.sendBuffer();
                    Keypad::clearKeypad(keydata, 6);
                    keyCount = 0;
                    delay(3000);
                    step = 2;                    
                }
                else //hien thong bao da luu thanh cong
                { 
                    user_ID.send_data_check = 1;
                    if(VHITEK::EEPROM::write_eeprom_1(user_ID)) //Dung
                    {
                        VHITEK::Config::KT_tong_tu_chua_SD();
                        // check_update[user_ID.so_tu]=1;
                        // check_send_card_info = true;
                        u8g2.clearBuffer();    
                        u8g2.setCursor(35,30);
                        u8g2.printf("DA LUU THE!");  
                        u8g2.sendBuffer();   

                        ID = 0;
                        step = 0;
                        Keypad::clearKeypad(keydata, 6);                
                        // VHITEK::Keypad::clearKeypad();
                        // VHITEK::Config::xem_eeprom_tu_bat_ky(so_tu_vua_nhap); //Xem eeprom khi da luu
                        // VHITEK::Config::xem_tung_o();
                        delay(3000);       //dung 3s -> ve Menu 
                        gotoMenu(menuMainId);  
                    }
                    else //sai 
                    {
                        // check_send_card_info = false;
                        u8g2.clearBuffer();    
                        u8g2.setCursor(30,20);
                        u8g2.printf("LOI DU LIEU!");  
                        u8g2.setCursor(40,35);
                        u8g2.printf("LIEN HE");     
                        u8g2.setCursor(25,45);
                        u8g2.printf("KY THUAT VIEN");                                                     
                        u8g2.sendBuffer(); 

                        // VHITEK::Config::xem_eeprom_tu_bat_ky(so_tu_vua_nhap); //Xem eeprom khi da luu
                        // VHITEK::Config::xem_tung_o();

                        ID=0;
                        reInit = true;
                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata, 6);
                        keyCount = 0; keyCount_tu = 0;
                        delay(3000);
                        gotoMenu(menuMainId);                        
                    }
                }    
            }

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    if(step==0) //tai trang nhap so tu
                    {
                        if(keyCount_tu==0)
                        {
                            gotoMenu(menuMainId);
                            return;
                        }
                        else {
                            keyCount_tu = 0;
                            reInit = true;
                            VHITEK::Keypad::clearKeypad();
                            Keypad::clearKeypad(keydata_tu, 3);
                        }
                    }
                    else
                    {
                        ID=0;
                        reInit = true;
                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        VHITEK::Keypad::clearKeypad(keydata, 6);
                        keyCount = 0; keyCount_tu = 0;
                        gotoMenu(menuMainId);
                        // return;                        
                    }
                } 

                if(key == '*' && step!=1) //nhan enter 
                {
                    step++;
                    // Serial.println("Luu the");
                }

                if(step == 0) //tai trang nhap so tu
                {
                    if (keyCount_tu < 3)
                    {
                        keydata_tu[keyCount_tu] = key;
                        keyCount_tu++;
                    } 
                    else {
                        VHITEK::Keypad::clearKeypad(keydata_tu, 3);
                        keyCount_tu = 0;
                    }                   
                }
                else if(step == 2){ //tai trang nhap MK
                    if (keyCount < 6)
                    {
                        keydata[keyCount] = key;
                        keyCount++;
                    } 
                    else {
                        VHITEK::Keypad::clearKeypad(keydata, 6);
                        keyCount = 0; keyCount_tu = 0;
                    }               
                }
            }
            // Serial.printf("step: %d, userLevel: %d, MenuID: %d, MenuPage: %d, TotalMenuPage: %d \n", step, userLevel, MenuID, menuPage, totalMenuPage);    
        }
        void menu_xoa_tu_bat_ky()
        {
            char key = Keypad::getKey();
            static char keydata[3];
            static uint8_t keyCount = 0; 
            static uint8_t step=0;

            static uint16_t so_tu; //so tu khach nhap vao      

            if (reInit == true)
            {
                Keypad::clearKeypad(keydata, 3);
                step = 0;
                keyCount = 0;
                reInit = false;
            } 

            if(step == 0) //nhap so tu can xoa
            {
                u8g2.clearBuffer();
                u8g2.setCursor(40,20);
                u8g2.printf("MOI NHAP");   
                u8g2.setCursor(25,30);
                u8g2.printf("SO TU CAN XOA");             
                u8g2.setCursor(5,45);
                u8g2.printf("TU SO: %c%c%c",  
                            (keydata[0]) ? keydata[0] : '_',
                            (keydata[1]) ? keydata[1] : '_',
                            (keydata[2]) ? keydata[2] : '_');            
                u8g2.setCursor(40,60);  
                u8g2.printf("(ENTER) DE XOA");                        
                u8g2.sendBuffer();
                so_tu = atoi(keydata);
            }
            else if(step == 1) //thuc hien xoa tu va ve man hinh chinh
            {
                user_setting user_clear;
                memset(&user_clear, 0, sizeof(user_clear));
                user_clear.send_data_check = 2;

                uint16_t o_luu;
                o_luu = tinh_o_luu_the(so_tu);

                if(kiem_tra_tu_da_co(so_tu) == 1){ //Neu co tu thi XOA -> ve Menu
                    // Serial.printf("bat dau: %d - ket thuc: %d",o_luu, o_luu+29);

                    // VHITEK::Config::xem_eeprom_tu_bat_ky(so_tu);
                    accessI2C1Bus([&]{
                        myMem.put(o_luu, user_clear);   
                    }, 100);
                    // VHITEK::Config::xem_eeprom_tu_bat_ky(so_tu); 

                    VHITEK::Config::KT_tong_tu_chua_SD();

                    u8g2.clearBuffer();
                    u8g2.setCursor(30,35);
                    u8g2.printf("DA XOA XONG!");   
                    u8g2.sendBuffer();
                    delay(2000);
                    gotoMenu(menuMainId);
                }
                else{ // KHONG CO -> hien thong bao -> ve Menu
                    VHITEK::Display::TB_tu_chua_su_dung();
                    delay(2000);
                    gotoMenu(menuMainId);
                }
            }

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    gotoMenu(menuMainId);
                    return;
                }
                else if(key == '*') //nhan enter 
                {
                    step++;
                }

                if (keyCount < 3)
                {
                    keydata[keyCount] = key;
                    keyCount++;
                } 
                else {
                    VHITEK::Keypad::clearKeypad(keydata, 3);
                    keyCount = 0;
                }                                
            }                          
        }      
        void menu_xoa_tat_ca_tu() //thuc hien xoa tat ca cac tu: xoa tu ô 1002 -> tong so tu
        {
            char key = Keypad::getKey();
            static char keydata[6];    
            static uint8_t keyCount = 0; 
            static uint8_t step=0;                          

            if (reInit)
            {
                Keypad::clearKeypad(keydata, 6);
                Keypad::clearKeypad();
                keyCount = 0; step=0;
                reInit = false;
            }

            if(step == 0)
            {
                u8g2.clearBuffer();
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.enableUTF8Print();
                u8g2.setFontDirection(0);
                u8g2.setFont(u8g2_font_resoledbold_tr);            
                u8g2.setCursor(15,20);
                u8g2.printf("NHAP LAI PASSWORD");
                u8g2.setCursor(35,60);
                u8g2.printf("VHITEK.VN");          

                u8g2.setCursor(30,40);
                u8g2.printf("%c %c %c %c %c %c",
                            (keydata[0]) ? '*' : '_',
                            (keydata[1]) ? '*' : '_',
                            (keydata[2]) ? '*' : '_',
                            (keydata[3]) ? '*' : '_',
                            (keydata[4]) ? '*' : '_',
                            (keydata[5]) ? '*' : '_');
                u8g2.sendBuffer();

                if(keyCount>=6)
                {
                    if((VHITEK::Config::so_sanh_mk_menu(keydata) == 1) or (VHITEK::Config::so_sanh_mk_menu(keydata) == 2))
                    {
                        step=1;
                    }
                    else 
                    {
                        VHITEK::Keypad::clearKeypad(keydata, 6);
                        reInit = true;
                        step=0;
                        u8g2.clearBuffer();
                        u8g2.setCursor(25,35);
                        u8g2.printf("SAI PASSWORD!");
                        u8g2.sendBuffer();   
                        delay(2000);  
                        gotoMenu(menuMainId);                   
                    }
                }
            }
            else 
            {
                user_setting user_clear;
                memset(&user_clear, 0, sizeof(user_clear));
                user_clear.send_data_check=2;
                // Serial.println(tong_so_tu);
                u8g2.clearBuffer();
                u8g2.setCursor(35,30);
                u8g2.printf("DANG XOA...");            
                u8g2.setCursor(25,45);
                u8g2.printf("TAT CA CAC TU");   
                u8g2.sendBuffer();  

                for(int i=1; i<=Tong_so_tu; i++)
                {
                    accessI2C1Bus([&]{
                        myMem.put(tinh_o_luu_the(i), user_clear);   
                    }, 100);
                    // VHITEK::Config::xem_eeprom_tu_bat_ky(i);                                        
                }      
                // VHITEK::Config::xem_tung_o();
                VHITEK::Config::KT_tong_tu_chua_SD();
                VHITEK::Keypad::clearKeypad(keydata, 6);
                reInit = true;
                step=0;
                delay(2000);
                gotoMenu(menuMainId);  
            }

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    step=0;keyCount = 0;
                    VHITEK::Keypad::clearKeypad(keydata, 6);
                    gotoMenu(menuMainId);
                    return;
                }
                else if(key == '*') //nhan enter 
                {
                    step++;
                }

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
        void Doi_the_Master()
        {
            char key = Keypad::getKey();
            static char keydata[6];    
            static uint8_t keyCount = 0; 
            static uint8_t step=0;
            static int y=0;

            if(step == 0) //Nhập Pass
            {
                u8g2.clearBuffer();
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.enableUTF8Print();
                u8g2.setFontDirection(0);
                u8g2.setFont(u8g2_font_resoledbold_tr);            
                u8g2.setCursor(15,20);
                u8g2.printf("NHAP LAI PASSWORD");
                u8g2.setCursor(35,60);
                u8g2.printf("VHITEK.VN");          

                u8g2.setCursor(30,40);
                u8g2.printf("%c %c %c %c %c %c",
                            (keydata[0]) ? '*' : '_',
                            (keydata[1]) ? '*' : '_',
                            (keydata[2]) ? '*' : '_',
                            (keydata[3]) ? '*' : '_',
                            (keydata[4]) ? '*' : '_',
                            (keydata[5]) ? '*' : '_');
                u8g2.sendBuffer();

                if(keyCount>=6)
                {
                    if((VHITEK::Config::so_sanh_mk_menu(keydata) == 1) or (VHITEK::Config::so_sanh_mk_menu(keydata) == 2))
                    {
                        step=1;
                    }
                    else 
                    {
                        VHITEK::Keypad::clearKeypad(keydata, 6);
                        reInit = true;
                        step=0;
                        u8g2.clearBuffer();
                        u8g2.setCursor(25,35);
                        u8g2.printf("SAI PASSWORD!");
                        u8g2.sendBuffer();   
                        delay(2000);  
                        gotoMenu(menuMainId);                   
                    }
                }
            }
            else if(step == 1) //nhập thẻ
            {
                u8g2.clearBuffer();    
                u8g2.setCursor(40,20);
                u8g2.printf("VUI LONG");  
                u8g2.setCursor(30,30);
                u8g2.printf("NHAP THE MOI");  
                u8g2.setCursor(45,45);
                u8g2.printf("******");    
                u8g2.sendBuffer(); 

                tagStr(ID,ID_Master); //chuyen int sang string
                if(ID!=0) step=2;
            }
            else if(step==2) //lưu vào eeprom
            {
                for(int i=30; i<= 42; i++)
                {
                    accessI2C1Bus([&]
                        { myMem.put(i, ID_Master[y]); },
                    100);
                    y++;
                }
                Serial.printf("Save master: %s\n", ID_Master);
                ID=0;
                y=0;
                step=3;
            }
            else if(step==3)
            {
                u8g2.clearBuffer();    
                u8g2.setCursor(35,25);
                u8g2.printf("DANG LUU...");  
                u8g2.setCursor(30,40);
                u8g2.printf("VUI LONG CHO");    
                u8g2.sendBuffer(); 

                delay(2000);
                ID=0;
                step=0;
                keyCount = 0;
                VHITEK::Keypad::clearKeypad(keydata, 6);
                gotoMenu(menuMainId);

                /*Serial.print("ID_Master: ");

                for(int i=30; i<= 42; i++)
                {
                    accessI2C1Bus([&]
                        { myMem.get(i, ID_Master[y]); },
                    100);
                    y++;
                }
                y=0;

                for(int i=0; i<=12; i++)
                {
                    Serial.print(ID_Master[i]-'0');
                }*/
            }

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    step=0;keyCount = 0;
                    VHITEK::Keypad::clearKeypad(keydata, 6);
                    gotoMenu(menuMainId);
                    return;
                }
                else if(key == '*') //nhan enter 
                {
                    step++;
                }

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
        void menu_xem_id_the()
        {
            char key = Keypad::getKey();

            u8g2.clearBuffer();
            u8g2.setCursor(25,20);
            u8g2.printf("XEM ID CUA THE");  
            u8g2.setCursor(10,40);
            u8g2.printf("ID:");          
            u8g2.setCursor(30,40);
            u8g2.printf("%010ld",ID);              
            u8g2.sendBuffer();  

            if(key)   
            {
                if(key == '#') //thoat ve menu
                {
                    ID = 0;
                    gotoMenu(menuMainId);
                    return;
                }
            }
        }    
        void menu_cai_dat_ngay_gio()
        {
            static thoigian ngay_gio_moi;
            char key = Keypad::getKey();
            static char keydata[2];
            static uint8_t keyCount = 0; 
            static uint8_t step=0;

            if (reInit == true)
            {
                Keypad::clearKeypad(keydata, 2);
                // step = 0;
                keyCount = 0;
                reInit = false;
            }    
            
            if(step == 0) //cai dat ngay
            {
                u8g2.clearBuffer();
                u8g2.setCursor(45,10);
                u8g2.printf("CAI DAT");
                u8g2.setCursor(25,20);
                u8g2.printf("NGAY/THANG/NAM");            
                u8g2.setCursor(5,35);
                u8g2.printf("HIEN TAI: %02d/%02d/%02d", thoi_gian.ngay, thoi_gian.thang,thoi_gian.nam);             
                u8g2.setCursor(5,50);
                u8g2.printf("NGAY: %c%c",  
                            (keydata[0]) ? keydata[0] : '_',
                            (keydata[1]) ? keydata[1] : '_');                          
                u8g2.setCursor(70,60);
                u8g2.printf("(Enter)->");                               
                u8g2.sendBuffer();   
                ngay_gio_moi.ngay=atoi(keydata);                               
            }
            else if(step == 1) //cai dat thang
            {
                u8g2.clearBuffer();
                u8g2.setCursor(45,10);
                u8g2.printf("CAI DAT");
                u8g2.setCursor(25,20);
                u8g2.printf("NGAY/THANG/NAM");  
                u8g2.setCursor(5,50);
                u8g2.printf("THANG: %c%c",  
                            (keydata[0]) ? keydata[0] : '_',
                            (keydata[1]) ? keydata[1] : '_');                          
                u8g2.setCursor(70,60);
                u8g2.printf("(Enter)->"); 
                ngay_gio_moi.thang=atoi(keydata);

                if(ngay_gio_moi.ngay>31) //ngay > 31
                {
                    VHITEK::Keypad::clearKeypad(keydata, 2);
                    step = 0;
                }
                else if(ngay_gio_moi.ngay == 0)
                {
                    ngay_gio_moi.ngay=thoi_gian.ngay;
                    u8g2.setCursor(5,35);
                    u8g2.printf("HIEN TAI: %02d/%02d/%02d", ngay_gio_moi.ngay, thoi_gian.thang,thoi_gian.nam);             
                    u8g2.sendBuffer();
                }
                else 
                {          
                    u8g2.setCursor(5,35);
                    u8g2.printf("HIEN TAI: %02d/%02d/%02d", ngay_gio_moi.ngay, thoi_gian.thang,thoi_gian.nam);                                         
                    u8g2.sendBuffer();                    
                }
            }
            else if(step == 2) //cai dat nam
            {
                u8g2.clearBuffer();
                u8g2.setCursor(45,10);
                u8g2.printf("CAI DAT");
                u8g2.setCursor(25,20);
                u8g2.printf("NGAY/THANG/NAM");     
                u8g2.setCursor(5,50);
                u8g2.printf("NAM: %c%c",  
                            (keydata[0]) ? keydata[0] : '_',
                            (keydata[1]) ? keydata[1] : '_');                          
                u8g2.setCursor(70,60);
                u8g2.printf("(Enter)->");   
                ngay_gio_moi.nam=atoi(keydata);

                if(ngay_gio_moi.thang>12) //thang > 31
                {
                    VHITEK::Keypad::clearKeypad(keydata, 2);
                    step = 1;
                }
                else if(ngay_gio_moi.thang==0)
                {
                    ngay_gio_moi.thang=thoi_gian.thang;
                    u8g2.setCursor(5,35);
                    u8g2.printf("HIEN TAI: %02d/%02d/%02d", ngay_gio_moi.ngay, ngay_gio_moi.thang, ngay_gio_moi.nam);             
                    u8g2.sendBuffer();
                }
                else 
                {
                    if(ngay_gio_moi.nam==0)
                    {
                        ngay_gio_moi.nam = thoi_gian.nam;
                        u8g2.setCursor(5,35);
                        u8g2.printf("HIEN TAI: %02d/%02d/%02d", ngay_gio_moi.ngay, ngay_gio_moi.thang, ngay_gio_moi.nam);             
                        u8g2.sendBuffer();  
                    }
                    else 
                    {
                        ngay_gio_moi.nam+=2000;
                        u8g2.setCursor(5,35);
                        u8g2.printf("HIEN TAI: %02d/%02d/%02d", ngay_gio_moi.ngay, ngay_gio_moi.thang, ngay_gio_moi.nam);             
                        u8g2.sendBuffer();                         
                    }
                }
            }
            //luu ngay/thang/nam -> hoi xem co muon tiep tuc cai gio/phut/giay
            else if(step == 3) 
            {
                rtc.adjust(DateTime(ngay_gio_moi.nam, ngay_gio_moi.thang, ngay_gio_moi.ngay, thoi_gian.gio, thoi_gian.phut, thoi_gian.giay)); //nam, thang, ngay, gio, phut, giay
                u8g2.clearBuffer();
                u8g2.setCursor(45,10);
                u8g2.printf("DA LUU");
                u8g2.setCursor(25,20);
                u8g2.printf("NGAY/THANG/NAM"); 
                u8g2.setCursor(30,35);
                u8g2.printf("%02d:%02d:%02d", ngay_gio_moi.ngay, ngay_gio_moi.thang, ngay_gio_moi.nam);     
                u8g2.setCursor(5,50);                         
                u8g2.printf("Enter-> Setup Gio");
                u8g2.setCursor(5,60);
                u8g2.printf("Cancel-> THOAT");
                u8g2.sendBuffer();
            }
            else if(step == 4) //cai dat GIO
            {
                u8g2.clearBuffer();
                u8g2.setCursor(45,10);
                u8g2.printf("CAI DAT");
                u8g2.setCursor(25,20);
                u8g2.printf("GIO:PHUT:GIAY");            
                u8g2.setCursor(5,50);
                u8g2.printf("GIO: %c%c",  
                            (keydata[0]) ? keydata[0] : '_',
                            (keydata[1]) ? keydata[1] : '_');                          
                u8g2.setCursor(70,60);
                u8g2.printf("(Enter)->");                                
                u8g2.setCursor(5,35);
                u8g2.printf("HIEN TAI: %02d:%02d", ngay_gio_moi.gio, ngay_gio_moi.phut);  
                u8g2.sendBuffer();            
                ngay_gio_moi.gio = atoi(keydata);      
            }
            else if(step == 5) //cai dat PHUT
            {
                u8g2.clearBuffer();
                u8g2.setCursor(45,10);
                u8g2.printf("CAI DAT");
                u8g2.setCursor(25,20);
                u8g2.printf("GIO:PHUT:GIAY");            
                u8g2.setCursor(5,50);
                u8g2.printf("PHUT: %c%c",  
                            (keydata[0]) ? keydata[0] : '_',
                            (keydata[1]) ? keydata[1] : '_');                          
                u8g2.setCursor(70,60);
                u8g2.printf("(Enter)->");                               
                ngay_gio_moi.phut = atoi(keydata);                 

                if(ngay_gio_moi.gio > 23) //co nhan nut NHUNG gio lon hon 23 
                {
                    VHITEK::Keypad::clearKeypad(keydata, 2);
                    step = 4;
                }
                else
                {
                    u8g2.setCursor(5,35);
                    u8g2.printf("HIEN TAI: %02d:%02d", ngay_gio_moi.gio, ngay_gio_moi.phut);      
                    u8g2.sendBuffer();       
                }
            }
            else if(step == 6) //KIEM TRA VA LUU PHUT
            {
                if(ngay_gio_moi.phut > 59) //co nhan NUT NHUNG phut lon hon 59
                {
                    VHITEK::Keypad::clearKeypad(keydata, 2);
                    step = 5;
                }
                else
                {
                    step = 7;
                }
            }
            else if(step == 7) //Luu Gio:Phut:Giay
            {
                rtc.adjust(DateTime(thoi_gian.nam, thoi_gian.thang, thoi_gian.ngay, ngay_gio_moi.gio, ngay_gio_moi.phut, ngay_gio_moi.giay)); //nam, thang, ngay, gio, phut, giay
                u8g2.clearBuffer();
                u8g2.setCursor(45,20);
                u8g2.printf("DA LUU");
                u8g2.setCursor(25,30);
                u8g2.printf("GIO:PHUT:GIAY");  
                u8g2.setCursor(35,45);
                u8g2.printf("%02d:%02d:%02d", ngay_gio_moi.gio, ngay_gio_moi.phut, ngay_gio_moi.giay);                 
                u8g2.sendBuffer();  
                delay(3000);
                gotoMenu(menuMainId);
                reInit = true;
                VHITEK::Keypad::clearKeypad(keydata, 2);
                step = 0; keyCount = 0;                            
            }

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    gotoMenu(menuMainId);
                    reInit = true;
                    VHITEK::Keypad::clearKeypad(keydata, 2);
                    step = 0; keyCount = 0;
                    return;
                }
                else if(key == '*') //nhan enter 
                {
                    reInit = true;
                    step++;
                    // VHITEK::Keypad::clearKeypad(keydata, 2);
                }

                if (keyCount < 2)
                {
                    keydata[keyCount] = key;
                    keyCount++;
                }   
                else {
                    VHITEK::Keypad::clearKeypad(keydata, 2);
                    keyCount = 0;
                }              
            }                            
        }      
        void menu_xem_tu_chua_su_dung() //Hien thi cac tu chua su dung
        {
            char key = Keypad::getKey();           
            uint16_t so_tu_chua_SD[Tong_so_tu];

            uint16_t kiem_tra_co=0;
            uint16_t vitri=0;

            byte hang=20; byte cot=5;

            for(int sotu=0; sotu<=Tong_so_tu; sotu++) //dò từ 1 đến tổng số tủ
            {
                accessI2C1Bus([&]{
                        myMem.get(tinh_o_luu_the(sotu), kiem_tra_co);       
                }, 100);                 
                so_tu_chua_SD[vitri] = kiem_tra_co;
                vitri++;                
            }

            VHITEK::Config::KT_tong_tu_chua_SD();

            u8g2.clearBuffer();
            u8g2.setCursor(20,10);
            u8g2.printf("TU CHUA SU DUNG");
            // Serial.print("So tu chua SD: ");
            for(int i=1; i<=Tong_so_tu; i++)
            {
                if(so_tu_chua_SD[i]==0)
                {
                    u8g2.setCursor(cot,hang);
                    u8g2.printf("%d,", i);                     
                    if(cot > 90)   
                    {
                        hang+=10;
                        cot=5;
                    }
                    else
                    {
                        if(i>=10&&i<100) cot+=16;
                        if(i>=100) cot+=22;
                        if (i<10&&i>0) cot+=12;
                    } 
                    // Serial.print(so_tu_chua_SD[i]); Serial.print(", ");
                }
            }
            u8g2.sendBuffer();
            // Serial.println();

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    gotoMenu(menuMainId);
                    return;
                }                            
            }  
        }
        void xem_thong_tin_may()
        {
            char key = Keypad::getKey();
            QRCode qrcode;
            uint8_t LCDType = 1;
            uint8_t qrcodeData[qrcode_getBufferSize(9)];
            char wQR[100];
            sprintf(wQR, "%s", apSSID);
            qrcode_initText(&qrcode, qrcodeData, 9, ECC_HIGH, wQR);
            u8g2.clearBuffer();
            if (LCDType)
                u8g2.drawBox(0, 0, 64, 64);
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
            u8g2.setFont(u8g2_font_profont10_mf); // Courier New Bold 10,12,14,18,24
            u8g2.setCursor(67,10);
            u8g2.printf("MACHINE INFO"); 
            u8g2.setCursor(65,25);
            u8g2.printf("Firmware:"); 
            u8g2.setCursor(80,35);
            u8g2.print(FW);            
            u8g2.setCursor(65,50);
            u8g2.printf("SSID:");                                     
            u8g2.setCursor(65, 60);
            u8g2.printf("%s", apSSID);
            u8g2.sendBuffer();           

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    gotoMenu(menuMainId);
                    return;
                }             
            }    
        }

        //Menu ky thuat
#ifndef mocua
        void mo_tu_bat_ky() //Mo 1 tu bat ky: cho nhap vao so tu -> mo tu
        {
            char key = Keypad::getKey();
            static char keydata[3];
            static uint8_t keyCount = 0; 
            static uint8_t step=0;

            static uint16_t so_tu;

            if (reInit == true)
            {
                Keypad::clearKeypad(keydata, 3);
                step = 0;
                keyCount = 0;
                reInit = false;
            } 

            if(step == 0)
            {
                u8g2.clearBuffer();
                u8g2.setCursor(35,10);
                u8g2.printf("NHAP SO TU");   
                u8g2.setCursor(45,20);
                u8g2.printf("CAN MO");    
                u8g2.setCursor(20,30);
                u8g2.printf("(0: MO TAT CA)");                              
                u8g2.setCursor(5,45);
                u8g2.printf("TU SO: %c%c%c",  
                            (keydata[0]) ? keydata[0] : '_',
                            (keydata[1]) ? keydata[1] : '_',
                            (keydata[2]) ? keydata[2] : '_');                 
                u8g2.setCursor(45,60);  
                u8g2.printf("(ENTER) DE MO");                                                                
                u8g2.sendBuffer(); 

                so_tu = atoi(keydata);                
            }
            else //Mo tu
            {
                VHITEK::mo_tu_locker::hanh_dong_mo_tu(so_tu);
                delay(1000);   
                gotoMenu(menuMainId);        
            }

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    gotoMenu(menuMainId);
                    return;
                }
                else if(key == '*') //nhan enter 
                {
                    step++;
                }

                if (keyCount < 3)
                {
                    keydata[keyCount] = key;
                    keyCount++;
                }   
                else {
                    VHITEK::Keypad::clearKeypad(keydata, 3);
                    keyCount = 0;
                }              
            }                  
        }  
#endif   
        void cai_dat_tong_so_tu() //cai dat tong so tu
        {
            char key = Keypad::getKey();
            static char keydata[3];
            static uint8_t keyCount = 0; 
            static uint8_t step=0;

            static uint16_t tong_so_tu;
            user_setting user_clear;
            memset(&user_clear, 0, sizeof(user_clear));            

            if (reInit == true)
            {
                Keypad::clearKeypad(keydata, 3);
                step = 0;
                keyCount = 0;
                reInit = false;
            } 

            if(step == 0)
            {
                u8g2.clearBuffer();
                u8g2.setCursor(45,20);
                u8g2.printf("LUU Y!");   
                u8g2.setCursor(12,35);
                u8g2.printf("Toan bo thong tin");
                u8g2.setCursor(15,45);
                u8g2.printf("cua tu se bi xoa");
                u8g2.setCursor(70,60);
                u8g2.printf("(Enter)->");                
                u8g2.sendBuffer();  
            }
            else if(step == 1)
            {
                VHITEK::Config::doc_tong_so_tu();
                VHITEK::Keypad::clearKeypad(keydata, 3);
                keyCount = 0;
                step = 2;
            }            
            else if(step == 2)
            {
                u8g2.clearBuffer();
                u8g2.setCursor(45,20);
                u8g2.printf("CAI DAT");   
                u8g2.setCursor(35,30);
                u8g2.printf("TONG SO TU");              

                u8g2.setCursor(5,45);
                u8g2.printf("Hien tai: %d (tu)", Tong_so_tu);        
                u8g2.setCursor(5,55);
                u8g2.printf("So moi: %c%c%c (tu)",  
                            (keydata[0]) ? keydata[0] : '_',
                            (keydata[1]) ? keydata[1] : '_',
                            (keydata[2]) ? keydata[2] : '_');                                                    
                u8g2.sendBuffer(); 

                tong_so_tu = atoi(keydata);
            }
            else if(step == 3) //luu vao eeprom: 1000
            {
                if(tong_so_tu == 0) //Neu tong so tu nhap vao = 0
                {
                    VHITEK::Display::TB_tu_lon_hon_0();
                    delay(2000);
                    gotoMenu(menuMainId);                       
                }
                else 
                {
                    for(int i=1; i<=Tong_so_tu; i++) //Xoa tat ca cac tu
                    {
                        accessI2C1Bus([&]{
                            myMem.put(tinh_o_luu_the(i), user_clear);   
                        }, 100);
                        // VHITEK::Config::xem_eeprom_tu_bat_ky(1);                     
                    } 

                    accessI2C1Bus([&]{ //Luu tong so tu moi
                            myMem.put(1000, tong_so_tu);       
                    }, 100);  

                    VHITEK::Config::doc_tong_so_tu(); //doc lai tong so tu
                    // VHITEK::Config::KT_tong_tu_chua_SD();

                    check_send_tong_so_tu = true;
                    step = 4;
                }              
            }
            else if(step == 4)
            {
                reInit = true;
                u8g2.clearBuffer();
                u8g2.setCursor(30,30);
                u8g2.printf("DANG LUU..."); 
                u8g2.setCursor(20,40);
                u8g2.printf("TONG SO TU MOI"); 
                u8g2.sendBuffer();
                delay(2000);
                gotoMenu(menuMainId);
            }

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    gotoMenu(menuMainId);
                    return;
                }
                else if(key == '*') //nhan enter 
                {
                    step++;
                }

                if (keyCount < 3)
                {
                    keydata[keyCount] = key;
                    keyCount++;
                }
                else {
                    VHITEK::Keypad::clearKeypad(keydata, 3);
                    keyCount = 0;
                }                                 
            }                
        }
        void KT_RTC() //Kiem tra hoat dong mach thoi gian DS1307
        {
            char key = Keypad::getKey();
            static bool status;
            static bool pos;

            if(pos == 0)
            {
                if (! rtc.begin(&Wire)) status = 1; //loi
                pos = 1;
            }
            else
            {
                u8g2.clearBuffer();
                u8g2.setFont(u8g2_font_resoledbold_tr);
                u8g2.setCursor(12,15);
                u8g2.printf("Kiem tra hoat dong");
                u8g2.setCursor(8,25);
                u8g2.printf("mach thoi gian thuc");       
                u8g2.setCursor(2,50);
                if (status==1) {
                    u8g2.printf("Trang thai: LOI...!");
                } else u8g2.printf("Trang thai: OK");
                u8g2.sendBuffer();
            }

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    status=0;
                    pos=0;
                    gotoMenu(menuMainId);
                    return;
                }
            }            
        }
        void KT_bo_nho() //Kiem tra hoat dong 2 IC eeprom
        {
            char key = Keypad::getKey();

            static bool status_1;
            static bool status_2;
            static bool pos;

            if(pos == 0)
            {
                if(myMem2.begin(0x53,Wire1)==false) status_2 = 1; //EEPROM 2 loi
                if(myMem.begin(0x51, Wire1) == false) status_1 = 1; //EEPROM 1 loi
                pos = 1;
            }
            else
            {
                u8g2.clearBuffer();
                u8g2.setFont(u8g2_font_resoledbold_tr);
                u8g2.setCursor(12,15);
                u8g2.printf("Kiem tra hoat dong");
                u8g2.setCursor(12,25);
                u8g2.printf("bo nho board mach");       
                u8g2.setCursor(2,45);
                if (status_1==1) {
                    u8g2.printf("Bo nho 1: LOI...!");
                } else u8g2.printf("Bo nho 1: OK");
                u8g2.setCursor(2,55);
                if (status_2==1) {
                    u8g2.printf("Bo nho 2: LOI...!");
                } else u8g2.printf("Bo nho 2: OK");                                 
                u8g2.sendBuffer();
            }

            if(key)   
            {
                if(key == '#') //nhan cancel
                {
                    status_1=0;
                    status_2=0;
                    pos=0;
                    gotoMenu(menuMainId);
                    return;
                }
            }                 
        }

        void begin()
        {
            //Menu cua khach hang
            add_menu("Them tu moi", menuThemtu, 1, menu_them_tu);
            add_menu("Xoa tu bat ky", menuXoatubatky, 1, menu_xoa_tu_bat_ky);
            add_menu("Xoa tat ca cac tu", menuXoatatca, 1, menu_xoa_tat_ca_tu);
            add_menu("Xem thong tin may", xemthongtinmay, 1, xem_thong_tin_may);
            add_menu("Cai dat ngay gio", menucaidatngay, 1, menu_cai_dat_ngay_gio);
            add_menu("Xem tu chua su dung", tuchuasudung, 1, menu_xem_tu_chua_su_dung);
            add_menu("Doi the MASTER", doithemaster, 1, Doi_the_Master);
            add_menu("Xem ID the", menuXemIDthe, 1, menu_xem_id_the);

            //Menu cua ky thuat
            #ifndef mocua
            add_menu("Mo tu bat ky", kiemtratu, 2, mo_tu_bat_ky);
            #endif
            add_menu("Cai dat tong so tu", caidattongsotu, 2, cai_dat_tong_so_tu);
            add_menu("KT mach thoi gian", kt_rtc, 2, KT_RTC);
            add_menu("KT bo nho cua mach", kt_bonho, 2, KT_bo_nho);

            userLevel = 0;
            MenuID = 0;
            VHITEK::Keypad::clearKeypad();
        }

        void menu_page_up()
        {
            char idx = menuPage + 1;
            if (idx > totalMenuPage)
            {
                menuPage = 0;
                return;
            }
            menuPage++;
        }

        void menu_page_down()
        {
            if (menuPage == 0)
            {
                menuPage = totalMenuPage;
                return;
            }
            menuPage--;
        }

        void end()
        {
            userLevel = 0;
            MenuID = 0;
            VHITEK::Keypad::clearKeypad();
        }

        void mainMenuReinit()
        {
            int totalMainMenuELMCount = 0;
            for (int i = 0; i < 30; i++)
            {
                mainMenuELM[i].enabled = false;
                if ((menuELM[i].userLevel <= userLevel) && (strlen(menuELM[i].menuName)))
                {
                    mainMenuELM[totalMainMenuELMCount] = menuELM[i];
                    mainMenuELM[totalMainMenuELMCount].enabled = true;
                    // Serial.printf("totalMainMenuELMCount = %d %s\n", totalMainMenuELMCount, mainMenuELM[totalMainMenuELMCount].menuName);
                    totalMainMenuELMCount++;
                }
            }

            totalMenuPage = (totalMainMenuELMCount / 7);
            if ((totalMainMenuELMCount - (totalMenuPage * 7)) > 0)
            {
                totalMenuPage++;
            }

            if (totalMenuPage > 0)
            {
                totalMenuPage--;
            }
        }

        void nhap_pass() //nhap pass de vao menu
        {
            char key = Keypad::getKey();

            static char keypadData[8] = {0, 0, 0, 0, 0, 0, 0};
            static uint8_t keypadCount = 0;
            if (reInit)
            {
                Keypad::clearKeypad(keypadData, 8);
                Keypad::clearKeypad();
                keypadCount = 0;
                reInit = false;
            }
            if (key)
            {
                if (keypadCount < 6)
                {
                    keypadData[keypadCount] = key;
                    keypadCount++;
                }
            }

            u8g2.clearBuffer();
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.enableUTF8Print();
            u8g2.setFontDirection(0);
            u8g2.setFont(u8g2_font_resoledbold_tr);            
            u8g2.setCursor(15,20);
            u8g2.printf("MOI NHAP PASSWORD");
            u8g2.setCursor(35,60);
            u8g2.printf(logoTSE);          

            u8g2.setCursor(30,40);
            u8g2.printf("%c %c %c %c %c %c",
                        (keypadData[0]) ? '*' : '_',
                        (keypadData[1]) ? '*' : '_',
                        (keypadData[2]) ? '*' : '_',
                        (keypadData[3]) ? '*' : '_',
                        (keypadData[4]) ? '*' : '_',
                        (keypadData[5]) ? '*' : '_');
            u8g2.sendBuffer();

            if(keypadCount >= 6)
            {
                if(VHITEK::Config::so_sanh_mk_menu(keypadData) == 1){ //Menu khach hang
                    userLevel = 1;
                    mainMenuReinit();
                    keypadCount = 0;
                    VHITEK::Keypad::clearKeypad(keypadData, 8);
                }
                else if (VHITEK::Config::so_sanh_mk_menu(keypadData) == 2) { //Menu cho ky thuat
                    userLevel = 2;
                    mainMenuReinit();
                    keypadCount = 0;                    
                    VHITEK::Keypad::clearKeypad(keypadData, 8);
                }
                else if(VHITEK::Config::so_sanh_mk_menu(keypadData) == 0)  //sai PassWord
                { 
                    u8g2.clearBuffer();
                    u8g2.setCursor(25,30);
                    u8g2.printf("SAI PASSWORD!");
                    u8g2.setCursor(25,40);
                    u8g2.printf("MOI NHAP LAI");   
                    u8g2.sendBuffer();
                    delay(3000);
                    userLevel = 0;
                    keypadCount = 0;
                    VHITEK::Keypad::clearKeypad(keypadData, 8);
                }
            }
            // Serial.printf("%c, %c, %c, %c, %c, %c\n", keypadData[0], keypadData[1], keypadData[2], keypadData[3], keypadData[4], keypadData[5]);
        }

        void mainMenu()
        {
            if (reInit == true)
            {
                mainMenuReinit();
                menuPage = 0;
                reInit = false;
            }

            u8g2.refreshDisplay();
            u8g2.setBitmapMode(0);
            u8g2.clearBuffer(); // clear the internal memory
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.setCursor(108, 61);
            u8g2.setFont(u8g2_font_resoledbold_tr); // Courier New Bold 10,12,14,18,24
            u8g2.printf("%d/%d", menuPage + 1, totalMenuPage + 1);
            u8g2.setFont(u8g2_font_resoledbold_tr); // Courier New Bold 10,12,14,18,24

            int cursorX = 10;
            uint8_t currentMenu[7];
            uint8_t activeMenuCount = 0;
            for (int i = 0; i < 7; i++)
            {
                uint8_t menuPtr = (menuPage * 7) + i;
                if (mainMenuELM[menuPtr].enabled)
                {
                    u8g2.setCursor(2, cursorX);
                    u8g2.printf("%d.%s", i + 1, mainMenuELM[menuPtr].menuName);
                    currentMenu[activeMenuCount] = mainMenuELM[menuPtr].menuIdx;
                    cursorX += 8;
                    activeMenuCount++;
                }
            }
            u8g2.sendBuffer();

            char key = Keypad::getKey();
            if (key)
            {
                if (key == '8')
                {
                    menu_page_down();
                    return;
                }

                if (key == '0')
                {
                    menu_page_up();
                    return;
                }

                key = key - '0';
                if (key == 0)
                {
                    return;
                }
                if (key <= activeMenuCount)
                {
                    gotoMenu(currentMenu[key - 1]);
                }
            }

            // Serial.printf("MenuID: %d\n", MenuID);
        }

        void loop()
        {
            if (userLevel == 0) nhap_pass(); //nhap password
            else if (userLevel == 1) { //Menu khach hang
                if(MenuID == 0)
                    mainMenu();
                else subMenuFunc();        
            } 
            else if (userLevel == 2) { //Menu ky thuat
                if(MenuID == 0)
                    mainMenu();
                else subMenuFunc();             
            } 

            // Serial.printf("userLevel: %d, MenuID: %d, MenuPage: %d, TotalMenuPage: %d \n",userLevel, MenuID, menuPage, totalMenuPage);
        }
    }
}