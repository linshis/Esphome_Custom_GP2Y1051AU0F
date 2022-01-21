#include "esphome.h"

//class My_dust_Sensor : public PollingComponent, public Sensor
class My_dust_Sensor : public PollingComponent, public sensor::Sensor, public UARTDevice
{
 public:
  My_dust_Sensor(uint32_t update_interval, UARTComponent *parent) : PollingComponent(update_interval), UARTDevice(parent) {}


  void setup() override {
       
   }

  void update() override {

    int incomeByte[7];
	int data;
	int z=0;
	int start = 0;
    int sum;
    int ErrCnt = 0;
    int ErrCntTh = 3;
    float Vout = 0;
    
    #if 1
	while (available()>0)
    {
        data=read();
        if( (data == 170) && (start == 0) )  // 170 = 0xaa, °_©l¦r¸` 
        {  
            z = 0;
            start = 1;
            incomeByte[z] = data;
        }
        else if(start)
        {
            z++;
            incomeByte[z] = data;
        } 
        if(z==6)
        {
            sum = incomeByte[1] + incomeByte[2] + incomeByte[3] + incomeByte[4];
            if( (incomeByte[5]==sum) && (incomeByte[6]==255) )
            {
                Vout =(incomeByte[1]*256.0 + incomeByte[2])/1024.0; 
                ESP_LOGD("custom", "UART %.2X %.2X %.2X %.2X %.2X %.2X %.2X", incomeByte[0], incomeByte[1], incomeByte[2], incomeByte[3], incomeByte[4], incomeByte[5], incomeByte[6]);
                ESP_LOGD("custom", "VOUT %f", Vout);
                break;
            }
            else
            {
                if(ErrCnt < ErrCntTh)
                {
                    z = 0;
                    start = 0;
                    ErrCnt++;
                }
                else
                {
                    Vout = 0.99;
                    ESP_LOGD("custom", "Too many checksum error, give up");
                    break;
                }
            }
        } 
    }
    #endif

    publish_state(Vout);              // publish
  }
};


