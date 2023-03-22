#ifndef _BRV3_H_
#define _BRV3_H_
#include "BillValidator.h"
#include "validatorHelper.h"
#include "protocol/ICTBC/ICTBC.h"
#define ICTBC_ERROR_PACKET_LENGHT -1
#define ICTBC_ERROR_PACKET_CHECKSUM -2
#define ICTBC_ERROR_PACKET_TIMEOUT -3
#define ICTBC_ERROR_NACK -4

#define ICTBC_RECEIVE_PACKET_DATA_OFFSET 5
#define ICTBC_RECEIVE_PACKET_DATA_ACK 1
#define ICTBC_RECEIVE_PACKET_DATA_REPLYCMD 0
#define ICTBC_RECEIVE_PACKET_DATA_NACK 2

#define ICTBC_POLL_EVENT_BILL_ESCROW 0
#define ICTBC_POLL_EVENT_BILL_STACKED_CASHBOX 1
#define ICTBC_POLL_EVENT_BILL_STACKED_RECYCLER 2
#define ICTBC_POLL_EVENT_BILL_WAITING 3
#define ICTBC_POLL_EVENT_BILL_RETURNED 4
#define ICTBC_POLL_EVENT_BILL_TO_CASHBOX 5
#define ICTBC_POLL_EVENT_FAULT 0xff
class BRV3 : public BillValidatorBaseClass
{
private:
    ICTBC_BUS *dataBus;
    bool isInitDone;
    int lastMessageCnt;
    unsigned int _featureLevel;
    unsigned int _countryCode;
    unsigned int _billScaleFactor;
    unsigned int _decimalPlaces;
    unsigned int _stackerCapacity;
    unsigned int _payoutCapacity;
    unsigned int _payoutBillPostion;
    unsigned int _billSecurityLevels;
    unsigned int _totalBillInPayout;
    bool _escrowEnabled;
    int32_t _min = 0;
    uint32_t _payoutVal;
    int32_t _max = 0;
    int32_t _billHoldingVal = 0;
    int32_t _billTypeCredit[16];
    int32_t _billsAccept[16];
    _BV_CONFIG_TYPE config;
    void _sendACK();
    void _sendNAK();
    int _TransferStatus();
    int _PayoutStatus();
    void _setAcceptNotes(int32_t *notes, uint8_t totalNotes);
    int _getBillEnable(uint8_t *highByte, uint8_t *lowByte);
    int _setBillEnable(uint8_t highByte, uint8_t lowByte);
    void _pollPraser(uint8_t evnt, uint8_t data);
    int _Poll();
    int _setBillPayoutValue(int32_t value);
    int _payoutBills(uint8_t totalNotes);
    int _countBillInPayout();
    int32_t _getBillTypeCredit(uint8_t channel);
    int _setReclyerCapacity();
    bool isFillReclyer = false;

public:
    BRV3(ICTBC_BUS *bus);
    virtual ~BRV3();
    virtual void setUsingBills(uint32_t *bills, uint8_t totalBills);
    virtual void setBillsMinMax(uint32_t min, uint32_t max); //set nhận tiền min-max
    // virtual void on(uint8_t event, uint32_t data);
    virtual bool init();
    virtual void loop(); // khi nao cần bill hoạt động thì gọi
    virtual int enableEsCrow();
    virtual int disableEsCrow();
    virtual int acceptBill(); // chuyển tiền từ đầu ngậm tiền sang hộp chứa
    virtual int rejectBill(); //trả lại tiền đang ngậm
    virtual int reset(); //reset cứng đầu đọc
    virtual int setBillPayoutValue(uint32_t value);
    virtual int countBillInPayout(int16_t *billCount); //số tờ trong hộp tiền thối
    virtual int payoutValue(int32_t amount, int32_t *totalPayout);
    virtual int emptyReclyer(); //xả tiền thối vô hộp chưa
    virtual int payoutStatus();
    virtual void startFillRecycler(); //bật chế độ nạp tiền thối, theo setPayoutValue
    virtual void endFillRecycler(); //tắt chế độ nạp tiền thối
    virtual void setPayoutValue(uint32_t value); //set giá tiền thối
    _BV_CONFIG_TYPE getBVConfig();
    // virtual bool disableValidator();
    // virtual bool enableValidator();
    String getValidatorFirmware(); //trả về firmware String
    // virtual String getValidatorName();
};
#endif