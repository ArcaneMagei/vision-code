#include <avr/io.h>
#include <Sabertooth.h>

Sabertooth ST(128);

/*
   2 - stanga diagonala
   3 - stanga
   4 - stanga fata
   5 - centru
   6 - dr fata
   7 - dr
   8 - dr diag

   9-12 - dip

   A0(dr),A1- qtr

   13 - start
   saber tx - tx
*/

void select_strategie();
void search();
void linie_alba();

byte sensors, dips;
unsigned long atac_millis = 0, qtr_millis = 0, search_millis = 0, diferenta_millis = 0, turning_millis = 0, japan_millis = 0;
boolean seen = false, parte, fiveSecondEnable = true, qtrEnable = true, enable = true, stratEnable = true;;//parte - false= stanga, true = dreapta
char direction = 0, strategie = 0;
int qtr_stanga_fata, qtr_dreapta_fata;

int main(void)
{
  init();

  SabertoothTXPinSerial.begin(115200);

  ST.motor(1, 0);
  ST.motor(2, 0);

  DDRD  &= ~0xFC; //senzori 2,3,4,5,6,7
  PORTD &= ~0xFC;

  DDRB  &= ~0x3F; //senzor 8 + dip switches+senzor start stop
  PORTB &= ~0x1F;

  while (1)
  {
    sensors = (PIND & ~0x03) | ((PINB & 0x01) & 0x03);
    qtr_stanga_fata  = analogRead(A0);
    qtr_dreapta_fata = analogRead(A1);

    if (PINB & 0x20) //start module
    {
      if (fiveSecondEnable)
      {
        delay(5000);
        fiveSecondEnable = false;
      }
      if (qtr_stanga_fata < 150)
      {
        seen = true;
        parte = true;
      }
      if ( qtr_dreapta_fata < 30)
      {
        seen = true;
        parte = false;
      }

      if (seen == true && qtrEnable)
        linie_alba();
      else
      {
        if (stratEnable)
        {
          switch (strategie)
          {
            case 0: //japan
              {
                diferenta_millis = millis() - atac_millis;
                if (diferenta_millis < 100)
                {
                  ST.motor(1, 127);
                  ST.motor(2, 127);
                }
                else if (diferenta_millis < 300)
                {
                  ST.motor(1, 0);
                  ST.motor(2, 0);
                }
                else if (diferenta_millis < 1000 && sensors == 0b11111101)
                {
                  ST.motor(1, 0);
                  ST.motor(2, 0);
                }
                else {
                  search();
                  stratEnable = false;
                }
              } break;
            case 1: //curba stanga
              {
                diferenta_millis = millis() - atac_millis;
                if (diferenta_millis < 150)
                {
                  ST.motor(1, 127);
                  ST.motor(2, 70);
                }
                else if (diferenta_millis < 400 && sensors == 0b11111101)
                {
                  ST.motor(1, 127);
                  ST.motor(2, 70);
                }
                else {
                  search();
                  stratEnable = false;
                }
              } break;
            case 2://curba_dreapta
              {
                diferenta_millis = millis() - atac_millis;
                if (diferenta_millis < 150)
                {
                  ST.motor(1, 70);
                  ST.motor(2, 127);
                }
                else if (diferenta_millis < 400 && sensors == 0b11111101)
                {
                  ST.motor(1, 70);
                  ST.motor(2, 127);
                }
                else {
                  search();
                  stratEnable = false;
                }
              } break;
            case 3: //blana
              {
                if (millis() - atac_millis < 100)
                {
                  ST.motor(1, 127);
                  ST.motor(2, 127);
                }
                else
                {
                  search();
                  stratEnable = false;
                }
              } break;

            case 4: //test linie
              {
                ST.motor(1, 100);
                ST.motor(2, 100);
              } break;
            case 5:
              {
                japanAdvanced();
              } break;
            case 6:
              {
                qtrEnable = false;
                search();
                stratEnable = false;
              } break;
          }//end strategii
        }
        else search();
        qtr_millis = millis();
      }
    }
    else//in stop
    {
      ST.motor(1, 0);
      ST.motor(2, 0);

      select_strategie();

      atac_millis = millis();
    }
  }//end while
}

