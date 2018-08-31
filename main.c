#include <avr/io.h>
#include <avr/interrupt.h>
#include <ucr/bit.h>
#include <ucr/timer.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

unsigned char left_btn;
unsigned char right_btn;
unsigned char up_btn;
unsigned char down_btn;
unsigned char start_btn;
const unsigned char boardWidth = 0x08; 
const unsigned char boardHeight = 0x08;

unsigned char X;
unsigned char Y;
unsigned char end;
unsigned char tempD = 0x00;

unsigned char left_btn;
unsigned char right_btn;
unsigned char up_btn;
unsigned char down_btn;
unsigned char start_btn;
unsigned char rst_btn; 

unsigned char food_x;
unsigned char food_y;

char InvertX(char s){
// 	unsigned char i; 
// 	unsigned char B0 = (X & 0x01); 
// 	unsigned char B1 = (X & 0x02); 
// 	unsigned char B2 = (X & 0x04); 
// 	unsigned char B3 = (X & 0x08); 
// 	unsigned char B4 = (X & 0x10); 
// 	unsigned char B5 = (X & 0x20); 
// 	unsigned char B6 = (X & 0x40); 
// 	unsigned char B7 = (X & 0x80); 	
// 	
// 	if (B0 == 0x01){
// 		B0 = 0x00; 
// 	} 
// 	else if (B0 == 0x00){
// 		B0 = 0x01; 
// 	}
// 	
// 	if (B1 == 0x01){
// 		B1 = 0x00; 
// 	}
// 	else if (B1 == 0x00){
// 		B1 = 0x01; 
// 	}
// 	if (B2 == 0x01){
// 		B2 = 0x00; 
// 	}
// 	else if (B2 == 0x00){
// 		B2 = 0x01;
// 	}
// 	if (B3 == 0x01){
// 		B3 = 0x00;
// 	}
// 	else if (B3 == 0x00){
// 		B3 = 0x01;
// 	}
// 	if (B4 == 0x01){
// 		B4 = 0x00;
// 	}
// 	else if (B4 == 0x00){
// 		B4 = 0x01; 
// 	}
// 	if (B5 == 0x01){
// 		B5 = 0x00;
// 	}
// 	else if (B5 == 0x00){
// 		B5 = 0x01; 
// 	}
// 	if (B6 == 0x01){
// 	B6 = 0x00; 
// 	}
// 	else if (B6 == 0x00){
// 		B6 = 0x01; 
// 	}
// 	if (B7 == 0x01){
// 		B7 = 0x00; 
// 	}
// 	else if (B7 == 0x00){
// 		B7 = 0x01; 
// 	}
// 	// bitwise operator here ????
// 	X = ((B7 << 7) & (B6 << 6) & (B5 << 5) & (B4 << 4) & (B3 << 3) & (B2 << 2) & (B1 << 1) & B0);

s = ~s; 

return s; 
	
}


typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
	
} task;

unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}



// ====================
// SM1: DEMO LED matrix
// ====================
enum SM1_States {sm1_display};
int SM1_Tick(int state) {

	// === Local Variables ===
	//static unsigned char column_val = 0x10; // sets the pattern displayed on columns
	//static unsigned char column_sel = 0xE7; // grounds column to display pattern
	 static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	 static unsigned char column_sel = 0x7F; // grounds column to display pattern

	
	// === Transitions ===
	switch (state) {
		case sm1_display:    
		break;
		
		default:   	        
		state = sm1_display;
		break;
	}
	
	// === Actions ===
	switch (state) {
		case sm1_display:   // If illuminated LED in bottom right corner
		
				if (column_sel == 0xFE && column_val == 0x80) {
					column_sel = 0x7F; // display far left column
					column_val = 0x01; // pattern illuminates top row
				}
				// else if far right column was last to display (grounded)
				else if (column_sel == 0xFE) {
					column_sel = 0x7F; // resets display column to far left column
					column_val = column_val << 1; // shift down illuminated LED one row
				}
				// else Shift displayed column one to the right
				else {
					column_sel = (column_sel >> 1) | 0x80;
		}
		break;
		
		default:   	        
		break;
	}
	
	PORTC = column_val; // PORTC displays column pattern
	PORTB = column_sel; // PORTB selects column to display pattern

	return state;
};



char getX(){
	return X; 
}
char getY(){
	return Y; 
}

void endGame(){
	end = 0x00; 
	unsigned char _X = 0; 
	unsigned char _Y = 0; 
	
	_X = getX(); 
	_Y = getY(); 
	// left bound
	if (_X == 0x80){
		end = 0x01; 
	}
	// right bound
	else if (_X == 0x01){
		end = 0x01; 
	}
	// up bound
	else if (_Y == 0x80){
		end = 0x01; 
	}
	else if (_Y == 0x01){
		end = 0x01; 
	}
	else {
		end = 0x00; 
	}
}

