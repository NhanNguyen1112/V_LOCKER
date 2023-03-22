#include "vhitek.h"

namespace VHITEK
{
    namespace Ds1307
    {
        // RTC_DS1307 rtc;

        void begin()
        {
            // rtc.begin();
            if (! rtc.begin(&Wire)) {
                // Serial.println("KHONG TIM THAY RTC");
                Serial.flush();
                //abort();
            }            
            if (! rtc.isrunning()) {
                // Serial.println("RTC KHONG CHAY, CAI DAT LAI THOI GIAN");
                rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
            }            
            // rtc.adjust(DateTime(2022, 7, 25, 16, 14, 0)); //nam, thang, ngay, gio, phut, giay
        }

        void loop()
        {
            DateTime now = rtc.now();

            thoi_gian.ngay = now.day();
            thoi_gian.thang = now.month();
            thoi_gian.nam = now.year();
            thoi_gian.gio = now.hour();
            thoi_gian.phut = now.minute();
            thoi_gian.giay = now.second();

            // Serial.printf("%02d:%02d:%02d \n", thoi_gian.gio, thoi_gian.phut, thoi_gian.giay);

            // Serial.print(now.year(), DEC);
            // Serial.print('/');
            // Serial.print(now.month(), DEC);
            // Serial.print('/');
            // Serial.print(now.day(), DEC);
            // Serial.print(" - ");
            // // Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
            // // Serial.print(") ");
            // Serial.print(now.hour(), DEC);
            // Serial.print(':');
            // Serial.print(now.minute(), DEC);
            // Serial.print(':');
            // Serial.print(now.second(), DEC);
            // Serial.println();            
        }

        void xem_thoi_gian()
        {
            // Serial.printf("%02d:%02d:%02d | ", thoi_gian.gio, thoi_gian.phut, thoi_gian.giay);
            // Serial.printf("Ngay:%02d Thang:%02d Nam:%02d \n", thoi_gian.ngay, thoi_gian.thang, thoi_gian.nam);
        }
    }
}