void linie_alba()
{
    if (millis() - qtr_millis < 80)
    {
      ST.motor(1, -127);
      ST.motor(2, -127);
    }
    else if (millis() - qtr_millis < 185)
    {
      if (parte)
      {
        ST.motor(1, -80);
        ST.motor(2, 80);
      }
      else
      {
        ST.motor(1, 80);
        ST.motor(2, -80);
      }
    }
    else
      seen = false;
}
void japanAdvanced()
{
  if ( sensors != 0b10001101 && enable)
  {
    if ((~sensors & 0x20) || (sensors == 0b11111101 && direction == 0)  )
    {
      if (millis() - japan_millis < 800) //vede cu fata
      {
        ST.motor(1, 0);
        ST.motor(2, 0);
      }
      else if (millis() - japan_millis < 850)
      {
        ST.motor(1, 100);
        ST.motor(2, 100);
      }
      else
      {
        japan_millis = millis();
      }
    }
    else
    {
      search_pe_loc();
    }
  }
  else {
    enable = false;
    search();
  }
}
void search_pe_loc()
{
  switch (sensors)
  {
    case 0b11011101://0
      {
        direction = 0;
      } break;
    case 0b10011101://1
      {
        direction = 0;
      } break;

    case 0b11001101://2
      {
        direction = 0;
      } break;

    case 0b10111101://3
      {
        ST.motor(1, 30);
        ST.motor(2, -30);
        direction = -1;
        turning_millis = millis();
        japan_millis = millis();
      } break;

    case 0b11101101://4
      {
        ST.motor(1, -30);
        ST.motor(2, 30);
        direction = 1;
        turning_millis = millis();
        japan_millis = millis();
      } break;

    case 0b10111001://5
      {
        ST.motor(1, 45);
        ST.motor(2, -45);
        direction = -1; //stanga
        turning_millis = millis();
        japan_millis = millis();
      } break;

    case 0b11101100://6
      {
        ST.motor(1, -45);
        ST.motor(2, 45);
        direction = 1;
        turning_millis = millis();
        japan_millis = millis();
      } break;

    case 0b11111001://7
      {
        ST.motor(1, 60);
        ST.motor(2, -60);
        direction = -2;
        turning_millis = millis();
        japan_millis = millis();
      } break;

    case 0b11111100://8
      {
        ST.motor(1, -60);
        ST.motor(2, 60);
        direction = 2;
        turning_millis = millis();
        japan_millis = millis();
      } break;

    case 0b11110001://9
      {
        ST.motor(1, 75);
        ST.motor(2, -75);
        direction = -2;
        turning_millis = millis();
        japan_millis = millis();
      } break;

    case 0b01111100://10
      {
        ST.motor(1, -75);
        ST.motor(2, 75);
        direction = 2;
        turning_millis = millis();
        japan_millis = millis();
      } break;

    case 0b11110101://11
      {
        ST.motor(1, 100);
        ST.motor(2, -100);
        direction = -3;
        turning_millis = millis();
        japan_millis = millis();
      } break;

    case 0b01111101://12
      {
        ST.motor(1, -100);
        ST.motor(2, 100);
        direction = 3;
        turning_millis = millis();
        japan_millis = millis();
      } break;

    case 0b11111101://13+14
      {

        switch (direction)
        {
          case 0:
            {

            } break;
          case 1:
            {
              if (millis() - turning_millis < 100)
              {
                ST.motor(1, -35);
                ST.motor(2, 35);
              }
              else
              {
                direction = 0;
              }
              japan_millis = millis();
            } break;
          case -1:
            {
              if (millis() - turning_millis < 100)
              {
                ST.motor(1, 35);
                ST.motor(2, -35);
              }
              else
              {
                direction = 0;
              }
              japan_millis = millis();
            } break;

          case 2:
            {
              if (millis() - turning_millis < 1000)
              {
                ST.motor(1, -60);
                ST.motor(2, 60);
              }
              else
              {
                direction = 0;
              }
              japan_millis = millis();
            } break;
          case -2:
            {
              if (millis() - turning_millis < 1000)
              {
                ST.motor(1, 60);
                ST.motor(2, -60);
              }
              else
              {
                direction = 0;
              }
              japan_millis = millis();
            } break;


          case 3:
            {
              if (millis() - turning_millis < 1000)
              {
                ST.motor(1, -110);
                ST.motor(2, 110);
              }
              else
              {
                direction = 0;
              }
              japan_millis = millis();
            } break;
          case -3:
            {
              if (millis() - turning_millis < 1000)
              {
                ST.motor(1, 110);
                ST.motor(2, -110);
              }
              else
              {
                direction = 0;
              }
              japan_millis = millis();
            } break;
        } break;
      }//end case 13+14

    default: //steag sau dubios
      {
        direction = 0;
      } break;
  }
}
void select_strategie()
{
  int dip1 = digitalRead(9);
  int dip2 = digitalRead(10);
  int dip3 = digitalRead(11);
  int dip4 = digitalRead(12);

  if (dip1 && dip2 && dip3 && dip4)
    strategie = 0; //jap
  if (!dip1 && dip2 && dip3 && dip4)
    strategie = 1; //stg
  if (dip1 && !dip2 && dip3 && dip4)
    strategie = 2; //dr
  if (dip1 && dip2 && !dip3 && dip4)
    strategie = 3; //blana
  if (dip1 && dip2 && dip3 && !dip4)
    strategie = 4; //linie
  if (!dip1 && dip2 && dip3 && !dip4)
    strategie = 5; //ultrajap
  if (!dip1 && !dip2 && !dip3 && !dip4)
    strategie = 6;
}
void search()
{
  switch (sensors)
  {
    case 0b10001101://100
      {
        ST.motor(1, 127);
        ST.motor(2, 127);
        direction = 0; //default
      } break;
    case 0b11011101://0
      {
        ST.motor(1, 80);
        ST.motor(2, 80);
        direction = 0;
      } break;
    case 0b10011101://1
      {
        ST.motor(1, 110);
        ST.motor(2, 100);
        direction = 0;
      } break;

    case 0b11001101://2
      {
        ST.motor(1, 100);
        ST.motor(2, 110);
        direction = 0;
      } break;

    case 0b10111101://3
      {
        ST.motor(1, 110);
        ST.motor(2, 90);
        direction = -1;
        turning_millis = millis();
      } break;

    case 0b11101101://4
      {
        ST.motor(1, 90);
        ST.motor(2, 110);
        direction = 1;
        turning_millis = millis();
      } break;

    case 0b10111001://5
      {
        ST.motor(1, 110);
        ST.motor(2, 80);
        direction = -1; //stanga
        turning_millis = millis();
      } break;

    case 0b11101100://6
      {
        ST.motor(1, 80);
        ST.motor(2, 110);
        direction = 1;
        turning_millis = millis();
      } break;

    case 0b11111001://7
      {
        ST.motor(1, 110);
        ST.motor(2, 0);
        direction = -2;
        turning_millis = millis();
      } break;

    case 0b11111100://8
      {
        ST.motor(1, 0);
        ST.motor(2, 110);
        direction = 2;
        turning_millis = millis();
      } break;

    case 0b11110001://9
      {
        ST.motor(1, 110);
        ST.motor(2, -70);
        direction = -2;
        turning_millis = millis();
      } break;

    case 0b01111100://10
      {
        ST.motor(1, -70);
        ST.motor(2, 110);
        direction = 2;
        turning_millis = millis();
      } break;

    case 0b11110101://11
      {
        ST.motor(1, 100);
        ST.motor(2, -100);
        direction = -3;
        turning_millis = millis();
      } break;

    case 0b01111101://12
      {
        ST.motor(1, -100);
        ST.motor(2, 100);
        direction = 3;
        turning_millis = millis();
      } break;

    case 0b11111101://13+14
      {
        switch (direction)
        {
          case 0:
            {
              ST.motor(1, 100);
              ST.motor(2, 100);
            } break;
          case 1:
            {
              if (millis() - turning_millis < 100)
              {
                ST.motor(1, 70);
                ST.motor(2, 110);
              }
              else
              {
                direction = 0;
              }
            } break;
          case -1:
            {
              if (millis() - turning_millis < 100)
              {
                ST.motor(1, 110);
                ST.motor(2, 70);
              }
              else
              {
                direction = 0;
              }
            } break;

          case 2:
            {
              if (millis() - turning_millis < 2000)
              {
                ST.motor(1, 0);
                ST.motor(2, 110);
              }
              else
              {
                direction = 0;
              }
            } break;
          case -2:
            {
              if (millis() - turning_millis < 2000)
              {
                ST.motor(1, 110);
                ST.motor(2, 0);
              }
              else
              {
                direction = 0;
              }
            } break;


          case 3:
            {
              if (millis() - turning_millis < 2000)
              {
                ST.motor(1, -110);
                ST.motor(2, 110);
              }
              else
              {
                direction = 0;
              }
            } break;
          case -3:
            {
              if (millis() - turning_millis < 2000)
              {
                ST.motor(1, 110);
                ST.motor(2, -110);
              }
              else
              {
                direction = 0;
              }
            } break;
        } break;
      }//end case 13+14

    default: //steag sau dubios
      {
        ST.motor(1, 110);
        ST.motor(2, 110);
      } break;
  }
}
