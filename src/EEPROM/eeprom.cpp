#include "vhitek.h"

namespace VHITEK
{
    namespace EEPROM
    {
        void begin()
        {
            Wire1.begin(16, 17,400000); // IC EEPROM U10: 0x51, U8: 0x53
            if(myMem2.begin(0x53,Wire1)==false)
            {
                Serial.println("No memory detected. Freezing.");
                // while (1);      
            }
            if (myMem.begin(0x51, Wire1) == false)
            {
                Serial.println("No memory detected. Freezing.");
                // while (1);
            }            
        }

        int16_t sumCalc_EEP_1_Machine(setting_machine data)
        {
            int len = sizeof(data);
            uint8_t buffer[1000];
            uint8_t *ptr = (uint8_t *)&data;
            uint8_t dataSize = 0;
            for (int i = 0; i < len; i++)
            {
                if ((uint32_t)&data.check_sum <= (uint32_t)&ptr[i])
                {
                    break;
                }
                buffer[i] = ptr[i];
                dataSize++;
            }
            uint16_t sum = cal_crc_loop_CCITT_A(dataSize, buffer);
            return sum;
        }  
        bool write_EEP_1_Machine(setting_machine user_check) //Lưu cài đặt máy
        {
            setting_machine read_check_sum;
            byte check_dung=0;
            
            user_check.check_sum=sumCalc_EEP_1_Machine(user_check);
            accessI2C1Bus([&]{
                    myMem.put(1000, user_check); //luu vao eeprom             
            }, 100);    

            accessI2C1Bus([&]{
                    myMem.get(1000, read_check_sum); //Doc check dum duoc luu trong EEPROM            
            }, 100);  
            // Serial.print("SUM EEPROM: "); Serial.println(read_check_sum.check_sum);

            for(int i=0; i<3; i++) //sao sanh 3 lan
            {
                if(user_check.check_sum == read_check_sum.check_sum) //neu Sum nhap voa va sum luu bang nhau
                {
                    check_dung ++;
                }
                else
                {
                    check_dung = 0;
                    break;
                } 
            }
            // Serial.print("Check Dung: "); Serial.println(check_dung);

            if(check_dung == 3) //neu dung
            {
                // Serial.print("Check Dung: "); Serial.println(check_dung);
                // VHITEK::Config::xem_eeprom_tu_bat_ky(user_check.so_tu);
                // VHITEK::Config::xem_tung_o();
                return true;
            }
            else //SAI -> ghi lai -> check lai
            {
                user_check.check_sum=sumCalc_EEP_1_Machine(user_check);
                accessI2C1Bus([&]{
                        myMem.put(1000, user_check); //luu vao eeprom              
                }, 100);    

                accessI2C1Bus([&]{
                        myMem.get(1000, read_check_sum); //Doc check dum duoc luu trong EEPROM             
                }, 100);    

                if(user_check.check_sum == read_check_sum.check_sum) //neu dung
                {
                    return true;
                    // Serial.println("DUNG - da check: ");
                    // VHITEK::Config::xem_eeprom_tu_bat_ky(user_check.so_tu);
                    // VHITEK::Config::xem_tung_o();
                }         
                else //sai -> xoa luon
                {
                    // Serial.println(".........SAI...........");
                    memset(&read_check_sum, 0, sizeof(read_check_sum));
                    accessI2C1Bus([&]{
                            myMem.put(1000, read_check_sum); //luu vao eeprom              
                    }, 100);        

                    // VHITEK::Config::xem_eeprom_tu_bat_ky(user_check.so_tu);
                    // VHITEK::Config::xem_tung_o();

                    return false;
                }  
            }          
        }
        setting_machine read_EEP_1_Machine() //Đọc cài đặt máy
        {
            uint16_t Sum_cal;
            setting_machine user_read_eeprom;

            accessI2C1Bus([&]{
                    myMem.get(1000, user_read_eeprom); //Doc Check SUM trong eeprom              
            }, 100);

            Sum_cal = sumCalc_EEP_1_Machine(user_read_eeprom); //tinh CHECK SUM vua doc

            if(Sum_cal == user_read_eeprom.check_sum) //Neu check SUM dung
            {
                // check_read_eeprom_1 = true;
                return user_read_eeprom;
            }
            // else check_read_eeprom_1 = false;

            // Serial.printf("So tu: %d - SUM_cal: %d - SUM eeprom: %d\n", so_tu, Sum_cal, user_read_eeprom.check_sum);
        }

