#include <mega32.h>
#include <alcd.h>
#include <stdio.h>
#include <delay.h>




unsigned char code[4], i = 0, j = 0, flagKeySet = 1, flagKeyTry = 1, try = 0, error = 10;
unsigned char str[17], validCard[11], invalidCard[11];
eeprom unsigned char correctPass[4];





#define relay PORTA.4
#define buzzer PORTA.5
#define setLED PORTA.6
#define selectMode PIND.6 
#define on 1
#define off 0



enum st
{
    setPass,
    setCard,
    getPass,
    getCard
}state;



interrupt [EXT_INT0] void ext_int0_isr(void)
{
    if(flagKeySet == 0)
    {
        correctPass[j] = PINC & 0x0f;
        switch(correctPass[j])
        {
            case 0: 
                correctPass[j] = 1;
                lcd_putchar('1');
            break;
            case 1: 
                correctPass[j] = 4;
                lcd_putchar('4');
            break;
            case 2: 
                correctPass[j] = 7;   
                lcd_putchar('7');
            break;
            case 3: 
                correctPass[j] = '*';
                lcd_putchar('*');
            break;
            case 4: 
                correctPass[j] = 2;
                lcd_putchar('2');
            break;
            case 5: 
                correctPass[j] = 5;  
                lcd_putchar('5');
            break;
            case 6: 
                correctPass[j] = 8;
                lcd_putchar('8');
            break;
            case 7: 
                correctPass[j] = 0;
                lcd_putchar('0');
            break;
            case 8: 
                correctPass[j] = 3; 
                lcd_putchar('3');
            break;
            case 9: 
                correctPass[j] = 6;
                lcd_putchar('6');
            break;
            case 10: 
                correctPass[j] = 9;
                lcd_putchar('9');
            break;
            case 11: 
                correctPass[j] = '#';
                lcd_putchar('#');
            break;
            case 12: 
                correctPass[j] = 'A';
                lcd_putchar('A');
            break;
            case 13: 
                correctPass[j] = 'B';
                lcd_putchar('B');
            break;
            case 14: 
                correctPass[j] = 'C';
                lcd_putchar('C');
            break;
            case 15: 
                correctPass[j] = 'D';
                lcd_putchar('D'); 
        }
        j++;     
    }
    
    if(flagKeyTry == 0)
    {
        code[i] = PINC & 0x0f;
        switch(code[i])
        {
            case 0: 
                code[i] = 1;
                //lcd_putchar('1');
            break;
            case 1: 
                code[i] = 4;
                //lcd_putchar('4');
            break;
            case 2: 
                code[i] = 7;   
                //lcd_putchar('7');
            break;
            case 3: 
                code[i] = '*';
                //lcd_putchar('*');
            break;
            case 4: 
                code[i] = 2;
                //lcd_putchar('2');
            break;
            case 5: 
                code[i] = 5;  
                //lcd_putchar('5');
            break;
            case 6: 
                code[i] = 8;
                //lcd_putchar('8');
            break;
            case 7: 
                code[i] = '0';
                //lcd_putchar('0');
            break;
            case 8: 
                code[i] = 3; 
                //lcd_putchar('3');
            break;
            case 9: 
                code[i] = 6;
                //lcd_putchar('6');
            break;
            case 10: 
                code[i] = 9;
                //lcd_putchar('9');
            break;
            case 11: 
                code[i] = '#';
                //lcd_putchar('#');
            break;
            case 12: 
                code[i] = 'A';
                //lcd_putchar('A');
            break;
            case 13: 
                code[i] = 'B';
                //lcd_putchar('B');
            break;
            case 14: 
                code[i] = 'C';
                //lcd_putchar('C');
            break;
            case 15: 
                code[i] = 'D';
                //lcd_putchar('D'); 
        }
        lcd_putchar('*');
        i++;    
    }

}

interrupt [EXT_INT1] void ext_int1_isr(void)
{
    state = getCard;
}



