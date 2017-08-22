/**
 * Calliope Robot.
 *
 * The robot is very simple, it only knows how to handle motor speeds it receives.
 *
 * @copyright (c) Calliope gGmbH.
 *
 * Licensed under the Apache Software License 2.0 (ASL 2.0)
 * Portions (c) Copyright British Broadcasting Corporation under MIT License.
 *
 * @author Matthias L. Jugel <leo@calliope.cc>
 */

#include "CalliopeRover.h"

#include "MicroBit.h"
//#include "uBit_neopixel.h"
#include "CalliopeRGB.h" 
#include "CalliopeSoundMotor.h" 

#ifdef CALLIOPE_ROVER

MicroBit uBit;

MicroBitImage smiley("0,0,255,0,0\n0,0,255,0,0\n0,0,0,0,0\n255,0,0,0,255\n0,255,255,255,0\n");
MicroBitImage angry("0,0,255,0,0\n0,0,255,0,0\n0,0,0,0,0\n0,255,255,255,0\n255,0,0,0,255\n");
MicroBitImage neutral("0,0,255,0,0\n0,0,255,0,0\n0,0,0,0,0\n0,255,255,255,0\0,0,0,0,0\n");


uint8_t speedLeft = 0;
uint8_t speedRight = 0;

uint8_t mode = 0;
/*
neopixel_strip_t m_strip;
uint8_t dig_pin_num = 0;
uint8_t leds_per_strip = 8;
uint8_t err;
uint8_t led_to_enable = 1;
uint8_t red = 255;
uint8_t green = 0;
uint8_t blue = 159;
*/

/**
 * This function is called whenever we receive data. The data packets
 * contain control information for the robot, what to do.
 */
void onData(MicroBitEvent event) {
    (void)event;
    PacketBuffer packet = uBit.radio.datagram.recv();

    if (mode == 0)
    {
    switch(packet.getByte(0)) {
        // brake
        case 'B':
            speedLeft = 0;
            speedRight = 0;
            uBit.display.print("B");
            uBit.soundmotor.motorAOn(speedLeft);
            uBit.soundmotor.motorBOn(speedRight);
            break;
        // speed
        case 'S':
            speedLeft = (uint8_t) packet.getByte(1);
            speedRight = (uint8_t) packet.getByte(2);
            if(speedLeft/10 == speedRight/10) uBit.display.print("^");
            else if(speedLeft/10 > speedRight/10) uBit.display.print("<");
            else if(speedLeft/10 < speedRight/10) uBit.display.print(">");
            //uBit.display.print("S");
            uBit.serial.printf("SET SPEED A=%d, B=%d\r\n", speedLeft, speedRight);
            uBit.soundmotor.motorAOn(speedLeft);
            uBit.soundmotor.motorBOn(speedRight);
            break;
        default:
            break;
    }
    }
}

/**
 * toggle robot mode: autonomous, renmote controlled.
 */
void setMode(MicroBitEvent event) {
    (void)event;
    if (mode)   // switch to remote mode
        mode = 0;
    else        // switch to autonomous mode
    {
        mode = 1;
        // stop the engines!
        uBit.display.print(smiley);

    }
    uBit.soundmotor.motorAOn(0);
    uBit.soundmotor.motorBOn(0);
    uBit.serial.printf("Robot mode: %d\r\n", mode);

}

float getDistance(void)
{
    float distance;
    distance = (float) (uBit.io.P0.getAnalogValue()); // P1 is a value in the range of 0 - 1024 
    return ((distance/2.0)*2.54);  //distance in cm
}


int main() {
    //int i;
    float distance;

    uBit.init();

    uBit.serial.printf("Robot: rover start\r\n");
    // first we add a hook that is called when we press button B
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, setMode);
    // initialize the radio and hookup function
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();
    uBit.radio.setGroup(RADIO_GROUP);

    uBit.serial.printf("Robot: ready\r\n");
    uBit.display.print(smiley);

    //uBit.display.print("R");

    #if 0
    neopixel_init(&m_strip, dig_pin_num, leds_per_strip);
    neopixel_clear(&m_strip);
    for (i=0; i<8; i++)
    {
      err = neopixel_set_color_and_show(&m_strip, led_to_enable, red, green, blue);
      if (err) {
	    //led_to_enable was not within number leds_per_strip
        uBit.display.print("E");
      }
      uBit.sleep(1000);
    }
    #endif
    // stop the engines!
    uBit.soundmotor.motorAOn(0);
    uBit.soundmotor.motorBOn(0);

    while (1) {
        if (mode)
        {
            distance = getDistance();
            if (distance < 20.0)
            {
                uBit.soundmotor.motorAOn(50);
                uBit.soundmotor.motorBOn(0);
                uBit.display.print(angry);

            }
            else if (distance < 30.0)
            {
                uBit.soundmotor.motorAOn(50);
                uBit.soundmotor.motorBOn(50);
                uBit.display.print(neutral);

            }
            else
            {
                uBit.soundmotor.motorAOn(100);
                uBit.soundmotor.motorBOn(100);
                uBit.display.print(smiley);

            }
        }

        uBit.sleep(100);
    }
    
    #if 0
    //clear and remove strip
    neopixel_clear(&m_strip);
    neopixel_destroy(&m_strip);
    #endif
}

#endif