        int16_t sumCalc_EEP_1_Cabine(cabine_config data)
        {
            int len = sizeof(data);
            uint8_t buffer[200];
            uint8_t *ptr = (uint8_t *)&data;
            uint8_t dataSize = 0;
            for (int i = 0; i < len; i++)
            {
                if ((uint32_t)&data.check_sum <= (uint32_t)&ptr[i])
                {
                    break;
                }
                buffer[i] = ptr[i];
                dataSize++;
            }
            uint16_t sum = cal_crc_loop_CCITT_A(dataSize, buffer);
            return sum;
        }
        bool write_EEP_1_Cabine(cabine_config user_check) //lưu config tủ
        {
            cabine_config read_check_sum;
            byte check_dung=0;

            static uint16_t o_luu;
            o_luu = VHITEK::Config::tinh_o_luu_the(user_check.sotu);
            
            user_check.check_sum=sumCalc_EEP_1_Cabine(user_check);
            accessI2C1Bus([&]{
                    myMem.put(o_luu, user_check); //luu vao eeprom             
            }, 100);    

            accessI2C1Bus([&]{
                    myMem.get(o_luu, read_check_sum); //Doc check dum duoc luu trong EEPROM            
            }, 100);  
            // Serial.print("SUM EEPROM: "); Serial.println(read_check_sum.check_sum);

            for(int i=0; i<3; i++) //sao sanh 3 lan
            {
                if(user_check.check_sum == read_check_sum.check_sum) //neu Sum nhap voa va sum luu bang nhau
                {
                    check_dung ++;
                }
                else
                {
                    check_dung = 0;
                    break;
                } 
            }
            // Serial.print("Check Dung: "); Serial.println(check_dung);

            if(check_dung == 3) //neu dung
            {
                // Serial.print("Check Dung: "); Serial.println(check_dung);
                // VHITEK::Config::xem_eeprom_tu_bat_ky(user_check.so_tu);
                // VHITEK::Config::xem_tung_o();
                return true;
            }
            else //SAI -> ghi lai -> check lai
            {
                user_check.check_sum=sumCalc_EEP_1_Cabine(user_check);
                accessI2C1Bus([&]{
                        myMem.put(o_luu, user_check); //luu vao eeprom              
                }, 100);    

                accessI2C1Bus([&]{
                        myMem.get(o_luu, read_check_sum); //Doc check dum duoc luu trong EEPROM             
                }, 100);    

                if(user_check.check_sum == read_check_sum.check_sum) //neu dung
                {
                    return true;
                    // Serial.println("DUNG - da check: ");
                    // VHITEK::Config::xem_eeprom_tu_bat_ky(user_check.so_tu);
                    // VHITEK::Config::xem_tung_o();
                }         
                else //sai -> xoa luon
                {
                    // Serial.println(".........SAI...........");
                    memset(&read_check_sum, 0, sizeof(read_check_sum));
                    accessI2C1Bus([&]{
                            myMem.put(o_luu, read_check_sum); //luu vao eeprom              
                    }, 100);        

                    // VHITEK::Config::xem_eeprom_tu_bat_ky(user_check.so_tu);
                    // VHITEK::Config::xem_tung_o();

                    return false;
                }  
            }          
        }
        cabine_config read_EEP_1_Cabine(uint16_t so_tu) //Đọc config tủ
        {
            uint16_t Sum_cal;
            cabine_config user_read_eeprom;
            uint16_t o_luu = VHITEK::Config::tinh_o_luu_the(so_tu);

            accessI2C1Bus([&]{
                    myMem.get(o_luu, user_read_eeprom); //Doc Check SUM trong eeprom              
            }, 100);

            Sum_cal = sumCalc_EEP_1_Cabine(user_read_eeprom); //tinh CHECK SUM vua doc

            if(Sum_cal == user_read_eeprom.check_sum) //Neu check SUM dung
            {
                // check_read_eeprom_1 = true;
                return user_read_eeprom;
            }
            // else check_read_eeprom_1 = false;

            // Serial.printf("So tu: %d - SUM_cal: %d - SUM eeprom: %d\n", so_tu, Sum_cal, user_read_eeprom.check_sum);
        }

//////////////////////// EEPROM 2 ////////////////////////////////