enum Game_States{First, Init, Start, Wait, Left, LeftR, Right, RightR, Up, UpR, Down, DownR, MoveL, MoveR, MoveUp, MoveDown, Reset, ResetR};
	
int Game_Tick(int state){

unsigned char temp_X;
	
	
	//transitions: 
	switch(state){
		case First: 
		state = Init; 
		break; 
		
		// init state - wait for start btn to be pressed
		case Init: 
			if ((~PINA & 0x02))
			{
				state = Start;
				break;
			}
			else if (!(~PINA & 0x02))
			{
				state = state;
				break; 
			}
		
		//wait for start btn to be released: 
		case Start:
			 if (~PINA & 0x02)
			 {
				 state = state;
			 }
			 else if (!(~PINA & 0x02))
			 {
				 state = Wait; 
			 }
		break; 
		
		// wait for one of the movement buttons to be pressed: 		
		case Wait: 
			if ((~PINA & 0x40)){
				state = Left;
			}
			else if (~PINA & 0x01){
				state = Right; 
			}
			else if (~PINA & 0x04){
				state = Up; 
			}
			else if (~PINA & 0x20){
				state = Down; }
			
			else{
				state = state; 
			}
		break; 
		
		// wait for left to be released: 
		case Left:
		if (!(~PINA & 0x40)){
			state = LeftR; 
		} 
		else if (~PINA & 0x10){
			state = Reset;
		}
		else if (~PINA & 0x40){
			state = state; 
	}		break;

		// wait for Right to be released:
		case Right:
		if (!(~PINA & 0x01)){
			state = RightR;
			break; 
		}
		else if (~PINA & 0x10){
			state = Reset;
		}
		else if (~PINA & 0x01){
			state = state;
			break; 
		}
		
		case Up: 
		if (!(~PINA & 0x04)){
			state = UpR; 
		}
		else if (~PINA & 0x10){
			state = Reset;
		}
		else
		{
			state = state; 
		}
		break; 
		
		case Down: 
		if (!(~PINA & 0x20)){
			state = DownR; 
		}
		else if (~PINA & 0x10){
			state = Reset;
		}
		else
		{
			state = state; 
		}
		break; 
		
		case LeftR:
		state = MoveL; 
		break; 
		
		case RightR: 
		state = MoveR; 
		break; 
		
		case UpR: 
		state = MoveUp; 
		break; 
		
		case DownR:
		state = MoveDown; 
		break; 
		
		case MoveL: 
		if (~PINA & 0x01){
			state = Right; 
		}
		else if (~PINA & 0x04){
			state = Up; 
		}
		else if (~PINA & 0x20){
			state = Down; 
		}
		else if (~PINA & 0x10){
			state = Reset;
		}
		else{
			state = state; 
		}
		break; 
		
		case MoveR: 
		if (~PINA & 0x40){
			state = Left; 
		}
		else if (~PINA & 0x04){
			state = Up; 
		}
		else if (~PINA & 0x20){
			state = Down; 
		}
		else if (~PINA & 0x10){
			state = Reset;
		}
		else {
			state = state; 
		}
		break; 
		
		case MoveUp: 
		if (~PINA & 0x40){
			state = Left; 
		}
		else if (~PINA & 0x01){
			state = Right; 
		}
		else if (~PINA & 0x20){
			state = Down; 
		}
		else if (~PINA & 0x10){
			state = Reset;
		}
		else{
			state = state; 
		}
		break; 
		
		case MoveDown: 
		if (~PINA & 0x40){
			state = Left; 
		}
		else if (~PINA & 0x01){
			state = Right;
		}
		else if (~PINA & 0x04){
			state = Up; 
		}
		else if (~PINA & 0x10){
			state = Reset;
		}
		else
		{
			state = state; 
		}
		break; 
		
		case Reset: 
		if (!(~PINA & 0x10)){
			state = ResetR; 
		}
		else if (~PINA & 0x10){
			state = state; 
		}
		break; 
		
		case ResetR: 
		state = Start; 
		break; 
		
	} // end transition switch
	//actions
	switch(state)
	{
		case First: 
		tempD = 0x01;
		PORTD = tempD;
		
		break; 
		
		case Init: 
		tempD = 0x02; 
		PORTD = tempD; 
		PORTC = Y;
		PORTB = X;
// 		PORTC = Y;
// 		PORTB = X;
		break; 
		
		case Start: 
		tempD = 0x04; 
		PORTD = tempD; 
		PORTC = Y;
		PORTB = X;
		break; 
		
		case Wait: 
		tempD = 0x08; 
		PORTD = tempD; 
		PORTC = Y;
		PORTB = X;
		break; 
		
		case MoveL: 
		endGame(); 
		if (!(end)){
			temp_X = getX();
			temp_X = X << 1;
			temp_X = temp_X | 0x01; 
			X = temp_X; 		
		}
			PORTC = Y;
			PORTB = X;
		break; 
		
		case MoveR:
		endGame();
		if (!(end)){
			temp_X = getX();
			temp_X = X >> 1; 
			temp_X = temp_X | 0x80; 
			X = temp_X; 
		}
		PORTC = Y;
		PORTB = X;
		break; 
		
		case MoveUp: 
		endGame(); 
		if (!end){
			Y = Y << 1; 
		}
		PORTC = Y;
		PORTB = X; 
		break; 
		
		case MoveDown:
		endGame(); 
		if (!(end)){
			Y = Y >> 1; 
		}
		PORTC = Y; 
		PORTB = X; 
		break; 
		
		case Left: 
		tempD = 0x80;
		PORTD = tempD;
		PORTC = Y;
		PORTB = X;
		break; 
		
		case Right: 
		PORTC = Y;
		PORTB = X;
		break; 
		
		case Up: 
		PORTC = Y;
		PORTB = X;
		break; 
		
		case Down: 
		PORTC = Y;
		PORTB = X;
		break; 
		
		case LeftR: 
		PORTC = Y;
		PORTB = X;
		break; 
		
		case RightR: 
		PORTC = Y;
		PORTB = X;
		break; 
		
		case UpR: 
		PORTC = Y;
		PORTB = X;
		break; 
		
		case DownR: 
		PORTC = Y;
		PORTB = X;
		break; 
		
		case Reset: 
		break; 
		
		case ResetR: 
			Y = 0x10; //PORTC
			X = 0xEF; //PORTB
		break; 
	}
	
	return state; 
}	



