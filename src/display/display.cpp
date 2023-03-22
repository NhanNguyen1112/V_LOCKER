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
            u8g2.setCursor(10, 60);
            u8g2.printf(logoTSE);                 
            u8g2.sendBuffer();
        }
        void hienthiLOGO() //hiện logo TSE or VHITEK
        {
            u8g2.setFont(u8g2_font_resoledbold_tr);
            #ifdef logoTSE
            u8g2.setCursor(10,60);
            u8g2.printf(logoTSE);
            #else 
            u8g2.setCursor(35,60);
            u8g2.printf(logoVHITEK);
            #endif
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

            VHITEK::WSSID = WiFi.SSID();
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
            u8g2.setCursor(20, 60);
            u8g2.printf(logoVHITEK);                                    
            u8g2.sendBuffer();                
        }
#else 
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
            if((uint32_t)(millis() - lastTick_page) > 5000)
            {
                check_page = !check_page;
                lastTick_page = millis();
            }          

            if(check_page == 0) //Man hinh thoi gian
            {
                VHITEK::WSSID = WiFi.SSID();
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
                u8g2.setCursor(10, 60);
                u8g2.printf(logoVHITEK);                                    
                u8g2.sendBuffer();                
            }  
            else  //Man hinh moi chon so tu
            {
                u8g2.clearBuffer();
                u8g2.drawXBM(0, 0, 128, 64, xinchaomoichontu_bits);                       
                u8g2.sendBuffer();           
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