#include "vhitek.h"

namespace VHITEK
{
    namespace Keypad
    {
        MatrixKeyboardManager keyboard;
        MultiIoAbstractionRef ExternalIO = multiIoExpander(0);
        KeyboardLayout keyLayout(4, 3, "04815926*37#");

        QueueHandle_t KeypadQueue; //tao Queue ten la KeypadQueue

        class MyKeyboardListener : public KeyboardListener
        {
        public:
            void keyPressed(char key, bool held) override
            {
                // Serial.printf("key pressed: %c \n", key);
                xQueueSend(KeypadQueue, &key, 10);
            }

            void keyReleased(char key) override
            {
                // Serial.print("key release");
            }

        } myListener;     

        void setup()
        {
            #ifdef V1
            multiIoAddExpander(ExternalIO, ioFrom8574(0x20), 8); //Vgas V1: 0x20 ; V2: 0x38
            #else 
            multiIoAddExpander(ExternalIO, ioFrom8574(0x38), 8); //Vgas V1: 0x20 ; V2: 0x38
            #endif
            KeypadQueue = xQueueCreate(50, sizeof(char));
            keyLayout.setRowPin(0, 7);
            keyLayout.setRowPin(1, 6);
            keyLayout.setRowPin(2, 5);
            keyLayout.setRowPin(3, 4);

            keyLayout.setColPin(0, 0);
            keyLayout.setColPin(1, 1);
            keyLayout.setColPin(2, 2);
            keyLayout.setColPin(3, 3);
            keyboard.initialise(ExternalIO, &keyLayout, &myListener);
        }           

        void loop()
        {
            taskManager.runLoop();      
        }      

        char getKey() //lay key tu Queue Key ra
        {
            char key;
            if(xQueueReceive(KeypadQueue, &key, 10))    return key;
            return 0;
        }

        void clearKeypad(char *keyData, int lenght)
        {
            for (int i = 0; i < lenght; i++)
            {
                keyData[i] = 0;
            }
            xQueueReset(KeypadQueue);
        }

        void clearKeypad()
        {
            xQueueReset(KeypadQueue);
        }          
    }
}