#include "vhitek.h"
namespace VHITEK
{
    #define LCD_RESET 4  // RST on LCD
    #define LCD_CS 2     // RS on LCD
    #define LCD_CLOCK 19 // E on LCD
    #define LCD_DATA 21  // R/W on LCD

    U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/LCD_CS, /* reset=*/LCD_RESET);

    namespace Display
    {
        void TB_dang_KT_cai_dat()
        {
            u8g2.clearBuffer();    
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(25,30);
            u8g2.printf("DANG KIEM TRA");  
            u8g2.setCursor(30,45);
            u8g2.printf("CAI DAT MAY");    
            u8g2.sendBuffer();
        }
        void thong_bao_tu_da_co() //thong bao TU NAY DA CO
        {
            u8g2.clearBuffer();    
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(40,30);
            u8g2.printf("TU DA CO");  
            u8g2.setCursor(15,40);
            u8g2.printf("MOI CHON TU KHAC");    
            u8g2.sendBuffer();
        }
        void TB_tu_chua_su_dung() //Thong bao tu chua duoc su dung
        {
            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(40,25);
            u8g2.printf("KHONG CO!");   
            u8g2.setCursor(5,40);
            u8g2.printf("TU CHUA DUOC SU DUNG");              
            u8g2.sendBuffer();
        }
        void TB_lon_hon_tong_TU() //Thong bao so tu vua nhap lon hon tong so tu quy dinh
        {
            u8g2.clearBuffer(); 
            u8g2.setFont(u8g2_font_resoledbold_tr);   
            u8g2.setCursor(30,25);
            u8g2.printf("TU VUA NHAP");  
            u8g2.setCursor(20,35);
            u8g2.printf("LON HON TONG SO");   
            u8g2.setCursor(30,45);
            u8g2.printf("TU QUY DINH");                       
            u8g2.sendBuffer();
        }
        void TB_tu_lon_hon_0() //thong bao so tu nhap vao phai lon hon 0
        {
            u8g2.clearBuffer();    
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(23,25);
            u8g2.printf("SO TU NHAP VAO");  
            u8g2.setCursor(23,35);
            u8g2.printf("PHAI LON HON 0");   
            u8g2.setCursor(33,45);
            u8g2.printf("(1 -> 999)");                       
            u8g2.sendBuffer();
        }
        void TB_ADD_khac_0() //TB địa chỉ nhập vào phải lớn hơn 0
        {
            u8g2.clearBuffer();    
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(17,25);
            u8g2.printf("DIA CHI NHAP VAO");  
            u8g2.setCursor(23,35);
            u8g2.printf("PHAI LON HON 0");                          
            u8g2.sendBuffer();
        }  
        void TB_update_FOTA() //man hinh thong bao update Fimware
        {
            u8g2.clearBuffer();
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.enableUTF8Print();
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setFontDirection(0);
            u8g2.setCursor(35, 15);
            u8g2.printf("DANG UPDATE");
            u8g2.setCursor(40, 30);
            u8g2.printf("Vui long");   
            u8g2.setCursor(30, 40);
            u8g2.printf("Cho 3-5 phut");                              
            u8g2.setFont(u8g2_font_ncenB10_tr);
            #ifdef logoVHITEK
            u8g2.setCursor(20, 60);
            u8g2.printf(logoVHITEK);  
            #else
            u8g2.setCursor(10, 60);    
            u8g2.printf(logoTSE); 
            #endif                 
            u8g2.sendBuffer();
        }
        void TB_2_sotu_bangnhau()
        {
            u8g2.clearBuffer();    
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(18,25);
            u8g2.printf("2 SO TU VUA NHAP");  
            u8g2.setCursor(40,35);
            u8g2.printf("BANG NHAU");   
            u8g2.setCursor(18,45);
            u8g2.printf("XIN MOI NHAP LAI");                       
            u8g2.sendBuffer();            
        }
        void TB_Server_Disconnect() //TB mất kết nối Server
        {
            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.setCursor(35, 25);
            u8g2.printf("THONG BAO");
            u8g2.setCursor(10, 45);
            u8g2.printf("MAT KET NOI SERVER");
            u8g2.sendBuffer();
        }
        void Trang_thanh_toan(String VNPAY, cabine_transac NewTrans)
        {
            VHITEK::Config::QR_VNP(VNPAY);
            u8g2.setFont(u8g2_font_profont10_mf); // Courier New Bold 10,12,14,18,24 
            u8g2.setCursor(70,10);
            u8g2.printf("QUET MA DE");
            u8g2.setCursor(70,19);
            u8g2.printf("THANH TOAN"); 
            u8g2.setFont(u8g2_font_u8glib_4_tr);
            u8g2.setCursor(85,26);
            u8g2.printf("HOAC"); 
            u8g2.setFont(u8g2_font_profont10_mf);
            u8g2.setCursor(75,36);
            u8g2.printf("TIEN MAC"); 
            u8g2.setCursor(65,52);
            u8g2.printf("TONG:%d", NewTrans.money); 
            u8g2.setCursor(65,62);
            u8g2.printf("NHAN:%ld", VHITEK::BILL::soTienDaNhan);
            u8g2.sendBuffer();
        }
        void Khong_con_tu_trong() //Không còn tủ trống
        {
            static uint64_t lastTick=0;
            static bool page=0;

            if((uint32_t)(millis() - lastTick) > 2000)
            {
                page = !page;
                lastTick = millis();
            } 
            
            if(page==0)
            {
                u8g2.clearBuffer();   
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.setFont(u8g2_font_unifont_t_vietnamese2);
                u8g2.drawUTF8(30, 28, "KHÔNG CÒN");
                u8g2.drawUTF8(30, 50, "TỦ TRỐNG");
                u8g2.sendBuffer(); 
            }
            else 
            {
                u8g2.clearBuffer();   
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.setFont(u8g2_font_ncenB10_tr);
                u8g2.setCursor(25, 20);    
                u8g2.printf("NO MORE");
                u8g2.setCursor(35, 40);    
                u8g2.printf("EMPTY");
                u8g2.setCursor(22, 60);    
                u8g2.printf("CABINETS");
                u8g2.sendBuffer();   
            }
        }
        void de_hang_vao() //Để hàng vào và đóng cửa
        { //Put the product inside and closure
            u8g2.clearBuffer();
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.setFont(u8g2_font_unifont_t_vietnamese2);
            u8g2.drawUTF8(20, 15, "ĐỂ HÀNG VÀO");
            u8g2.drawUTF8(20, 30, "VÀ ĐÓNG CỬA");
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(5, 50);
            u8g2.printf("PUT THE PRODUCT");
            u8g2.setCursor(5, 60);
            u8g2.printf("INSIDE & CLOSURE");
            u8g2.sendBuffer();
        }
        void dang_tao_GD() //Đang tạo giao dịch
        {
            u8g2.clearBuffer();   
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.setFont(u8g2_font_unifont_t_vietnamese1);
            u8g2.drawUTF8(33, 15, "ĐANG TẠO");
            u8g2.drawUTF8(30, 30, "GIAO DỊCH");
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(5, 50);
            u8g2.printf("CREATING");
            u8g2.setCursor(5, 60);
            u8g2.printf("A NEW TRANSACTION");
            u8g2.sendBuffer();
        }
        void mo_cua(uint16_t sotu) //Mở cửa
        {
            u8g2.clearBuffer();
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.setFont(u8g2_font_unifont_t_vietnamese1);
            u8g2.drawUTF8(25, 20, "MỞ TỦ/OPEN");
            u8g2.setFont(u8g2_font_timB24_tf);
            u8g2.setCursor(45, 55);
            u8g2.printf("%02d", sotu);
            u8g2.sendBuffer();
        }
        void TB_thoi_tien() //TB thối tiền
        {
            u8g2.clearBuffer();
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.setFont(u8g2_font_unifont_t_vietnamese2);
            u8g2.drawUTF8(10, 30, "NHẬN TIỀN THỪA");
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(2, 50);
            u8g2.printf("GET ROTTEN MONEY BACK");
            u8g2.sendBuffer();
        }
        void quet_ma_lay_SP() //Quét mã để lấy sản phẩm
        {
            u8g2.clearBuffer();
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.setFont(u8g2_font_unifont_t_vietnamese2);
            u8g2.drawUTF8(38, 15, "QUÉT MÃ");
            u8g2.drawUTF8(30, 30, "NHẬN HÀNG");
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(5, 50);
            u8g2.printf("SCAN THE CODE");
            u8g2.setCursor(5, 60);
            u8g2.printf("FOR PICKUP");
            u8g2.sendBuffer();
        }
        void nhan_hang_dong_cua() //Nhận hàng và đóng cửa
        {
            u8g2.clearBuffer();
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.setFont(u8g2_font_unifont_t_vietnamese2);
            u8g2.drawUTF8(28, 15, "VUI LÒNG");
            u8g2.drawUTF8(25, 30, "NHẬN HÀNG");
            u8g2.drawUTF8(20, 45, "VÀ ĐÓNG CỬA");
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(10, 60);
            u8g2.printf("PICKUP AND CLOSURE");
            u8g2.sendBuffer();
        }

        #ifdef mocua
        void hien_ngay_gio() //Hien thi ngay, gio tai man hinh chinh
        {
            static uint64_t lastTick=0;
            static bool check=0;

            if((uint32_t)(millis() - lastTick) > 5000)
            {
                check = !check;
                lastTick = millis();
            }         

            u8g2.clearBuffer();
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.enableUTF8Print();
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setFontDirection(0);
            if(check == 0)
            {
                u8g2.setCursor(35,10);
                u8g2.printf("%02d/%02d/%d", thoi_gian.ngay, thoi_gian.thang,thoi_gian.nam);   
            }
            else 
            {
                u8g2.setFont(u8g2_font_profont10_mf);
                u8g2.setCursor(5,10);
                u8g2.print("Wifi:");
                u8g2.setCursor(30,10);
                u8g2.print(WiFi.SSID());
                u8g2.setCursor(105,10);
                u8g2.print(VHITEK::Config::Wifi_RSSI()); 
                u8g2.setFont(u8g2_font_resoledbold_tr);   
                u8g2.setCursor(120,10);
                u8g2.print("%");  
            }
            u8g2.setFont(u8g2_font_timB24_tf);   
            u8g2.setCursor(27, 41);
            u8g2.printf("%02d:%02d", thoi_gian.gio, thoi_gian.phut); 
            u8g2.setFont(u8g2_font_ncenB10_tr);
            #ifdef logoVHITEK
            u8g2.setCursor(20, 60);
            u8g2.printf(logoVHITEK);  
            #else
            u8g2.setCursor(10, 60);    
            u8g2.printf(logoTSE); 
            #endif                                    
            u8g2.sendBuffer();                
        }
        #endif

        #ifdef Locker_NoiBo
        void hien_ngay_gio() //Hien thi ngay, gio tai man hinh chinh
        {
            static uint64_t lastTick=0;
            static uint64_t lastTick_page=0;
            static bool check=0;
            static bool check_page=0;

            if((uint32_t)(millis() - lastTick) > 2000)
            {
                check = !check;
                lastTick = millis();
            }
            if((uint32_t)(millis() - lastTick_page) > 8000)
            {
                check_page = !check_page;
                lastTick_page = millis();
            }         

            if(check_page == 0)
            {
                u8g2.clearBuffer();
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.enableUTF8Print();
                u8g2.setFont(u8g2_font_resoledbold_tr);
                u8g2.setFontDirection(0);
                if(check == 0)
                {
                    u8g2.setCursor(35,10);
                    u8g2.printf("%02d/%02d/%02d", thoi_gian.ngay, thoi_gian.thang,thoi_gian.nam);   
                }
                else 
                {
                    u8g2.setFont(u8g2_font_profont10_mf);
                    u8g2.setCursor(5,10);
                    u8g2.print("Wifi:");
                    u8g2.setCursor(30,10);
                    u8g2.print(WiFi.SSID());
                    u8g2.setCursor(105,10);
                    u8g2.print(VHITEK::Config::Wifi_RSSI()); 
                    u8g2.setFont(u8g2_font_resoledbold_tr);   
                    u8g2.setCursor(120,10);
                    u8g2.print("%");  
                }
                u8g2.setFont(u8g2_font_timB24_tf);   
                u8g2.setCursor(27, 41);
                u8g2.printf("%02d:%02d", thoi_gian.gio, thoi_gian.phut); 

                u8g2.setFont(u8g2_font_ncenB10_tr);
                #ifdef logoVHITEK
                u8g2.setCursor(20, 60);
                u8g2.printf(logoVHITEK);  
                #else
                u8g2.setCursor(10, 60);    
                u8g2.printf(logoTSE); 
                #endif                                   
                u8g2.sendBuffer(); 
            } 
            else 
            {
                u8g2.clearBuffer();
                u8g2.drawXBM(0, 0, 128, 64, xinchaomoichontu_bits);                       
                u8g2.sendBuffer();
            }              
        }
        #endif

        #ifdef Locker_Ship_Barcode
        void hien_ngay_gio() //Hien thi ngay, gio tai man hinh chinh
        {
            static uint64_t lastTick=0;
            static uint64_t lastTick_page=0;
            static bool check=0;
            static int check_page=0;

            if((uint32_t)(millis() - lastTick) > 2000)
            {
                check = !check;
                lastTick = millis();
            }

            if(check_page == 0)
            {
                if((uint32_t)(millis() - lastTick_page) > 7000)
                {
                    check_page = 1;
                    lastTick_page = millis();
                }
            }  
            else if(check_page==1)
            {
                if((uint32_t)(millis() - lastTick_page) > 5000)
                {
                    check_page = 2;
                    lastTick_page = millis();
                }
            }   
            else if(check_page==2)
            {
                if((uint32_t)(millis() - lastTick_page) > 5000)
                {
                    check_page = 3;
                    lastTick_page = millis();
                }
            } 
            else if(check_page==3)
            {
                if((uint32_t)(millis() - lastTick_page) > 5000)
                {
                    check_page = 0;
                    lastTick_page = millis();
                }
            }     

            if(check_page == 0) //Man hinh thoi gian
            {
                u8g2.clearBuffer();
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.enableUTF8Print();
                u8g2.setFont(u8g2_font_resoledbold_tr);
                u8g2.setFontDirection(0);
                if(check == 0)
                {
                    u8g2.setCursor(35,10);
                    u8g2.printf("%02d/%02d/%02d", thoi_gian.ngay, thoi_gian.thang,thoi_gian.nam);   
                }
                else 
                {
                    u8g2.setFont(u8g2_font_profont10_mf);
                    u8g2.setCursor(5,10);
                    u8g2.print("Wifi:");
                    u8g2.setCursor(30,10);
                    u8g2.print(WiFi.SSID());
                    u8g2.setCursor(105,10);
                    u8g2.print(VHITEK::Config::Wifi_RSSI()); 
                    u8g2.setFont(u8g2_font_resoledbold_tr);   
                    u8g2.setCursor(120,10);
                    u8g2.print("%");  
                }
                u8g2.setFont(u8g2_font_timB24_tf);   
                u8g2.setCursor(27, 41);
                u8g2.printf("%02d:%02d", thoi_gian.gio, thoi_gian.phut); 
                u8g2.setFont(u8g2_font_ncenB10_tr);
                #ifdef logoVHITEK
                u8g2.setCursor(20, 60);
                u8g2.printf(logoVHITEK);  
                #else
                u8g2.setCursor(10, 60);    
                u8g2.printf(logoTSE); 
                #endif                                                           
                u8g2.sendBuffer();                
            }  
            else if(check_page == 1)  //Pgae 1
            {
                u8g2.clearBuffer();   
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.setFont(u8g2_font_unifont_t_vietnamese1);
                u8g2.drawUTF8(33, 28, "NHẤN NÚT");
                u8g2.drawUTF8(20, 50, "ĐỂ GỬI HÀNG");
                u8g2.sendBuffer();           
            }
            else if(check_page == 2)  //Pgae 2
            {
                u8g2.clearBuffer();   
                u8g2.drawFrame(0, 0, 128, 64);
                u8g2.setFont(u8g2_font_ncenB10_tr);
                u8g2.setCursor(2, 30);    
                u8g2.printf("PRESS BUTTON");
                u8g2.setCursor(30, 50);    
                u8g2.printf("TO SHIP");
                u8g2.sendBuffer();   
            }
            else if(check_page == 3)  //Pgae 3
            {
                quet_ma_lay_SP(); //Quét mã để lấy hàng
            }
        }
        #endif

        //Hiển thị màn hình nhập số tủ
        // đối số đưa vào là key data
        //Cho nhập số tủ -> trả về số tủ
        uint16_t nhap_so_tu(char *keydata) 
        {
            uint16_t so_tu;
            u8g2.clearBuffer();    
            u8g2.drawFrame(0, 0, 128, 64);
            u8g2.enableUTF8Print();
            u8g2.setFontDirection(0);
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setCursor(40,20);
            u8g2.printf("XIN CHAO");  
            u8g2.setCursor(20,30);
            u8g2.printf("MOI NHAP SO TU");          
            u8g2.setCursor(5,50);
            u8g2.printf("TU SO: "); 
            u8g2.setCursor(73,60);
            u8g2.printf("(Enter)->");               

            u8g2.setCursor(45,50);
            u8g2.printf("%c%c%c",
                        (keydata[0]) ? keydata[0] : '_',
                        (keydata[1]) ? keydata[1] : '_',
                        (keydata[2]) ? keydata[2] : '_');                                
            u8g2.sendBuffer(); 
            return so_tu = atoi(keydata);              
        }

        void setup()
        {
            SPI.begin(LCD_CLOCK, -1, LCD_DATA, LCD_CS);

            u8g2.setBusClock(700000);   //Chỉ định tốc độ đồng hồ bus (tần số) cho I2C và SPI. 
            //                             Giá trị mặc định sẽ được sử dụng nếu hàm này không được gọi. 
            //                             Lệnh này phải được đặt trước lệnh gọi đầu tiên tới u8g2.begin () hoặc u8g2.initDisplay () .
            u8g2.begin();

            u8g2.enableUTF8Print();
            u8g2.setFont(u8g2_font_resoledbold_tr);
            u8g2.setFontDirection(0);   //xác định hướng vẽ của tất cả các chuỗi; 
                                        // 0: trái->phải; 1: từ trên xuống; 2: phải->trái; 3: xuống đầu trang

            u8g2.clearBuffer();
            u8g2.drawXBM(0, 0, 128, 64, logo_vhitek_bits); //vẽ XBM Bitmap: x, y, w, h, bitmap
            u8g2.sendBuffer();
            delay(2000);
            // Serial.println("Display Module Initialized"); 
        }

        void loop()
        {
            // nhap_so_tu();
        }
    }
}