        int16_t sumCalc_eeprom_2(hanh_dong data)
        {
            int len = sizeof(data);
            uint8_t buffer[50];
            uint8_t *ptr = (uint8_t *)&data;
            uint8_t dataSize = 0;
            for (int i = 0; i < len; i++)
            {
                if ((uint32_t)&data.check_sum <= (uint32_t)&ptr[i])
                {
                    break;
                }
                buffer[i] = ptr[i];
                dataSize++;
            }
            uint16_t sum = cal_crc_loop_CCITT_A(dataSize, buffer);
            return sum;
        } 
        bool write_eeprom_2(hanh_dong giaodich)
        {
            hanh_dong read_check_sum;
            byte check_dung=0;

            giaodich.check_sum = sumCalc_eeprom_2(giaodich); //tinh check sum cua hanh_dong
            accessI2C1Bus([&]{
                    myMem2.put(dia_chi_IDX_hien_tai, giaodich); //luu vao eeprom             
            }, 100); 
            accessI2C1Bus([&]{ //Doc de lay ID cua tu 
                    myMem2.get(dia_chi_IDX_hien_tai, read_check_sum);        
            }, 100);                           

            for(int i=0; i<3; i++) //sao sanh 3 lan
            {
                if(giaodich.check_sum == read_check_sum.check_sum) //neu Sum nhap voa va sum luu bang nhau
                {
                    check_dung ++;
                }
                else
                {
                    check_dung = 0;
                    break;
                } 
            }

            if(check_dung == 3) //neu dung
            {
                // Serial.print("Check Dung: "); Serial.println(check_dung);
                // VHITEK::Config::xem_eeprom_tu_bat_ky(user_check.so_tu);
                // VHITEK::Config::xem_tung_o();
                return true;
            }
            else 
            {
                giaodich.check_sum = sumCalc_eeprom_2(giaodich); //tinh check sum cua hanh_dong
                accessI2C1Bus([&]{
                        myMem2.put(dia_chi_IDX_hien_tai, giaodich); //luu vao eeprom             
                }, 100); 
                accessI2C1Bus([&]{ //Doc de lay ID cua tu 
                        myMem2.get(dia_chi_IDX_hien_tai, read_check_sum);        
                }, 100);      

                if(giaodich.check_sum == read_check_sum.check_sum) //neu dung
                {
                    return true;
                }          
                else 
                {
                    // Serial.println(".........SAI...........");  
                    memset(&read_check_sum, 0, sizeof(read_check_sum));
                    accessI2C1Bus([&]{
                            myMem2.put(dia_chi_IDX_hien_tai, read_check_sum); //luu vao eeprom              
                    }, 100);        

                    return false;               
                }  
            }
        }
        hanh_dong read_eeprom_2(uint16_t dia_chi)
        {
            uint16_t Sum_cal;
            hanh_dong read_check_sum;

            accessI2C1Bus([&]{
                    myMem2.get(dia_chi, read_check_sum); //Doc Check SUM trong eeprom              
            }, 100);           

            Sum_cal = sumCalc_eeprom_2(read_check_sum); //tinh CHECK SUM vua doc 
            // Serial.print(" - SUM cal: "); Serial.println(Sum_cal);

            if(Sum_cal == read_check_sum.check_sum) //Neu check SUM dung
            {
                check_read_eeprom_2 = true;
                return read_check_sum;
            }
            else check_read_eeprom_2 = false;
        }

    }
}