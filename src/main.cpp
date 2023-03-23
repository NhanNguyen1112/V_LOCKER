#include <Arduino.h>
#include <vhitek.h>

namespace VHITEK
{
  SemaphoreHandle_t SPISemaphoreHandle; // SPI bus semaphore
  uint32_t SPISemaphoreHandleTakenTick; // SPI bus semaphore taken tick

  SemaphoreHandle_t I2CSemaphoreHandle; // I2C bus semaphore
  uint32_t I2CSemaphoreHandleTakenTick; // I2C bus semaphore taken tick

  SemaphoreHandle_t I2C1SemaphoreHandle; // I2C1 bus semaphore
  uint32_t I2C1SemaphoreHandleTakenTick; // I2C1 bus semaphore taken tick

  SemaphoreHandle_t MDBSemaphoreHandle;
  uint32_t MDBSemaphoreHandleTakenTick;

  setting_machine save_config_machine;
  cabine_config save_cabine;
  QR QRread;
  MayIN Data_MayIN;
  Music Data_Music;

  uint8_t lastMode = 1;
  uint8_t currentMode = 1; // 0 = menu, 1 = work

  volatile uint64_t ID;
  char ID_Master[13];
  uint8_t tuchuasd[24];

  ExternalEEPROM myMem;
  ExternalEEPROM myMem2;

  thoigian thoi_gian;
  RTC_DS1307 rtc;

  uint32_t IDX_hien_tai;
  uint32_t dia_chi_IDX_hien_tai;

  char apSSID[30];
  String socuoiID;

  String WSSID;
  String bufferGM65;

  boolean check_read_eeprom_1;
  boolean check_read_eeprom_2;
  bool check_send_tong_so_tu;
  bool check_send_card_info;
  bool check_update_FOTA;
  bool check_update_machine;
  bool check_his_send;

  uint16_t Tong_tu_chua_SD;
  uint16_t diachi_giaodich;

  unsigned short cal_crc_loop_CCITT_A(short l, unsigned char *p)
  {
    int i, j;
    unsigned short seed = 0xFFFF;
    unsigned short cd = 0x8005;
    unsigned short crc = seed;

    for (i = 0; i < l; ++i)
    {
      crc ^= (p[i] << 8);
      for (j = 0; j < 8; ++j)
      {
        if (crc & 0x8000)
          crc = (crc << 1) ^ cd;
        else
          crc <<= 1;
      }
    }
    return crc;
  }

  // I2C-1: 2 IC EEPROM
  bool accessI2C1Bus(std::function<void()> &&Callback, uint32_t timeout) // I2C 1
  {
    if (xSemaphoreTake(I2C1SemaphoreHandle, timeout) == pdTRUE)
    {
      Callback();
      xSemaphoreGive(I2C1SemaphoreHandle);
      return true;
    }
    return false;
  }

  // I2C-2: KeyPad; DS1307
  bool accessI2CBus(std::function<void()> &&Callback, uint32_t timeout) // I2C 2
  {
    if (xSemaphoreTake(I2CSemaphoreHandle, timeout) == pdTRUE)
    {
      Callback();
      xSemaphoreGive(I2CSemaphoreHandle);
      return true;
    }
    return false;
  }

  // SPI: LCD
  bool accessSPIBus(std::function<void()> &&Callback, uint32_t timeout)
  {
    if (xSemaphoreTake(SPISemaphoreHandle, timeout) == pdTRUE)
    {
      Callback();
      xSemaphoreGive(SPISemaphoreHandle);
      return true;
    }
    return false;
  }

  bool accessMDBBus(std::function<void()> &&Callback, uint32_t timeout)
  {
    if (xSemaphoreTake(MDBSemaphoreHandle, timeout) == pdTRUE)
    {
      //Serial.println("CALL FUNCTION");
      Callback();
      xSemaphoreGive(MDBSemaphoreHandle);
      return true;
    }
    return false;
  }

  void taskKeypad(void *parameter)
  {
    // Serial.println("Keypad Task Started");
    while (1)
    {
      accessI2CBus([&]{
            VHITEK::Keypad::loop(); // Call keypad loop                   
      }, 100);

      accessI2CBus([&]{
            VHITEK::RFID::loop();               
      }, 100);

      accessI2CBus([&]{
            VHITEK::Ds1307::loop();                    
      }, 100);

      // accessI2CBus(callback, 100); // Try to access the I2C bus with 100ms timeout
      delay(10);
    }
  }

  void menuMode()
  {
    Menu::loop();
  }

  void workMode()
  {
    // VHITEK::mo_tu_locker::mo_tu_locker();
    // VHITEK::mo_tu_locker::locker_button();
    // VHITEK::mo_tu_locker::locker_Barcode();
    // VHITEK::mo_tu_locker::mo_cua_cuon();
    VHITEK::Display::hien_ngay_gio();
  }

