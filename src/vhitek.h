#ifndef vhitek_h
#define vhitek_h

#include <Arduino.h>
#include "ArduinoJson.h"
#include "RFID.h"
#include "display/logo.h"
#include <SPI.h>
#include "WiFi.h"
#include "SPI.h"
#include <stdlib.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <SPIFFS.h>
#include <FS.h>
#include <qrcode.h>
#include <IoAbstraction.h>
#include <TaskManagerIO.h>
#include <KeyboardManager.h>
#include <IoAbstractionWire.h>
#include <U8g2lib.h>
#include "SparkFun_External_EEPROM.h"
#include "RTClib.h"
#include "time.h"
#include <sys/time.h>
#include "AsyncJson.h"
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <esp32fota.h>
#include <HTTPClient.h>
#include "Adafruit_Thermal.h"

#define V1
#define mocua

#define LCD_RESET 4  // RST on LCD
#define LCD_CS 2     // RS on LCD
#define LCD_CLOCK 19 // E on LCD
#define LCD_DATA 21  // R/W on LCD

#define U8_Width 128 //Hang LCD
#define U8_Height 64 //Cot LCD

#define MK_khach 123456 //Setup MK cho khach
#define MK_KT 456789    //Setup MK cho ky thuat vien

#define PIN_MENU 15 //Nut MENU
#define PIN_LEVEL_SHIFTER_ENABLE 13 //Luon de muc HIGH de ENABLE ESP32

#ifdef mocua
#define PIN_PUMP 12
#define PIN_VAL1 27
#define PIN_VAL2 14
#else
#define PIN_595_DATA 12
#define PIN_595_CLOCK 14
#define PIN_595_LATCH 27
#define PIN_165_LATCH 32
#define PIN_ENC1 34
#endif

#define TX 5
#define RX 18

#define so_luong_menu 30 //tong so menu
#define logoTSE "TSEVENDING" //10,60
#define logoVHITEK "VHITEK.VN" //35,60
#define FW_TYPE  "VLOCKER-V1"
#define FW  "LK0.0.1"
#define FW_server 1


/*
*  Bàn phím
*  màn hình
*  1 nút nhấn gửi hàng
*  UART Máy in nhiệt
*  UART đầu quét QR
*  UART Bill tiền 
*  
* Module RS485 Âm thanh
* Module RS485: máy in nhiệt, đầu đọc QR
* 
*/

namespace VHITEK
{
    typedef struct
    {
        uint32_t MASS;
        uint32_t KH;
        uint32_t KT;
    } password;
    extern password Vpass;

    typedef struct //
    {
        uint16_t so_tu; //2 byte
        char ID_user[13]; //13 byte: 1 byte 0 o cuoi
        uint8_t mat_khau[10]; //10 byte: 1 byte 0 o cuoi
        uint8_t send_data_check; // 1: chua gui; 0: da gui
        uint16_t check_sum; //2 byte 
    } user_setting;

    typedef struct //tong 28byte
    {
        uint32_t IDX; //4byte
        uint8_t ID[13]; //13 byte: 0->12
     
        uint8_t trang_thai; //1 byte
        uint16_t so_tu; //2 byte
        uint8_t send_data_check; //1 byte

        uint8_t ngay; //1 byte
        uint8_t thang; //1 byte
        uint8_t nam; //1 byte
        uint8_t gio; //1 byte

        uint8_t phut; //1 byte
        uint16_t check_sum; //2 byte   
    } hanh_dong;

    typedef struct 
    {
        int ngay;
        int thang;
        int nam;
        int gio;
        int phut;
        int giay;
    } thoigian;
    extern thoigian thoi_gian;

    extern Adafruit_Thermal printer;

    extern RTC_DS1307 rtc;
    
    extern U8G2_ST7920_128X64_F_HW_SPI u8g2;
    extern volatile uint64_t ID;
    extern char ID_Master[13];
    extern uint8_t tuchuasd[24];
    extern ExternalEEPROM myMem;
    extern ExternalEEPROM myMem2;
    extern char apSSID[30];
    extern String socuoiID;
    extern String WSSID;
    extern String bufferGM65;

    extern uint32_t IDX_hien_tai;
    extern uint32_t dia_chi_IDX_hien_tai;

    extern boolean check_read_eeprom_1;
    extern boolean check_read_eeprom_2;
    extern bool check_send_tong_so_tu;
    extern bool check_send_card_info;
    extern bool check_update_FOTA;
    extern bool check_update_machine;
    extern bool check_his_send;

