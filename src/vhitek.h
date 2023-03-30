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
#include <UnixTime.h>

#ifdef __cplusplus
  extern "C" {
#endif
  uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

uint8_t temprature_sens_read();

//////////////CHỌN CHỨC NĂNG//////////////////

    // #define serverchinh
    #ifdef serverchinh
    #define API ".............."
    #else 
    #define API "159.223.48.4"
    #endif

    // #define mocua //Mở cửa cuốn của CTY
    #ifdef mocua
    #define Use_RFID
    #endif

    // #define Locker_NoiBo //Locker Nội bộ, dùng RFID
    #ifdef Locker_NoiBo
    #define Use_RFID
    #endif

    // #define Locker_Shipping //Locker gửi/nhận hàng, dùng bill, VNP, có music
    #ifdef Locker_Shipping
    #define Use_bill
    #define Use_Music
    #endif

    #define Locker_Ship_Barcode //Locker gửi/nhận hàng dùng 1 nút nhấn, bill, đầu đọc QR + máy in nhiệt
    #ifdef Locker_Ship_Barcode
    #define Use_QR
    #define Use_Printer
    #define Use_Music
    // #define Use_bill
    #endif

    // #define CoSanTien //Có săn tiền không cần bill

//////////// CHỌN LOGO /////////////////////
    #define logoTSE "TSEVENDING"
    #ifndef logoTSE
    #define logoVHITEK "VHITEK.VN"
    #endif

//////////// CẬP NHẬT PHIÊN BẢN /////////////
    #ifdef Locker_NoiBo
        #define BOARD_TYPE  "VLOCKER-V1"
        #define FW_VERSION  "LK0.0.1"
        #define FW_server 1
    #endif

    #if defined(Locker_Shipping) || defined(Locker_Ship_Barcode)
        #define BOARD_TYPE  "VLOCKER-SHIPPING"
        #define FW_VERSION  "LKS0.0.1"
        #define FW_server 1
    #endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MK_khach 123456 //Setup MK cho khach
#define MK_KT 456789    //Setup MK cho ky thuat vien
#define MK_MASS 231223  //Setup MK MASS

#define PIN_MENU 15 //Nut MENU
#define PIN_LEVEL_SHIFTER_ENABLE 13 //Luon de muc HIGH de ENABLE ESP32
#define so_luong_menu 30 //tong so menu

#ifdef mocua
#define PIN_PUMP 12
#define PIN_VAL1 27
#define PIN_VAL2 14
#else
#define PIN_ENC1 33
#endif

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

    typedef struct //đọc trạng thái IO
    {
        uint16_t add;
        uint16_t id;
        uint8_t status;
    }statusIO;
    extern statusIO ReadIO;

    extern uint8_t flag_status; //cờ kiểm tra status
    
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
        uint32_t PW_Control;
        
        char chuaSD1[50];
        uint32_t chuaSD2;
        uint64_t chuaSD3;
        
        uint16_t check_sum;
    } setting_machine;

    typedef struct //cài đặt tủ: kích thước 120 byte; ô bắt đầu lưu 2000
    {
        uint16_t sotu;
        char ID_user[15];
        char mat_khau[10];
        uint8_t size;
        uint8_t send_data;
        uint8_t OTP[8];
        uint8_t sdt_gui[12];
        uint8_t sdt_nhan[12];
        char barcode[15];
        thoigian time_sender;

        char chuaSD[20];
        uint64_t chuaSD2;

        uint16_t check_sum;
    } cabine_config;

    typedef struct //IC2: Transaction; Kích thước: 132 Byte
    {
        uint32_t IDX;
        uint32_t money; 
        char RFID[15];
        char mat_khau[10];
        uint16_t so_tu; 
        uint8_t size_tu;
        uint8_t status_cabine;
        uint8_t send_data;
        uint8_t typepayment; //1 byte: 0-tienmat; 1:VNPAY
        uint8_t sdt_gui[12]; 
        uint8_t sdt_nhan[12]; 
        thoigian time_sender; 
        thoigian time_receive;

        uint8_t chua_SD[50];

        uint16_t check_sum; 
    } cabine_transac;

    extern setting_machine save_config_machine;
    extern cabine_config save_cabine;

    extern cabine_transac last_trans;
    
