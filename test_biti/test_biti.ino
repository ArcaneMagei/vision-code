#include <avr/io.h>

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

byte sensors;
unsigned long atac_millis = 0, qtr_millis = 0, search_millis = 0, diferenta_millis = 0;;
boolean seen = false, parte;//parte - false= stanga, true = dreapta
char direction = 0, strategie = 0;
int qtr_stanga_fata, qtr_dreapta_fata;

int main(void)
{
  init();
  /*
    DDRD  &= ~0xFC; //senzori 2,3,4,5,6,7
    PORTD &= ~0xFC;

    DDRB  &= ~0x3F; //senzor 8 + dip switches+senzor start stop
    PORTB &= ~0x1F;*/
  DDRD &= ~0xBD;
  PORTD &= ~0xBD;

  DDRB &= ~0x01;
  PORTB &= ~0x01;
  Serial.begin(9600);
  while (1)
  {
    //sensors = (PIND & ~0x03) | ((PINB & 0x01) & 0x03);
    sensors = (PIND & ~0x47) | (PINB & 0x01);
    qtr_stanga_fata  = analogRead(A0);
    qtr_dreapta_fata = analogRead(A1);/*
    if(PINB & 0x02)
    {
      Serial.print("---dip1 NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---dip1 DA---");
      Serial.print("\t");
    }

    if(PINB & 0x04)
    {
      Serial.print("---dip2 NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---dip2 DA---");
      Serial.print("\t");
    }
    if(PINB & 0x08)
    {
      Serial.print("---dip3 NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---dip3 DA---");
      Serial.print("\t");
    }
    if(PINB & 0x10)
    {
      Serial.print("---dip4 NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---dip4 DA---");
      Serial.print("\t");
    }*//*
    if(sensors & 0x04)
    {
      Serial.print("---diag stg NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---diag stg DA---");
      Serial.print("\t");
    }
    if(sensors & 0x08)
    {
      Serial.print("---stg NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---stg DA---");
      Serial.print("\t");
    }
    if(sensors & 0x10)
    {
      Serial.print("---fata stg NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---fata stg DA---");
      Serial.print("\t");
    }
    if(sensors & 0x20)
    {
      Serial.print("---mijl NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---mijl DA---");
      Serial.print("\t");
    }
    if(sensors & 0x40)
    {
      Serial.print("---dr_fata NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---dr_fata DA---");
      Serial.print("\t");
    }
    if(sensors & 0x80)
    {
      Serial.print("---dreapta NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---dreapta DA---");
      Serial.print("\t");
    }
    if(sensors & 0x01)
    {
      Serial.print("---diag dreapta NU---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---diag dreapta DA---");
      Serial.print("\t");
    }
    Serial.println(" ");*/
    if (sensors & 0x80)
    {
      Serial.print("---stg fata DA---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---stg fata NU---");
      Serial.print("\t");
    }
    if (sensors & 0x20)
    {
      Serial.print("---fata DA---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---fata NU---");
      Serial.print("\t");
    }
    if (sensors & 0x10)
    {
      Serial.print("---dr fata DA---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---dr fata NU---");
      Serial.print("\t");
    }
    if (sensors & 0x08)
    {
      Serial.print("---diag dr DA---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---diag dr NU---");
      Serial.print("\t");
    }
    if (sensors & 0x01)
    {
      Serial.print("---Diag stanga  DA---");
      Serial.print("\t");
    }
    else
    {
      Serial.print("---diag stanga NU---");
      Serial.print("\t");
    }
    Serial.println(" ");
    delay(200);
  }//end while
}


void select_strategie()
{
  if (PINB & 0x12 == 0x12)
    strategie = 0;
  else if (PINB & 0x02)
    strategie = 1;
  else if (PINB & 0x10)
    strategie = 2;
  else if (!(PINB & 0x12))
    strategie = 3;
}
