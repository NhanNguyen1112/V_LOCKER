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

  char apSSID[30]; //mã máy

  setting_machine save_config_machine;
  cabine_config save_cabine;
  cabine_transac last_trans;
  QR QRread;
  MayIN Data_MayIN;
  Music Data_Music;
  statusIO ReadIO;
  uint8_t flag_status; //cờ kiểm tra status

  uint8_t lastMode = 1;
  uint8_t currentMode = 1; // 0 = menu, 1 = work

  volatile uint64_t ID;
  uint16_t tuchuasd[200];
  uint16_t Tong_tu_chua_SD;

  thoigian thoi_gian;
  RTC_DS1307 rtc;

  uint32_t IDX_hien_tai;
  uint32_t dia_chi_IDX_hien_tai;

  String socuoiID;

  bool check_send_tong_so_tu;
  bool check_send_card_info;
  bool check_update_machine;
  // bool check_his_send;
  int server_check_connect;
  uint16_t Last_DC_GD;

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

  #if defined(Use_bill)
  void Bill(void *parameter)
  {
    while(1)
    {
      accessMDBBus([&]{
        VHITEK::Validator.BV->loop();
      }, 2000);

      delay(200);
    }
  }
  #endif

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

  void working()
  {
    if(VHITEK::server_check_connect>=5) VHITEK::Display::TB_Server_Disconnect();
    else
    {
      #ifdef Locker_NoiBo
      VHITEK::ACTION::Locker_NB_RFID();
      #endif
      
      #ifdef Locker_Ship_Barcode
      VHITEK::ACTION::Locker_Ship_BARCODE(); 
      #endif
    } 
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
      { 
        working();
      }

      delay(10);
    }
  }

  void _Synch_Task(void *parameter)
  {
    DynamicJsonDocument doc(10000);
    cabine_transac trans_read;
    cabine_config UID;
    uint32_t lastTick_FOTA=0;
    uint32_t lastTick_wifi=0;
    uint32_t lastTick_status_machine=0;
    uint32_t lastTick_Cal_Money=0;
    uint32_t lastTick_card_info=0;
    uint32_t diachi=0;

    while(1)
    {
      if (!WiFi.isConnected()) //Kiem  tra ket noi Wifi
      {
        if ((uint32_t)(millis() - lastTick_wifi) > 5000)
        {
          if (WiFi.reconnect())
          {
            esp_wifi_set_ps(WIFI_PS_NONE);
          }
          lastTick_wifi = millis();
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
      
      if(check_send_tong_so_tu == true) //Gui Tong so tu len
      {
        if ((WiFi.status() == WL_CONNECTED))
        {
          char url[1024];
          HTTPClient http;
          sprintf(url, "http://%s:8000/locker/updatetotalcabined", API);
          http.begin(url); //API
          http.addHeader("Content-Type", "application/json");   

          String json_data = VHITEK::Config::Json_tong_tu();    
          int post = http.POST(json_data.c_str());
          // Serial.println(VHITEK::Config::Json_tong_tu().c_str());

          String payload = http.getString();     
          Serial.print("Status Gửi Tổng Tủ: "); Serial.println(payload);  

          if (post == 200)
          {
            DeserializationError error = deserializeJson(doc, payload);
            if (error == 0)
            {
              // Serial.println(doc["status"].as<String>());
              if(doc["status"].as<boolean>() == true) //NEU da gui duoc
              {
                Serial.println("Da gui Tong So Tu");
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

      if((WiFi.status() == WL_CONNECTED)) //Gui trang thai may
      {
        if ( ((uint32_t)(millis() - lastTick_status_machine) > 60000) or check_update_machine == true ) //1p update 1 lan
        {
          char url[1024];
          HTTPClient http;
          sprintf(url, "http://%s:8000/locker/updatestatuslocker", API);
          http.begin(url); //API
          http.addHeader("Content-Type", "application/json");              
          String json_data=VHITEK::Config::Json_machine_status();
          // Serial.println(VHITEK::Config::Json_machine_status().c_str());

          int post = http.POST(json_data.c_str());
          String payload = http.getString();
          Serial.print("Status TTM: "); Serial.println(payload);          

          if (post == 200)  //Check for the returning code
          { 
            server_check_connect=0;
            DeserializationError error = deserializeJson(doc, payload);
            if (error == 0)
            {
              // Serial.println(doc["status"].as<String>());
              if(doc["status"].as<bool>() == true) //NEU da gui duoc
              {
                check_update_machine = false;
                // Serial.println("Đã gửi được trạng thái máy");             
              }
            }
          }
          else 
          {
            server_check_connect++;
            // Serial.printf("Server check: %d\n", server_check_connect);
          }
          http.end();  

          lastTick_status_machine = millis();
        }
      }

      if ((WiFi.status() == WL_CONNECTED))  //GUI lich su giao dich
      {
        if(diachi <= 64000)
        {
          trans_read = VHITEK::EEPROM::read_eeprom_2(diachi);
          if(EEPROM::check_read_eeprom_2 == true)
          {
            if(trans_read.send_data == 1) //Chưa gửi lên Server
            {
              #if defined(Locker_Shipping) || defined(Locker_Ship_Barcode)
              VHITEK::ACTION::Send_His(trans_read, diachi);
              #endif
            }
          }
          diachi+=sizeof(cabine_transac);
        }
        else diachi=0;
      }

      #if defined(Locker_Shipping) || defined(Locker_Ship_Barcode)
      if((WiFi.status() == WL_CONNECTED)) //Doc cai dat gia thue tu Server
      {
        // bill_price_setup Price_read;
        if ( ((uint32_t)(millis() - lastTick_Cal_Money) > 3000) ) //3s check 1 lan
        {
          char url[1024];
          HTTPClient http;
          sprintf(url, "http://%s:8000/shipperlocker/get-calc-money-infor/%s", API, apSSID);
          http.begin(url); //API
          http.addHeader("Content-Type", "application/json");              

          int httpResponseCode = http.GET();
          // Serial.println(apiurl.c_str());       

          if (httpResponseCode == 200)  //Check for the returning code
          { 
            String payload = http.getString();
            // Serial.println(payload);
            DeserializationError error = deserializeJson(doc, payload);
            if (error == 0)
            {
              if(doc["block1minutes"].as<int>()!=save_config_machine.Gia_Thue_Tu.block1_minutes or doc["block1price"].as<int>()!=save_config_machine.Gia_Thue_Tu.block1_price or
                doc["block2minutes"].as<int>()!=save_config_machine.Gia_Thue_Tu.block2_minutes or doc["block2price"].as<int>()!=save_config_machine.Gia_Thue_Tu.block2_price)
              {
                save_config_machine.Gia_Thue_Tu.block1_minutes = doc["block1minutes"].as<int>();
                save_config_machine.Gia_Thue_Tu.block1_price = doc["block1price"].as<int>();
                save_config_machine.Gia_Thue_Tu.block2_minutes = doc["block2minutes"].as<int>();
                save_config_machine.Gia_Thue_Tu.block2_price = doc["block2price"].as<int>(); 

                VHITEK::Config::Save_Set_Machine();

                Serial.println("Da luu cai dat gia moi");
              }        
            }
          }
          else 
          {
            Serial.printf("ERROR Get Giá Thuê: %d\n",httpResponseCode);
          }
          http.end();  

          lastTick_Cal_Money = millis();
        }
      }
      #endif

      #ifdef Locker_NoiBo
      if((WiFi.status() == WL_CONNECTED))  //Gui thong tin tu len Server
      {
        if ((uint32_t)(millis() - lastTick_card_info) > 5000) //5s gui len 1 lan
        {
          char url[1024];
          HTTPClient http;
          sprintf(url, "http://%s:8000/locker/updateinforcard", API);
          http.begin(url); //Specify the URL
          http.addHeader("Content-Type", "application/json");        

          for(int sotu=1; sotu<=save_config_machine.tongtu; sotu++) 
          {
            UID  = EEPROM::read_EEP_1_Cabine(sotu);
            // accessI2C1Bus([&]{
            //       EEPROM::myMem.get(VHITEK::Config::tinh_o_luu_the(sotu), UID); //Doc Check SUM trong eeprom              
            // }, 100);

            if(UID.send_data == 1) //Them tu moi va gui len Server
            {
              String json_data = VHITEK::Config::Json_thong_tin_tu(UID);    
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
                    UID.send_data=0;
                    if(VHITEK::EEPROM::write_EEP_1_Cabine(UID))
                    {
                      // VHITEK::Config::xem_eeprom_tu_bat_ky(sotu); //Xem eeprom khi da luu
                      Serial.println("Đã gửi được TT tủ");     
                    }           
                  }
                }
                http.end();            
              }
            }
            else if(UID.send_data == 2) //Xoa va gui len server
            {
              memset(&UID, 0, sizeof(UID));       
              UID.sotu = sotu;
              UID.ID_user[0] = '0';
              UID.mat_khau[0] = '0';   

              String json_data = VHITEK::Config::Json_thong_tin_tu(UID);    
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
                    // memset(&UID, 0, sizeof(UID));
                    // accessI2C1Bus([&]{
                    //       myMem.put(VHITEK::Config::tinh_o_luu_the(sotu), UID); //Doc Check SUM trong eeprom              
                    // }, 100);
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
      #endif

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
        pinMode(PIN_ENC1, INPUT_PULLUP);
    #endif

    // ENABLE THE LEVEL SHIFTER (I2C BUS, SERIAL 1 BUS)
    pinMode(PIN_LEVEL_SHIFTER_ENABLE, OUTPUT);
    digitalWrite(PIN_LEVEL_SHIFTER_ENABLE, HIGH); // Always: HIGH

    VHITEK::Config::begin();

    // Start Keypad Task
    xTaskCreateUniversal(taskKeypad, "taskKeypad", 10000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreateUniversal(mainTask, "mainTask", 10000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreateUniversal(_Synch_Task, "Task_synch", 10000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    #if defined(Use_bill)
    xTaskCreateUniversal(Bill, "Task_bill", 10000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    #endif

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
  static DynamicJsonDocument doc(10000);
  static char buffer[200];
  static int ptr = 0;

  auto clear_buffer = [&](){
    for (int i = 0; i < 200; i++)
    {
      buffer[i] = 0;
    }
    ptr = 0;
  };

  if(Serial2.available()) //nhận từ RS485
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
        String state;

        Serial.print("READ PORT: ");
        serializeJson(doc, Serial);
        Serial.println();

        if(addMas == 1)
        {
          if(add == VHITEK::save_config_machine.Sub_boar.Add_Module_QR) //Module QR
          {
            #ifdef Use_QR
            if(VHITEK::ACTION::start_funct == 0)
            {
              VHITEK::QRread.add = add;
              VHITEK::QRread.data = doc["data"].as<String>().c_str();
              VHITEK::QRread.check_sum = doc["crc"].as<uint16_t>();
              VHITEK::ACTION::start_funct = 2;
            }
            #endif
          }
          else if(add == VHITEK::save_config_machine.Sub_boar.Add_Module_Music) //Music
          {

          }
          else if(add == VHITEK::save_config_machine.Sub_boar.Add_Module_Printer) //printer
          {
            #ifdef Use_Printer
            // Serial.println(doc["state"].as<String>().c_str());
            state = doc["state"].as<String>().c_str();
            if(state == "OK") VHITEK::ACTION::CheckSend_Data = 1;
            else if(state == "NOPAPER") VHITEK::ACTION::CheckSend_Data = 2;
            #endif
          }
          else //Các board IO
          {
            if(VHITEK::flag_status == 1)
            {
              VHITEK::ReadIO.add = add;
              VHITEK::ReadIO.id = doc["id"].as<uint16_t>();
              VHITEK::ReadIO.status = doc["status"].as<uint8_t>();
              VHITEK::flag_status = 2;
            }
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