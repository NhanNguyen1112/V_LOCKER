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

  Adafruit_Thermal printer(&Serial1);

  password Vpass;

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

  uint16_t Tong_so_tu;
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
    VHITEK::mo_tu_locker::mo_cua_cuon();
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

  void dong_bo_task(void *parameter) //thuc hien cac tac vu dong bo
  {
    DynamicJsonDocument doc(10000);
    hanh_dong data;
    user_setting user_ID;
    static uint32_t lastTick_FOTA=0;
    static uint32_t lastTick=0;
    static uint32_t lastTick_status_machine=0;
    static uint32_t lastTick_card_info=0;
    static uint32_t diachi=0;
    
    while(1)
    {
      if (!WiFi.isConnected()) //Kiem  tra ket noi Wifi
      {
        if ((uint32_t)(millis() - lastTick) > 5000)
        {
          if (WiFi.reconnect())
          {
            esp_wifi_set_ps(WIFI_PS_NONE);
          }
          lastTick = millis();
        }
      }

      if((WiFi.status() == WL_CONNECTED))  //Kiem tra update FOTA
      {
        if((uint32_t)(millis() - lastTick_FOTA) > 60000) //60s kiem tra update 1 lan
        {
          // Serial.println("Kiem tra FOTA");
          VHITEK::FOTA::Star_update();
          lastTick_FOTA = millis();
        }  
      }

      if ((WiFi.status() == WL_CONNECTED))  //GUI lich su hanh dong mo cua len Server
      { 
        if(check_his_send == true) //Nếu có hành động mở cửa
        {
          data = VHITEK::EEPROM::read_eeprom_2(diachi_giaodich); //doc lich su giao dich da luu trong EEPROM 2
          if(check_read_eeprom_2 == true) //Neu doc trong EEPROM ra OK
          {
            /*Serial.print("IDX: "); Serial.print(data.IDX);
            Serial.print(" - Status: "); Serial.print(data.trang_thai);
            Serial.print(" - So tu: "); Serial.print(data.so_tu);
            Serial.print(" - Send data: "); Serial.print(data.send_data_check);
            Serial.print(" - ID: ");
            for(int i=0; i<=12; i++)
            {
                Serial.print(data.ID[i]-'0');
                // Serial.printf("%02X ",giao_dich.ID[i]);
            }            

            Serial.print(" - Ngay: "); Serial.print(data.ngay);
            Serial.print(" - Thang: "); Serial.print(data.thang);
            Serial.print(" - Nam: "); Serial.print(data.nam);
            Serial.print(" | Gio: "); Serial.print(data.gio);
            Serial.print(" - Phut: "); Serial.print(data.phut);
            Serial.print(" - Check Sum: "); Serial.println(data.check_sum);
            delay(3000); */

            if(data.send_data_check == 1) //CHUA gui
            {
              HTTPClient http;
              http.begin("http://159.223.48.4:8000/locker/sethistorytransactionlocker"); //Specify the URL
              http.addHeader("Content-Type", "application/json");              
              String json_data=VHITEK::Config::toJson(data);
              // Serial.println(VHITEK::Config::toJson(data).c_str());

              int post = http.POST(json_data.c_str());
              String payload = http.getString();
              // Serial.println(payload);          
              if (post == 200)  //Check for the returning code
              { 
                DeserializationError error = deserializeJson(doc, payload);
                if (error == 0)
                {
                  // Serial.println(doc["status"].as<String>());
                  if(doc["status"].as<boolean>() == true) //NEU da gui duoc
                  {
                    // Serial.println("Da gui duoc transaction");
                    data.send_data_check = 0;
                    accessI2C1Bus([&]{
                        myMem2.put(diachi+7, data.send_data_check); //cap nhat lai Data send check
                    }, 100);    
                    check_his_send = false;                
                  }
                }
              }
              http.end();     
            }
          }
        }
        else 
        {
          if(diachi <= 64000)
          {
            data = VHITEK::EEPROM::read_eeprom_2(diachi); //doc lich su giao dich da luu trong EEPROM 2
            if(check_read_eeprom_2 == true) //Neu doc trong EEPROM ra OK
            {
              /*Serial.print("IDX: "); Serial.print(data.IDX);
              Serial.print(" - Status: "); Serial.print(data.trang_thai);
              Serial.print(" - So tu: "); Serial.print(data.so_tu);
              Serial.print(" - Send data: "); Serial.print(data.send_data_check);
              Serial.print(" - ID: ");
              for(int i=0; i<=12; i++)
              {
                  Serial.print(data.ID[i]-'0');
                  // Serial.printf("%02X ",giao_dich.ID[i]);
              }            

              Serial.print(" - Ngay: "); Serial.print(data.ngay);
              Serial.print(" - Thang: "); Serial.print(data.thang);
              Serial.print(" - Nam: "); Serial.print(data.nam);
              Serial.print(" | Gio: "); Serial.print(data.gio);
              Serial.print(" - Phut: "); Serial.print(data.phut);
              Serial.print(" - Check Sum: "); Serial.println(data.check_sum);
              delay(3000);*/
              if(data.send_data_check == 1) //CHUA gui
              {
                HTTPClient http;
                http.begin("http://159.223.48.4:8000/locker/sethistorytransactionlocker"); //Specify the URL
                http.addHeader("Content-Type", "application/json");              
                String json_data=VHITEK::Config::toJson(data);
                // Serial.println(VHITEK::Config::toJson(data).c_str());

                int post = http.POST(json_data.c_str());
                String payload = http.getString();
                // Serial.println(payload);

                if (post == 200)  //Check for the returning code
                { 
                  DeserializationError error = deserializeJson(doc, payload);
                  if (error == 0)
                  {
                    // Serial.println(doc["status"].as<String>());
                    if(doc["status"].as<boolean>() == true) //NEU da gui duoc
                    {
                      // Serial.println("Da gui duoc transaction");
                      // Serial.println(doc["status"].as<String>());
                      data.send_data_check = 0;
                      accessI2C1Bus([&]{
                          myMem2.put(diachi+7, data.send_data_check); //cap nhat lai Data send check
                      }, 100);                    
                    }
                  }
                }
                http.end();  
              }
            }
            diachi+=sizeof(hanh_dong);  
          }
          else diachi=0;
        }
      }

      if(check_send_tong_so_tu == true) //Gui Tong so tu len
      {
        if ((WiFi.status() == WL_CONNECTED))
        {
          HTTPClient http;
          http.begin("http://159.223.48.4:8000/locker/updatetotalcabined"); //Specify the URL
          http.addHeader("Content-Type", "application/json");   

          String json_data = VHITEK::Config::Json_tong_tu();    
          int post = http.POST(json_data.c_str());
          String payload = http.getString();       
          if (post == 200)
          {
            DeserializationError error = deserializeJson(doc, payload);
            if (error == 0)
            {
              // Serial.println(doc["status"].as<String>());
              if(doc["status"].as<boolean>() == true) //NEU da gui duoc
              {
                // Serial.println("Da gui Tong So Tu");
                check_send_tong_so_tu = false;              
              }
              else 
              {
                check_send_tong_so_tu = true; 
              }
            }     
          }
          http.end();   
        }
      }

      if((WiFi.status() == WL_CONNECTED))  //Gui thong tin tu len Server
      {
        if ((uint32_t)(millis() - lastTick_card_info) > 2000) //5s gui len 1 lan
        {
          HTTPClient http;
          http.begin("http://159.223.48.4:8000/locker/updateinforcard"); //Specify the URL
          http.addHeader("Content-Type", "application/json");        

          for(int sotu=1; sotu<=Tong_so_tu; sotu++) 
          {
            accessI2C1Bus([&]{
                    myMem.get(VHITEK::Menu::tinh_o_luu_the(sotu), user_ID); //Doc Check SUM trong eeprom              
            }, 100);

            if(user_ID.send_data_check == 1) //Them tu moi va gui len Server
            {
              String json_data = VHITEK::Config::Json_thong_tin_tu(user_ID);    
              int post = http.POST(json_data.c_str());
              String payload = http.getString();     
              if (post == 200)  //Check for the returning code
              { 
                DeserializationError error = deserializeJson(doc, payload);
                if (error == 0)
                {
                  // Serial.println(doc["status"].as<String>());
                  if(doc["status"].as<boolean>() == true) //NEU da gui duoc
                  {     
                    user_ID.send_data_check=0;
                    if(VHITEK::EEPROM::write_eeprom_1(user_ID))
                    {
                      // VHITEK::Config::xem_eeprom_tu_bat_ky(sotu); //Xem eeprom khi da luu
                      // Serial.println("Đã gửi được: Thêm mới");     
                    }
                    // else{} // Serial.println("KHÔNG gửi được: Thêm mới");               
                  }
                }
                http.end();            
              }
            }

            if(user_ID.send_data_check == 2) //Xoa va gui len server
            {
              memset(&user_ID, 0, sizeof(user_ID));       
              user_ID.so_tu = sotu;
              user_ID.ID_user[0] = '0';
              user_ID.mat_khau[0] = '0';   

              String json_data = VHITEK::Config::Json_thong_tin_tu(user_ID);    
              int post = http.POST(json_data.c_str());
              String payload = http.getString();  
              // Serial.println(VHITEK::Config::Json_thong_tin_tu(user_ID).c_str());

              if (post == 200)  //Check for the returning code
              { 
                DeserializationError error = deserializeJson(doc, payload);
                if (error == 0)
                {
                  // Serial.println(doc["status"].as<String>());
                  if(doc["status"].as<boolean>() == true) //NEU da gui duoc
                  {     
                    memset(&user_ID, 0, sizeof(user_ID));
                    accessI2C1Bus([&]{
                          myMem.put(VHITEK::Menu::tinh_o_luu_the(sotu), user_ID); //Doc Check SUM trong eeprom              
                    }, 100);
                    // VHITEK::Config::xem_eeprom_tu_bat_ky(sotu); //Xem eeprom khi da luu
                    // Serial.println("Đã gửi được: XÓA");                                                  
                  }
                }
              }
              http.end(); 
            }

          } //END For
          lastTick_card_info = millis();
        }
      }

      if((WiFi.status() == WL_CONNECTED)) //Gui trang thai may
      {
        if ( ((uint32_t)(millis() - lastTick_status_machine) > 1000) or check_update_machine == true ) //60s update 1 lan
        {
          HTTPClient http;
          http.begin("http://159.223.48.4:8000/locker/updatestatuslocker"); //Specify the URL
          http.addHeader("Content-Type", "application/json");              
          String json_data=VHITEK::Config::Json_machine_status();
          // Serial.println(VHITEK::Config::Json_machine_status().c_str());

          int post = http.POST(json_data.c_str());
          String payload = http.getString();
          // Serial.println(payload);          

          if (post == 200)  //Check for the returning code
          { 
            DeserializationError error = deserializeJson(doc, payload);
            if (error == 0)
            {
              // Serial.println(doc["status"].as<String>());
              if(doc["status"].as<boolean>() == true) //NEU da gui duoc
              {
                check_update_machine = false;
                // Serial.println("Đã gửi được trạng thái máy");             
              }
            }
          }
          http.end();  

          lastTick_status_machine = millis();
        }
      }

      delay(10);
    }
  } //End void dong bo

  void begin() // Setup main
  {
    Serial.begin(115200);

    //Serial2.begin(baud-rate, protocol, RX pin, TX pin);
    Serial2.begin(115200, SERIAL_8N1, 5, 18, false); //QR
    
    Serial1.begin(9600, SERIAL_8N1, 23, 22, false); //In nhiet
    printer.begin();

    Wire.begin(25, 26, 100000); // Init I2C cua: KeyPad, DS1307

    // Khoi tao Semaphore
    vSemaphoreCreateBinary(I2C1SemaphoreHandle);
    vSemaphoreCreateBinary(I2CSemaphoreHandle);
    vSemaphoreCreateBinary(SPISemaphoreHandle);

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
    VHITEK::Config::doc_so_cuoi_ID_MAY();

    Display::setup();
    Keypad::setup();
    RFID::begin();
    Ds1307::begin();
    VHITEK::FOTA::FOTAbegin();
    VHITEK::OTA::WifiBegin();

    // VHITEK::Config::All_Clear_eeprom(0, 64000);
    VHITEK::Config::cai_dat_mat_khau(); //Neu chua luu MK de vai menu thi se tu luu

    // VHITEK::transaction::load_du_lieu();
    VHITEK::Config::doc_tong_so_tu();
    // VHITEK::Config::KT_tong_tu_chua_SD();

    // Start Keypad Task
    xTaskCreateUniversal(taskKeypad, "taskKeypad", 10000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreateUniversal(mainTask, "mainTask", 10000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    // xTaskCreateUniversal(dong_bo_task, "Task_dong_bo", 10000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);

    // Serial.print("IDX: "); Serial.print(IDX_hien_tai);
    // Serial.print(" - Dia chi: "); Serial.println(dia_chi_IDX_hien_tai);
  }
}

void setup()
{
  VHITEK::begin();
}

void loop()
{
  // delay(2000);
  // String gatewayIP = WiFi.gatewayIP().toString();
  // Serial.println(gatewayIP);

  // if(VHITEK::mo_tu_locker::read_locker_status(1)==1)
    // Serial.println("mo tu");
  
  // Serial.println(VHITEK::mo_tu_locker::read_locker_status(8));

  // VHITEK::Config::Test_led();
  // for(int i=8; i<=VHITEK::Tong_so_tu; i++)
  // {
  //   // Serial.printf("Tu: %d, Status: %d\n", i, VHITEK::mo_tu_locker::locker_open(i));
  //   Serial.printf("Tu: %d, Status: %d\n", i, VHITEK::mo_tu_locker::read_locker_status(i));
  //   delay(1000);
  // }
  // delay(3000);
  // Serial.println(VHITEK::ID);
}