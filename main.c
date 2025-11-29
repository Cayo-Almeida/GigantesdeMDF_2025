#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//As próximas duas bibliotecas são para o funcionamento da comunicação via rádio
#include <RH_ASK.h>
#include <SPI.h>

RH_ASK rf_driver; //Importante para a comunicação via rádio

volatile uint8_t overflow_count = 0;

uint8_t leds[] = {2,3,4}; // Array para identificação dos LEDs em DDRC
int vidas = 3; // Quantidade de vidas
uint8_t vivo = 1; //Varíavel de controle para saber se o tanque ainda está em jogo

volatile uint8_t adc_flag = 0;
volatile uint16_t adc_value = 0;

void adc_init(void) //Inicia o ADC
{
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 128
}

uint16_t adc_read(uint8_t canal) //Lê os valores analógicos recebidos
{
    ADMUX = (ADMUX & 0XF0) | (canal & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

void timer0_init() //Inicia o timer0
{
	TCCR0A = (1 << WGM01) | (1 << WGM00);
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
    TCCR0B = (1 << CS01) | (1 << CS00);
}

void timer2_init() //inicia o timer2
{
	TCCR2A = 0x00;
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
    TIMSK2 = (1 << TOIE2);          
    sei();
}

void recebeuDisparo(){
	vidas--;
	if(vidas!=0)
	{
		vivo = 0;
		PORTC &= ~(1 << leds[vidas]);
		//Realiza o 180º graus
		OCR0A = 255;
		OCR0B = 255;
		PORTB &= ~(1<<2);
		PORTB &= ~(1<<0);
		PORTD |= (1<<7);
		PORTB |= (1<<1);
		_delay_ms(2000); // delay até realizar o giro de 180º
		//Encerra o giro, desliga os motores e permanece inativo
		PORTB &= ~(1<<3);
		TIMSK2 &= ~(1 << TOIE2);
		OCR0A = 0;
		OCR0B = 0;
		PORTB &= ~(1<<2);
		PORTB &= ~(1<<1);
		PORTB &= ~(1<<0);
		PORTD &= ~(1<<7);
		_delay_ms(3000);
		vivo = 1;
		TIMSK2 |= (1 << TOIE2);
	}
	else
	{
		//Todas as vidas foram encerradas, logo não pode funcionar mais
		vivo = 0;
		PORTB &= ~(1<<3);
		TIMSK2 &= ~(1 << TOIE2);
		PORTC &= ~(1 << leds[vidas]); //Desliga o último LED restante
		//Realiza o 180º graus
		OCR0A = 255;
		OCR0B = 255;
		PORTB &= ~(1<<2);
		PORTB &= ~(1<<0);
		PORTD |= (1<<7);
		PORTB |= (1<<1);
		_delay_ms(2000); // delay até realizar o giro de 180º
		//Encerra o giro, desliga os motores e permanece inativo
		PORTB &= ~(1<<3);
		TIMSK2 &= ~(1 << TOIE2);
		OCR0A = 0;
		OCR0B = 0;
		PORTB &= ~(1<<2);
		PORTB &= ~(1<<1);
		PORTB &= ~(1<<0);
		PORTD &= ~(1<<7);
	}
}

void moverTanque(uint8_t comando)
{
	//Os pinos que controlam a polaridade do motor da ESQUERDA são: PD7 e PB0
	//Os pinos que controlam a polaridade do motor da DIREITA são: PB1 e PB2
    switch(comando)
	{
        case 1: // frente
            OCR0A = 255; //PD6
            OCR0B = 255; //PD5
			//Encerra o envio de tensão para os pinos PB0 e PB1
            PORTB &= ~(1<<0); //Polaridade para trás
            PORTB &= ~(1<<1); //Polaridade para trás
			//Envia tensão para os pinos PD7 e PB2
            PORTD |= (1<<7);// Polaridade para frente
            PORTB |= (1<<2);// Polaridade para frente
            break;
        case 2: // ré
            OCR0A = 255;
            OCR0B = 255;
			//Encerra o envio de tensão para os pinos PD7 e PB2
            PORTD &= ~(1<<7);// Polaridade para frente
            PORTB &= ~(1<<2);// Polaridade para frente
			//Envia tensão para os pinos PD7 e PB2
            PORTB |= (1<<0);  //Polaridade para trás
            PORTB |= (1<<1); //Polaridade para trás
            break;
        case 3: // esquerda
			//Envia tensão somente para o PB2
            OCR0A = 0;
            OCR0B = 255;
            PORTD &= ~(1<<7);// Polaridade para frente
            PORTB &= ~(1<<0); //Polaridade para trás
            PORTB &= ~(1<<1); //Polaridade para trás
            PORTB |= (1<<2);// Polaridade para frente
            break;
        case 4: // direita
			//Envia tensão somente para o PD7
            OCR0A = 255;
            OCR0B = 0;
            PORTB &= ~(1<<1); //Polaridade para trás
            PORTB &= ~(1<<0); //Polaridade para trás
            PORTB &= ~(1<<2);// Polaridade para frente
            PORTD |= (1<<7); // Polaridade para frente
            break;
        case 5: // diagonal sup esq
			//Encerra o envio de tensão para os pinos PB0 e PB1
            OCR0A = 127;
            OCR0B = 255;
            PORTB &= ~(1<<0); //Polaridade para trás
            PORTB &= ~(1<<1); //Polaridade para trás
			//Envia tensão para os pinos PD7 e PB2
            PORTD |= (1<<7); // Polaridade para frente
            PORTB |= (1<<2); // Polaridade para frente
            break;
        case 6: // diagonal sup dir
            OCR0A = 255;
            OCR0B = 127;
			//Encerra o envio de tensão para os pinos PB0 e PB1
            PORTB &= ~(1<<0); //Polaridade para trás
            PORTB &= ~(1<<1); //Polaridade para trás
			//Envia tensão para os pinos PD7 e PB2
            PORTD |= (1<<7); // Polaridade para frente
            PORTB |= (1<<2); // Polaridade para frente
            break;
        case 7: // diagonal inf esq
            OCR0A = 127;
            OCR0B = 255;
			//Encerra o envio de tensão para os pinos PD7 e PB2
            PORTD &= ~(1<<7); // Polaridade para frente zerada
            PORTB &= ~(1<<2); // Polaridade para frente zerada
			//Envia tensão para os pinos PB0 e PB1;
            PORTB |= (1<<0); //Polaridade para trás
            PORTB |= (1<<1); //Polaridade para trás
            break;
        case 8: // diagonal inf dir
            OCR0A = 255;
            OCR0B = 127;
			//Encerra o envio de tensão para os pinos PD7 e PB2
            PORTD &= ~(1<<7);
            PORTB &= ~(1<<2);
			//Envia tensão para os pinos PB0 e PB1;
            PORTB |= (1<<0);
            PORTB |= (1<<1);
            break;
    }
}

int valor = 0;

int main (void)
{
	//CI L293D: Este CI é responsável pelo controle dos motores do projeto
	DDRD |= (1<<5)|(1<<6); // PWM 1,2 e PWM 3,4 , respectivamente
	DDRD |= (1<<7); //PIN 3A
	DDRB |= (1<<0); //PIN 4A
	DDRB |= (1<<1); //PIN 2A
	DDRB |= (1<<2); //PIN 1A

	//Os pinos PB2 e PD7 são responsáveis por girar o tanque para FRENTE
	//Os pinos PB1 e PD0 são responsáveis por girar o tanque para TRÁS

	//Laser
	DDRB |= (1<<3);
	
	//LEDs para vida
	DDRC |= (1<<4)|(1<<3)|(1<<2);
	
	//LDR
	DDRC &= ~(1 << PC5);
	
	//Ascende os LEDs
	PORTC |= (1<<4)|(1<<3)|(1<<2);
	
	timer0_init();
	timer2_init();
	adc_init();
	rf_driver.init(); //Inicia a comunicação entre o controle e o Atmega328 via rádio
	
	while(vivo == 1)
	{
		valor = adc_read(5);
		//Verifica o valor de LDR; se a condição for verdadeira, chama a função recebeuDisparo();
		if (valor > 1000)
		{
			recebeuDisparo();
		}

		// Verifica comandos recebidos pelo receptor 433Mhz
        uint8_t buf[2]; //buff[0] é o ID do sinal e buff[1] o ID da posição do joystick no controle
		uint8_t buflen = sizeof(buf);
		if(rf_driver.recv(buf, &buflen))
		{
			//Se a varíavel de identificação corresponder, então é o sinal correto
    		if(buf[0] == 0xA5)
			{
        	moverTanque(buf[1]);
    		}
		}
	}
}

ISR(TIMER2_OVF_vect)
{
    overflow_count++;
    if (overflow_count >= 61)
    {
        PORTB ^= (1 << PB3); // Pisca o Laser
        overflow_count = 0;
    }
}