void main(void)
{

    DDRA = 0xff;
    PORTA = 0x00;
    
    DDRB = 0x00;
    PORTB = 0x00;
    
    DDRC = 0x00;
    PORTC = 0x00;
    
    DDRD = 0x00;
    PORTD = 0x00;



    // External Interrupt(s) initialization
    // INT0: On
    // INT0 Mode: Falling Edge
    // INT1: On
    // INT1 Mode: Falling Edge
    // INT2: Off
    GICR|=(1<<INT1) | (1<<INT0) | (0<<INT2);
    MCUCR=(1<<ISC11) | (0<<ISC10) | (1<<ISC01) | (0<<ISC00);
    MCUCSR=(0<<ISC2);
    GIFR=(1<<INTF1) | (1<<INTF0) | (0<<INTF2);

    // USART initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART Receiver: On
    // USART Transmitter: Off
    // USART Mode: Asynchronous
    // USART Baud Rate: 9600
    UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
    UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (0<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
    UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
    UBRRH=0x00;
    UBRRL=0x33;

    lcd_init(16);
    
    lcd_gotoxy(0, 0);
    lcd_puts("Welcome!");
    delay_ms(3000);
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Set Pass:"); 

    #asm("sei")

    while (1)
    {
        switch(state)
        {
            case setPass:
            {
                flagKeySet = 0;
                if(j == 4)
                {
                    j = 0;
                    flagKeySet = 1;
                    lcd_gotoxy(0, 1);
                    setLED = on;
                    lcd_putsf("Password Set");
                    delay_ms(3000);
                    setLED = off;
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("Set ID Card:");
                    state = setCard;
                }
                break;
            }
            
            case setCard:
            {
                lcd_gotoxy(0, 1);
                gets(validCard, 11);
                setLED = on;
                lcd_puts("ID Card Set");
                delay_ms(3000);
                setLED = off;
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_putsf("Password:");
                state = getPass; 
                flagKeyTry = 0;
                break;
            }
            
            case getPass:
            {
                if(i == 4)
                {
                    i = 0;
                    flagKeyTry = 1;
                    if((code[0] == correctPass[0]) && (code[1] == correctPass[1]) && (code[2] == correctPass[2]) && (code[3] == correctPass[3]))
                    {
                        lcd_gotoxy(0, 1);
                        lcd_putsf("Correct Password");
                        relay = on;
                        delay_ms(3000);
                        relay = off;
                        lcd_clear();
                        lcd_gotoxy(0, 0);
                        lcd_putsf("Password:");
                        flagKeyTry = 0;
                    }
                    else
                    {
                        try++;
                        lcd_gotoxy(0, 1);
                        lcd_putsf("Wrong Password!");
                        buzzer = on;
                        delay_ms(3000);
                        buzzer = off;
                        lcd_clear();
                        lcd_gotoxy(0, 0);
                        lcd_putsf("Password:");
                        flagKeyTry = 0;
                        if(try > 3)
                        {
                            try = 0;
                            lcd_clear();
                            lcd_gotoxy(0, 0);
                            lcd_puts("SystemLocked!");
                            for(error = 10; error > 0; error--)
                            {
                                flagKeyTry = 1;
                                lcd_gotoxy(14, 0);
                                sprintf(str, "%2d", error);
                                lcd_puts(str);
                                delay_ms(1000);    
                            }
                            lcd_clear();
                            lcd_gotoxy(0, 0);
                            lcd_putsf("Password:");
                            flagKeyTry = 0;
                        }      
                    }
                }
                /*if(selectMode == 0)
                {
                    delay_ms(1000);
                    flagKeyTry = 1;
                    state = getCard; 
                }
                else
                {
                    state = getPass;
                    flagKeyTry = 0;
                }*/   
                break;
            }
            
            case getCard:
            {
                flagKeyTry = 1;
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_putsf("Near Card:");
                gets(invalidCard, 11);
                if( (invalidCard[3] == validCard[2]) && (invalidCard[5] == validCard[4]) && (invalidCard[7] == validCard[6]) && (invalidCard[9] == validCard[8]))
                {
                    lcd_gotoxy(0, 1);
                    lcd_putsf("Valid Card");
                    relay = on;
                    delay_ms(3000);
                    relay = 0;
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("Password:");
                    state = getPass; 
                    flagKeyTry = 0;
                }
                else
                {
                    try++;
                    lcd_gotoxy(0, 1);
                    lcd_putsf("Invalid Card!");
                    buzzer = on;
                    delay_ms(3000);
                    buzzer = off;
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_putsf("Password:");
                    state = getPass;
                    flagKeyTry = 0;
                    if(try > 3)
                    {
                        try = 0;
                        lcd_clear();
                        lcd_gotoxy(0, 0);
                        lcd_puts("SystemLocked!");
                        for(error = 10; error > 0; error--)
                        {
                            lcd_gotoxy(14, 0);
                            sprintf(str, "%2d", error);
                            lcd_puts(str);
                            delay_ms(1000);    
                        }
                        lcd_clear();
                        lcd_gotoxy(0, 0);
                        lcd_putsf("Password:");
                        state = getPass;
                        flagKeyTry = 0;
                    }
                }  
            }   
        }//End Switch()
    }//End While(1)
}//End Main()
