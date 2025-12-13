#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t overflow_count = 0;

uint8_t leds[] = {2,3,4};
int vidas = 3;
uint8_t vivo = 1;

volatile uint8_t adc_flag = 0;
volatile uint16_t adc_value = 0;

void adc_init(void) {
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 128
}

uint16_t adc_read(uint8_t canal) {
    ADMUX = (ADMUX & 0XF0) | (canal & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

void timer0_init(){
	TCCR0A = (1 << WGM01) | (1 << WGM00);
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
    TCCR0B = (1 << CS01) | (1 << CS00);
}

void timer2_init(){
	TCCR2A = 0x00;
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
    TIMSK2 = (1 << TOIE2);          
    sei();
}

void recebeuDisparo(){
	vidas--;
	if(vidas!=0){
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
	else{
		//Todas as vidas foram encerradas, logo não pode funcionar mais
		vivo = 0;
	    PORTB &= ~(1<<3);
		TIMSK2 &= ~(1 << TOIE2);
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
		OCR0A = 0;
		OCR0B = 0;
		PORTB &= ~(1<<2);
		PORTB &= ~(1<<1);
		PORTB &= ~(1<<0);
		PORTD &= ~(1<<7);
	}
}

void moverTanque(char comando){

    switch(comando){
        case 'F': // frente
            OCR0A = 255; //PD6
            OCR0B = 255; //PD5
            PORTB &= ~(1<<0); //Polaridade para trás
            PORTB &= ~(1<<1); //Polaridade para trás
            PORTD |= (1<<7);// Polaridade para frente
            PORTB |= (1<<2);// Polaridade para frente
            break;
        case 'B': // ré
            OCR0A = 255;
            OCR0B = 255;
            PORTD &= ~(1<<7);// Polaridade para frente
            PORTB &= ~(1<<2);// Polaridade para frente
            PORTB |= (1<<0);  //Polaridade para trás
            PORTB |= (1<<1); //Polaridade para trás
            break;
        case 'L': // esquerda
            OCR0A = 0;
            OCR0B = 255;
            PORTD &= ~(1<<7);// Polaridade para frente
            PORTB &= ~(1<<0); //Polaridade para trás
            PORTB &= ~(1<<1); //Polaridade para trás
            PORTB |= (1<<2);// Polaridade para frente
            break;
        case 'R': // direita
            OCR0A = 255;
            OCR0B = 0;
            PORTB &= ~(1<<1); //Polaridade para trás
            PORTB &= ~(1<<0); //Polaridade para trás
            PORTB &= ~(1<<2);// Polaridade para frente
            PORTD |= (1<<7); // Polaridade para frente
            break;
        case 'G': // diagonal sup esq
            OCR0A = 127;
            OCR0B = 255;
            PORTB &= ~(1<<0); //Polaridade para trás
            PORTB &= ~(1<<1); //Polaridade para trás
            PORTD |= (1<<7); // Polaridade para frente
            PORTB |= (1<<2); // Polaridade para frente
            break;
        case 'H': // diagonal sup dir
            OCR0A = 255;
            OCR0B = 127;
            PORTB &= ~(1<<0); //Polaridade para trás
            PORTB &= ~(1<<1); //Polaridade para trás
            PORTD |= (1<<7); // Polaridade para frente
            PORTB |= (1<<2); // Polaridade para frente
            break;
        case 'I': // diagonal inf esq
            OCR0A = 127;
            OCR0B = 255;
            PORTB |= (1<<0); //Polaridade para trás
            PORTB |= (1<<1); //Polaridade para trás
            PORTD &= ~(1<<7); // Polaridade para frente
            PORTB &= ~(1<<2); // Polaridade para frente
            break;
        case 'J': // diagonal inf dir
            OCR0A = 255;
            OCR0B = 127;
            PORTB |= (1<<0);
            PORTB |= (1<<1);
            PORTD &= ~(1<<7);
            PORTB &= ~(1<<2);
            break;
        case 'S':
            OCR0A = 0;
            OCR0B = 0;
            PORTB &= ~(1<<0);
            PORTB &= ~(1<<1);
            PORTD &= ~(1<<7);
            PORTB &= ~(1<<2);            
            break;
    }
}

int valor = 0;

int main (void){
	//CI L293D
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
	
	PORTC |= (1<<4)|(1<<3)|(1<<2);

  //Serial.begin(9600);
	Serial.begin(38400);
	timer0_init();
	timer2_init();
	adc_init();
	
	while(vivo == 1){
		
		valor = adc_read(5);
		if (valor >= 1023) {
			recebeuDisparo();
    }
    if(Serial.available()){ //Faz a verificação dos dados recebidos através do HC-05
      char comando_recebido = Serial.read();
      moverTanque(comando_recebido);
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