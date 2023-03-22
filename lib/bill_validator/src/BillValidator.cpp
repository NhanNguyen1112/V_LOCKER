#include "BillValidator.h"
#include "bv/ICT/BRV3.h"
#include "bv/ITL/nv11.h"
BillValidator::BillValidator()
{
}

BillValidator::~BillValidator()
{
}
void BillValidator::begin(uint8_t protocol)
{
    if (protocol == 3)
    {
        Serial.println("Begin SSP Protocol");
        this->BV = new NV11(this->ESSPDataBus);
        return;
    }
    this->BV = new BRV3(this->ICTDataBus);
}