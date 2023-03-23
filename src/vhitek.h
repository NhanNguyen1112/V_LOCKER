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
#include "validatorHelper.h"
#include <BillValidator.h>
#include "Adafruit_Thermal.h"

//////////////CHỌN CHỨC NĂNG//////////////////
#define mocua
#define Locker_NoiBo
#define Locker_Shipping

#define Use_RFID
#define Use_QR
#define Use_Printer
#define Use_Music
/////////////////////////////////////////////

#define MK_khach 123456 //Setup MK cho khach
#define MK_KT 456789    //Setup MK cho ky thuat vien
#define MK_MASS 231223  //Setup MK MASS

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

#define logoTSE "TSEVENDING" //10,60
#define logoVHITEK "VHITEK.VN" //35,60

#define so_luong_menu 30 //tong so menu

#define FW_TYPE  "VLOCKER-V1"
#define FW  "LK0.0.1"
#define FW_server 1

namespace VHITEK
{
    typedef struct 
    {
        uint16_t add;
        String data;
        uint16_t check_sum;
    } QR;
    extern QR QRread;

    typedef struct
    {
        uint16_t add;
        uint16_t ID;
        String Barcode;
        uint16_t checksum;
    } MayIN;
    extern MayIN Data_MayIN;

    typedef struct
    {
        uint16_t add;
        uint16_t id;
        uint32_t timer;
    } Music;
    extern Music Data_Music;
    ////////////////////////////////////////////////////////////

    typedef struct 
    {
        uint8_t ngay;
        uint8_t thang;
        uint16_t nam;
        uint8_t gio;
        uint8_t phut;
        uint8_t giay;
    } thoigian;
    extern thoigian thoi_gian;

    typedef struct //lưu total money, số lần giao dịch
    {
        uint32_t Total_Sale;
        uint32_t Total_Money;

        thoigian Time_sale_day;
        uint32_t Total_Sale_Day;
        uint32_t Total_Money_Day;

        uint32_t Total_Sale_Month;
        uint32_t Total_Money_Month;

        uint32_t Total_Sale_week;
        uint32_t Total_Money_Week;

    }Sale_set;

    typedef struct //cài đặt bill tiền
    {
        uint32_t billmin=0; 
        uint32_t billmax=0;
        uint32_t payout=0;
        uint64_t chuaSD1;
        uint64_t chuaSD2;
    } billset;

    typedef struct //giá thuê tủ
    {
        uint32_t block1_price;
        uint32_t block1_minutes;
        uint32_t block2_price;
        uint32_t block2_minutes;
    } bill_price_setup;

    typedef struct //cài đặt các board phụ: board IO, QR, in nhiệt, music.
    {
        uint16_t tongboard; //Chưa sử dụng
        uint8_t Add_Board_IO[100];
        uint8_t Add_Module_QR;
        uint8_t Add_Module_Printer;
        uint8_t Add_Module_Music;

        uint64_t chua_su_dung1;
        char chua_su_dung2[100];
    }Setting_Board_SUB;

    typedef struct //cài đặt máy: kích thước 816 byte; lưu ô 1000
    {
        uint16_t tongtu;
        uint8_t Tu_CSD[200];
        uint8_t error[200];

        Setting_Board_SUB Sub_boar;
        billset BILL;
        char ID_mas[15];
        Sale_set Sale_total;
        uint8_t timerQR;
        bill_price_setup Gia_Thue_Tu;
        
        char chuaSD1[50];
        uint64_t chuaSD2;
        uint64_t chuaSD3;
        
        uint16_t check_sum;
    } setting_machine;

    typedef struct //cài đặt tủ: kích thước 120 byte; ô bắt đầu lưu 2000
    {
        uint16_t sotu;
        char ID_user[13];
        uint8_t mat_khau[10];
        uint8_t size;
        uint8_t send_data;
        uint8_t OTP[8];
        uint8_t sdt_gui[12];
        uint8_t sdt_nhan[12];
        uint8_t barcode[15];
        thoigian time_sender;

        char chuaSD[20];
        uint64_t chuaSD2;

        uint16_t check_sum;
    } cabine_config;

    extern setting_machine save_config_machine;
    extern cabine_config save_cabine;
    
//////////////////////////////////////////////////////////////////////////////////////////////////////////
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

    extern uint16_t Tong_tu_chua_SD;
    extern uint16_t diachi_giaodich;

    unsigned short cal_crc_loop_CCITT_A(short l, unsigned char *p);
    bool accessI2C1Bus(std::function<void()> &&Callback, uint32_t timeout);
    bool accessI2CBus(std::function<void()> &&Callback, uint32_t timeout);
    bool accessSPIBus(std::function<void()> &&Callback, uint32_t timeout);
    bool accessMDBBus(std::function<void()> &&Callback, uint32_t timeout);

    extern BillValidator Validator;
    namespace bill
    {
        extern long soTienDaNhan;
        extern long soTienChuaNhan;
        void begin();
    }

    namespace EEPROM
    {
        void begin();

        int16_t sumCalc_EEP_1_Machine(setting_machine data);
        bool write_EEP_1_Machine(setting_machine user_check); //Lưu cài đặt máy
        setting_machine read_EEP_1_Machine(); //Đọc cài đặt máy

        int16_t sumCalc_EEP_1_Cabine(cabine_config data);
        bool write_EEP_1_Cabine(cabine_config user_check); //lưu config tủ
        cabine_config read_EEP_1_Cabine(uint16_t so_tu); //Đọc config tủ

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
        setting_machine get_setting_machine(); //đọc cài đặt máy
        bool Save_Set_Machine(); //Lưu setting máy, ô 1000

        void Send_Printer(uint16_t add, uint16_t sotu, String data);
        void Send_Music(uint16_t add, uint16_t sobaihat, uint16_t timeout);

        String toJson(hanh_dong data);
        String Json_tong_tu(); //Tao Json tong so tu
        String Json_thong_tin_tu(user_setting user); //Tao Json thong tin tu
        String Json_machine_status(); //Tao Json tinh trang may
        
        uint16_t tinh_o_luu_the(uint16_t so_tu); //tinh o bat dau luu the theo so tu
        bool kiem_tra_tu_da_co(uint16_t so_tu); //Kiem tra xem so tu nay da luu chua
        int so_sanh_mk_menu(char *mk);
        void xem_eeprom_tu_bat_ky(int so_tu);
        void xem_tung_o();
        void All_Clear_eeprom(int eep, uint16_t Max_diachi); // Xoa toan bo IC eeprom luu hanh dong mo cua
        
        String loadChipID();

        void Test_led();
        int Wifi_RSSI(); //RSSI cua Wifi
        uint16_t KT_tong_tu_chua_SD(); //Tinh tong so tu chua su dung
    }

    namespace RFID
    {
        void begin();
        void loop();
        // uint64_t read_id_card();
    }

    namespace Menu
    {
        enum MENU_IDX
        {
            menuMainId = 0,
            caisizetu,
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
            setbillminmax,
            setpayout,
            fillrecycler,
            giathuetu,
            kt_rtc,
            kt_bonho,
            add_BoardIO,
            addQR,
            addprint,
            addmusic,
            kt_QR,
            kt_print,
            kt_music,
        };

        void loop();
        void begin();
        void end();
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
        void hienthiLOGO(); //hiện logo TSE or VHITEK
        void TB_2_sotu_bangnhau();
        void TB_ADD_khac_0(); //TB địa chỉ nhập vào phải lớn hơn 0
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