//////////////////////////////////////////////////////////////////////////////////////////////////////////

    extern RTC_DS1307 rtc;
    extern U8G2_ST7920_128X64_F_HW_SPI u8g2;
    extern volatile uint64_t ID;
    extern uint16_t tuchuasd[200];
    extern uint16_t Tong_tu_chua_SD;
    
    extern char apSSID[30];
    extern String socuoiID;

    extern uint32_t IDX_hien_tai;
    extern uint32_t dia_chi_IDX_hien_tai;

    extern bool check_send_tong_so_tu;
    extern bool check_send_card_info;
    extern bool check_update_machine;
    // extern bool check_his_send;
    extern int server_check_connect;
    extern uint16_t diachi_giaodich;

    unsigned short cal_crc_loop_CCITT_A(short l, unsigned char *p);
    bool accessI2C1Bus(std::function<void()> &&Callback, uint32_t timeout);
    bool accessI2CBus(std::function<void()> &&Callback, uint32_t timeout);
    bool accessSPIBus(std::function<void()> &&Callback, uint32_t timeout);
    bool accessMDBBus(std::function<void()> &&Callback, uint32_t timeout);

    extern BillValidator Validator;
    namespace BILL
    {
        extern long soTienDaNhan;
        extern long soTienChuaNhan;
        void begin();
        int32_t tinh_tien(thoigian TimeGui, thoigian TimeNhan);
    }

    namespace EEPROM
    {
        extern ExternalEEPROM myMem;
        extern ExternalEEPROM myMem2;

        extern bool check_read_eeprom_1_Machine;
        extern bool check_read_eeprom_1_Cabine;
        extern bool check_read_eeprom_2;

        void begin();

        int16_t sumCalc_EEP_1_Machine(setting_machine data);
        bool write_EEP_1_Machine(); //Lưu cài đặt máy
        setting_machine read_EEP_1_Machine(); //Đọc cài đặt máy

        int16_t sumCalc_EEP_1_Cabine(cabine_config data);
        bool write_EEP_1_Cabine(cabine_config user_check); //lưu config tủ
        cabine_config read_EEP_1_Cabine(uint16_t so_tu); //Đọc config tủ

        int16_t sumCalc_eeprom_2(cabine_transac data);
        bool write_eeprom_2(cabine_transac giaodich);
        cabine_transac read_eeprom_2(uint16_t dia_chi);
    }

    namespace ACTION
    {
        extern int CheckSend_Data;
        extern int start_funct;
        void Locker_NB_RFID();  // Locker nội bội dùng RFID
        void mo_cua_cuon();
        void Locker_Ship_BARCODE();
        void Send_His(cabine_transac data);
    }

    namespace transaction
    {
        bool save_trans(uint16_t so_tu, uint8_t cabine_status); //lưu trans
        void load_du_lieu();
    }

    namespace Ds1307
    {
        void begin();
        void loop();
        void xem_thoi_gian();
        uint64_t ToTimeStamp(thoigian thoigian);
    }

    namespace Config
    {
        void begin();
        
        void get_setting_machine(); //đọc cài đặt máy
        bool Save_Set_Machine(); //Lưu setting máy, ô 1000

        void Send_Printer(uint16_t add, uint16_t sotu, String data);
        void Send_Music(uint16_t add, uint16_t sobaihat, uint16_t timeout);

        String Json_His_Shipping(cabine_transac data); //lich su giao dich
        String Json_tong_tu(); //Tao Json tong so tu
        // String Json_thong_tin_tu(user_setting user); //Tao Json thong tin tu
        String Json_machine_status(); //Tao Json tinh trang may
        String Json_vnpay_neworder(uint32_t IDX, int32_t tongtien, int32_t random); //Tạo giao dich VNPAY moi
        String Json_vnpay_check(uint32_t IDX, int32_t random); //Thực hiện thanh toán
        String Json_info_shipper(cabine_config data); // Tao Json luu info shipper
        
        uint16_t tinh_o_luu_the(uint16_t so_tu); //tinh o bat dau luu the theo so tu
        bool kiem_tra_tu_da_co(uint16_t so_tu); //Kiem tra xem so tu nay da luu chua
        int so_sanh_mk_menu(char *mk);
        void All_Clear_eeprom(int eep, uint16_t Max_diachi); // Xoa toan bo IC eeprom luu hanh dong mo cua
        void decodeID(uint16_t sotu_INPUT, uint16_t &id, uint16_t &add);
        bool read_locker(uint16_t sotu); //gửi lệnh đọc trạng thái tủ
        bool openlocker(uint16_t sotu); //Gửi lệnh Mở tủ
        bool Open_IO(uint16_t so_tu); //Hành động mở tủ
        String xuatbarcode(uint16_t sotu);       
        int xuat_sotu(String data);
        void QR_VNP(String QR); //Hiện QR VNP 
        void HT_QR(String QR);

        String loadChipID();
        int get_temp(); //đọc nhiệt độ ESP32
        int Wifi_RSSI(); //RSSI cua Wifi
        void KT_tong_tu_chua_SD(); //Tinh tong so tu chua su dung
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
            doimkmotu,
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
        void TB_2_sotu_bangnhau();
        void TB_ADD_khac_0(); //TB địa chỉ nhập vào phải lớn hơn 0
        void TB_dang_KT_cai_dat();
        void TB_Server_Disconnect(); //TB mất kết nối Server
        void Trang_thanh_toan(String VNPAY, cabine_transac NewTrans);
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
        extern bool check_update_FOTA;
        void FOTAbegin();
        void Star_update();
    }

    void begin();
}

#endif