int main(void)
{
	DDRC = 0xFF; PORTC = 0x00; // output Matrix Y
	DDRB = 0xFF; PORTB = 0x00; // output Matrix X
	DDRA = 0x00; PORTA = 0xFF; // input Joystick Voltage
	DDRD = 0xFF; PORTD = 0x00; // output LED

	//define input buttons
	start_btn = (~PINA & 0x02); //A1
	
	left_btn = (~PINA & 0x40); //A6
	right_btn = (~PINA & 0x01); //A0
	up_btn = (~PINA & 0x04); //A3
	down_btn = (~PINA & 0x20); //A5
	
	rst_btn = (~PINA & 0x10); // reset

	//ADC_init();

	

	unsigned long int SMTick1_calc = 500; //

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick1_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	
		// init X and Y;
		Y = 0x10;  //PORTC
		X = 0xEF; //PORTB

	static task task1;
	task *tasks[] = { &task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// 	// Task 1
// 	task1.state = sm1_display;//Task initial state.
// 	task1.period = SMTick1_period;//Task Period.
// 	task1.elapsedTime = 0;//Task current elapsed time.
// 	task1.TickFct = &SM1_Tick;//Function pointer for the tick.

	task1.state = First; 
	task1.period = SMTick1_period; 
	task1.elapsedTime = 0; 
	task1.TickFct = &Game_Tick; 

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
    unsigned short i; // Scheduler for-loop iterator
    while(1) {
	    // Scheduler code
	    for ( i = 0; i < numTasks; i++ ) {
		    // Task is ready to tick
		    if ( tasks[i]->elapsedTime == tasks[i]->period ) {
			    // Setting next state for task
			    tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			    // Reset the elapsed time for next tick.
			    tasks[i]->elapsedTime = 0;
		    }
		    tasks[i]->elapsedTime += 1;
			
			// 64 - default 
			// left - 68 X
			// right - ? X
			// up - ?  Y
			// down - 66 Y
			// 254 
// 			
// 			#define MAX 64
// 			unsigned short analogToDigital = ADC;
// 			unsigned char dir = 0;
// 
// 			if ((~PINA & 0x01) && (analogToDigital >= MAX))
// 			{
// 				dir = 0x02; //down
// 			}
// 			else if ((~PINA & 0x01) && (analogToDigital < MAX)){
// 				dir = 0x01; // up
// 			}
// 			else if ((~PINA & 0x02) && (analogToDigital < MAX)){
// 				dir = 0x03; // right
// 			}
// 			else if ((~PINA & 0x02) && (analogToDigital >= MAX)){
// 				dir = 0x04; // left
// 			}
// 			else {
// 				dir = 0; 
// 			}
// 
// 			
// 			PORTD = dir;
			
			
	    }
	    while(!TimerFlag);
	    TimerFlag = 0;
    }

    // Error: Program should not exit!
    return 0;
}