  void mainTask(void *parameter)
  {
    while (1)
    {
      if (lastMode != digitalRead(PIN_MENU))
      {
        if (lastMode == 1)
        {
          Menu::begin();
          VHITEK::OTA::OTAServerBegin();
        }
        else
        {
          Menu::end();
          VHITEK::OTA::OTAServerStop();
        }
        lastMode = digitalRead(PIN_MENU);
        currentMode = digitalRead(PIN_MENU);
        delay(500);
      }

      if (currentMode == 0) // menu
      {
        VHITEK::OTA::OTAServerLoop();       
        menuMode();
      }
      else
      { // 1 work
        workMode();
      }
      delay(10);
    }
  }

  void begin() // Setup main
  {
    Serial.begin(115200);
    Wire.begin(25, 26, 400000); // Init I2C cua: KeyPad, DS1307

    Serial1.begin(9600, SERIAL_8N1, 18, 5, false); //BILL
    
    Serial2.begin(115200, SERIAL_8N1, 23, 22, false); //RS485
   
    // Khoi tao Semaphore
    vSemaphoreCreateBinary(I2C1SemaphoreHandle);
    vSemaphoreCreateBinary(I2CSemaphoreHandle);
    vSemaphoreCreateBinary(SPISemaphoreHandle);
    vSemaphoreCreateBinary(MDBSemaphoreHandle);

#ifdef mocua
    pinMode(PIN_PUMP, OUTPUT);
    pinMode(PIN_VAL1, OUTPUT);
    pinMode(PIN_VAL2, OUTPUT);
#else
    pinMode(PIN_595_LATCH, OUTPUT);
    pinMode(PIN_595_DATA, OUTPUT);
    pinMode(PIN_595_CLOCK, OUTPUT);
    pinMode(PIN_165_LATCH, OUTPUT);
    pinMode(PIN_ENC1, INPUT_PULLUP);
#endif

    // ENABLE THE LEVEL SHIFTER (I2C BUS, SERIAL 1 BUS)
    pinMode(PIN_LEVEL_SHIFTER_ENABLE, OUTPUT);
    digitalWrite(PIN_LEVEL_SHIFTER_ENABLE, HIGH); // Always: HIGH

    VHITEK::EEPROM::begin();
    VHITEK::Config::loadChipID();
    Display::setup();
    Keypad::setup();
    RFID::begin();
    Ds1307::begin();
    VHITEK::FOTA::FOTAbegin();
    VHITEK::OTA::WifiBegin();

    // VHITEK::Config::All_Clear_eeprom(1, 64000);
    VHITEK::transaction::load_du_lieu();
    // VHITEK::Config::KT_tong_tu_chua_SD();

    // Start Keypad Task
    xTaskCreateUniversal(taskKeypad, "taskKeypad", 10000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreateUniversal(mainTask, "mainTask", 10000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    // xTaskCreateUniversal(dong_bo_task, "Task_dong_bo", 10000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);

    // Serial.print("IDX: "); Serial.print(IDX_hien_tai);
    // Serial.print(" - Dia chi: "); Serial.println(dia_chi_IDX_hien_tai);

    // Serial.println(sizeof(save_config_machine));
    // Serial.println(sizeof(save_cabine));
  }
}

void setup()
{
  VHITEK::begin();
}

void loop()
{
  DynamicJsonDocument doc(10000);
  static char buffer[200];
  static int ptr = 0;

  auto clear_buffer = [&](){
    for (int i = 0; i < 200; i++)
    {
      buffer[i] = 0;
    }
    ptr = 0;
  };

  while(Serial2.available()) //nhận từ RS485
  {
    int data = Serial2.read();
    if (data == 0x7F) // 7F bắt đầu
    {
      clear_buffer();
      return;
    }
    else if (data == 0x7E) // Kết thúc
    {
      doc.clear();
      DeserializationError error = deserializeJson(doc, buffer);

      if(error == error.Ok)
      {
        int addMas = doc["addMas"].as<int>();
        int add = doc["add"].as<int>();

        Serial.println("READ PORT: ");
        serializeJson(doc, Serial);
        Serial.println();

        if(addMas == 1)
        {
          if(add == VHITEK::save_config_machine.Sub_boar.Add_Module_QR) //Module QR
          {
            VHITEK::QRread.add = add;
            VHITEK::QRread.data = doc["data"].as<String>();
            VHITEK::QRread.check_sum = doc["crc"].as<uint16_t>();
          }
          else if(add == VHITEK::save_config_machine.Sub_boar.Add_Module_Music) //Music
          {

          }
          else if(add == VHITEK::save_config_machine.Sub_boar.Add_Module_Printer) //printer
          {

          }
          else
          {
            clear_buffer();
            return;
          }
        }
      }

      clear_buffer();
      return;
    }
    else 
    {
      if (ptr < 200)
      {
        buffer[ptr] = data;
        ptr++;
      }
      else { clear_buffer(); }
    }
  }
}