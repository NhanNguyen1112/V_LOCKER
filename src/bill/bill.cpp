#include "vhitek.h"

namespace VHITEK
{
    BillValidator Validator;
    namespace bill
    {
        ICTBC_BUS dataICTBus(&Serial1, 1);
        essp dataESSPBus(&Serial1, Master);

        long soTienDaNhan;
        long soTienChuaNhan;

        // void test_tinh_tien();

        void OnCreditCallback(unsigned long value)
        {
            soTienDaNhan += value;
            soTienChuaNhan = 0;
            Serial.println("Vua nhan tien");
            Serial.println(value);
            Serial.println("Tong tien da nhan");
            Serial.println(soTienDaNhan);
        }

        void OnEscrowCreditCallback(unsigned long value)
        {
            soTienChuaNhan = value;
            Serial.println("Ngam tien");
            Serial.println(value);
        }

        void OnBillRejectedCallback(unsigned long value)
        {
            soTienChuaNhan = 0;
            Serial.println("Tra lai tien dang ngam");
            Serial.println(value);
        }

        void begin()
        {
            Validator.ICTDataBus = &dataICTBus;
            Validator.ESSPDataBus = &dataESSPBus;
            // Serial.println("Bill begin");
            Validator.begin(getBVProtocol(5, 18, &Serial1)); 
            Validator.BV->setPayoutValue(VHITEK::save_config_machine.BILL.payout); // set giá tiền thối
            Validator.BV->setBillsMinMax(VHITEK::save_config_machine.BILL.billmin, VHITEK::save_config_machine.BILL.billmax); //Set tiền nhận Min, Max
            Validator.BV->reset();
            Validator.BV->init();
            Validator.BV->on(BV_EVENT_BILL_STACKED_RECLYER, OnCreditCallback);
            Validator.BV->on(BV_EVENT_BILL_STACKED_CASHBOX, OnCreditCallback);
            Validator.BV->on(BV_EVENT_BILL_HOLDING, OnEscrowCreditCallback);
            Validator.BV->on(BV_EVENT_BILL_REJECTED, OnBillRejectedCallback);
            // Serial.println("exit bill begin");
        }

        /*int32_t tinh_tien(time_ship TimeGui, time_ship TimeNhan)
        {
            int32_t total_revenue=0;
            int32_t total_block2_revenue=0;
            int32_t block1minutes = Price_setup.block1_minutes;
            int32_t block1price = Price_setup.block1_price;
            int32_t block2minutes = Price_setup.block2_minutes;
            int32_t block2price = Price_setup.block2_price;

            int32_t timestamp_gui = VHITEK::Ds1307::ToTimeStamp(TimeGui);
            int32_t timestamp_nhan = VHITEK::Ds1307::ToTimeStamp(TimeNhan);
            int32_t Total_TimeStamp = timestamp_nhan - timestamp_gui;
            Serial.printf("Tổng timestamp: %d\n", Total_TimeStamp);

            int32_t total_minutes = Total_TimeStamp/60;
            Serial.printf("Tổng Phút: %d\n", total_minutes);

            int32_t total_block2_minutes = total_minutes - block1minutes;
            Serial.printf("Tổng Phút Block 2: %d\n", total_block2_minutes);

            if(total_block2_minutes>0)
            {
                total_block2_revenue = int(total_block2_minutes/block2minutes)*block2price;
                if(total_block2_minutes % block2minutes > 0)
                {
                    total_block2_revenue = block2price + total_block2_revenue;
                }
            }
            total_revenue = total_block2_revenue + block1price;
            Serial.printf("Tổng tiền: %d\n", total_revenue);
            return total_revenue;
        } */

        /*void test_tinh_tien()
        {
            time_ship Tgui, Tnhan;

            Tgui.ngay=28;
            Tgui.thang=9;
            Tgui.nam=22;
            Tgui.gio=1;
            Tgui.phut=0;
            Tgui.giay=0;

            Tnhan.ngay=28;
            Tnhan.thang=9;
            Tnhan.nam=22;
            Tnhan.gio=6;
            Tnhan.phut=0;
            Tnhan.giay=0;

            Serial.print("Tính tiền: ");
            Serial.println(tinh_tien(Tgui, Tnhan));
        }*/

    }
}