    extern uint16_t Tong_so_tu;
    extern uint16_t Tong_tu_chua_SD;
    extern uint16_t diachi_giaodich;

    unsigned short cal_crc_loop_CCITT_A(short l, unsigned char *p);
    bool accessI2C1Bus(std::function<void()> &&Callback, uint32_t timeout);
    bool accessI2CBus(std::function<void()> &&Callback, uint32_t timeout);
    bool accessSPIBus(std::function<void()> &&Callback, uint32_t timeout);

    namespace EEPROM
    {
        void begin();
        int16_t sumCalc_eeprom_1(user_setting data);
        bool write_eeprom_1(user_setting user_check);
        user_setting read_eeprom_1(uint16_t so_tu);

        int16_t sumCalc_eeprom_2(hanh_dong data);
        bool write_eeprom_2(hanh_dong giaodich);
        hanh_dong read_eeprom_2(uint16_t dia_chi);
    }

    namespace mo_tu_locker
    {
        bool kiem_tra_dung_tu(uint16_t so_tu, user_setting user_check);
        void hanh_dong_mo_tu(uint16_t so_tu);
        void mo_cua_cuon();
        void mo_tu_locker();
        void locker_Barcode();
        void locker_button();
        bool locker_open(uint8_t id);
        int read_locker_status(uint16_t locker_id);
    }

    namespace transaction
    {
        void luu_hanh_dong(uint16_t so_tu, uint8_t trang_thai);
        void load_du_lieu();
        void XEM_eeprom_2(uint32_t IDX);
    }

    namespace Ds1307
    {
        void begin();
        void loop();
        void xem_thoi_gian();
    }

    namespace Config
    {
        String test(uint32_t IDX, int stastus);
        // void getdata(bool &readdt);
        void sendtest(uint32_t IDX, int stastus);

        void InNhiet(String data);

        String toJson(hanh_dong data);
        String Json_tong_tu(); //Tao Json tong so tu
        String Json_thong_tin_tu(user_setting user); //Tao Json thong tin tu
        String Json_machine_status(); //Tao Json tinh trang may
        void cai_dat_mat_khau();
        int so_sanh_mk_menu(char *mk);
        void xem_eeprom_tu_bat_ky(int so_tu);
        void xem_tung_o();
        void All_Clear_eeprom(int eep, uint16_t Max_diachi); // Xoa toan bo IC eeprom luu hanh dong mo cua
        
        String loadChipID();
        void doc_so_cuoi_ID_MAY();

        void Test_led();
        void doc_tong_so_tu(); //Doc tong so tu
        void Read_ID_master();
        int Wifi_RSSI(); //RSSI cua Wifi
        uint16_t KT_tong_tu_chua_SD(); //Tinh tong so tu chua su dung
    }

    namespace RFID
    {
        void begin();
        void loop();
        uint64_t read_id_card();
    }

    namespace Menu
    {
        enum MENU_IDX
        {
            menuMainId = 0,
            menuThemtu,
            menuXoatubatky,
            menuXoatatca,
            xemthongtinmay,
            menucaidatngay,
            kiemtratu,
            motubatky,
            caidattongsotu,
            tuchuasudung,
            doithemaster,
            menuXemIDthe,
            kt_rtc,
            kt_bonho
        };

        void loop();
        void begin();
        void end();
        uint16_t tinh_o_luu_the(int so_tu);
        bool kiem_tra_tu_da_co(int so_tu);
    }

    namespace Display
    {
        void setup();
        void loop();
        void hien_ngay_gio();
        uint16_t nhap_so_tu(char *keydata);
        void TB_tu_lon_hon_0();
        void TB_lon_hon_tong_TU();
        void TB_tu_chua_su_dung();
        void thong_bao_tu_da_co();
        void TB_update_FOTA(); //man hinh thong bao update Fimware
    }

    namespace Keypad
    {
        void setup();
        void loop();
        char getKey();
        void clearKeypad(char *keyData, int lenght);
        void clearKeypad();
    }    

    namespace OTA
    {
        void OTAServerStop();
        void OTAServerLoop();
        void OTAServerInit();
        void WifiBegin();
        void OTAServerBegin();
    }

    namespace FOTA
    {
        void FOTAbegin();
        void Star_update();
    }

    void begin();
}

#endif