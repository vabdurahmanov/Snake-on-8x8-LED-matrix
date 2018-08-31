#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "io.h"
#include "io.c"
#include <ucr/timer.h>

// BUTTONS:
#define start_btn ~PIND & 0x02 // D1
#define reset_btn ~PIND & 0x08 // D3
#define right_btn ~PIND & 0x01 // D0
#define left_btn ~PIND & 0x20 // D5
#define up_btn ~PIND & 0x04 // D2
#define down_btn ~PIND & 0x10 // D4

unsigned char tempBoardArray[8][8];
unsigned char foodcoords;
unsigned char start = 0; // boolean for start btn press
unsigned char reset = 0; //boolean for reset btn press
unsigned char end = 0;  // boolean for end game
unsigned char pscore = 0; 
unsigned char gameover = 0; 
unsigned char foodX = 0; 
unsigned char foodY = 0; 
unsigned char seed = 0; 
unsigned char gamerunning = 0; 
unsigned char highscore = 0; 
unsigned char eaten = 0; // is food eaten? 

// General structure of grid objects
typedef struct _gridObject{
	unsigned char xcoord;
	unsigned char ycoord;
	unsigned char size;
} gridObject;

gridObject head;
gridObject food;
gridObject body[64];
gridObject board[64];

void IncrementScore(){
	pscore = pscore + 1;
}

// Get X coordinate
unsigned char getX(gridObject o){
	return o.xcoord;
}
// Get Y coordinate
unsigned char getY(gridObject o){
	return o.ycoord;
}
// CHECK FOR END OF GAME
void endGame(){
	
	unsigned char x = 0x00;
	unsigned char y = 0x00;
	
	x = getX(head);
	y = getY(head);
	// left bound
	if (x == 0x80){
		end = 1;
	}
	// right bound
	else if (x == 0x01){
		
		end = 1;

	}
	// up bound
	else if (y == 0x80){
		
		end = 1;


	
	}
	else if (y == 0x01){
		end = 1;
	}
	else {
		end = 0;
	}
}
// SET initial SNAKE COORDS - randomize in the future; 
void InitSnake(){
	unsigned char tempX = 0xDF; 
	head.xcoord = (~(tempX) & 0xFF); 
	head.ycoord = 0x10; 
	body[0].xcoord = (~(tempX) & 0xFF); 
	body[0].ycoord = 0x08;
	body[1].xcoord = (~(tempX) & 0xFF); 
	body[1].ycoord = 0x04;  
	body[2].xcoord = (~(tempX) & 0xFF); 
	body[2].ycoord = 0x02; 
	
	body->size = 3;

}


void initFood(){
	foodX = 0x08; 
	foodY = 0x02; 
}


//MOVE functions: 
void moveLeft(){
	
	unsigned char r;
	
	// move head
	unsigned char tempHeadOldX = getX(head);
	unsigned char tempHeadOldY = getY(head);
	
	head.xcoord = tempHeadOldX << 1;// body[0]
	head.ycoord = tempHeadOldY; // body[0]


	// move body - each block first up by 1 block, then 1 right by 1 block until reached the length of body
	
	
	for (r = 0; r < (body->size); r++)
	{
		unsigned char bodytempX;
		unsigned char bodytempY;
		// store old coordinates of body[r]
		bodytempY = body[r].ycoord;
		bodytempX = body[r].xcoord;
		// assign new coordinates
		body[r].ycoord = tempHeadOldY;
		body[r].xcoord = tempHeadOldX;
		// reassign temp values
		tempHeadOldX = bodytempX;
		tempHeadOldY = bodytempY;
	}
	
	
}
void moveRight(){
	
	unsigned char r, t;
	
	// move head
	unsigned char tempHeadOldX = getX(head);
	unsigned char tempHeadOldY = getY(head);

	
	head.xcoord = (tempHeadOldX >> 1); // body[0]
	head.ycoord = tempHeadOldY; // body[0]

	// move body - each block first up by 1 block, then 1 right by 1 block until reached the length of body
	
	
	for (r = 0; r < (body->size); r++)
	{	
		unsigned char bodytempX;
		unsigned char bodytempY;
		// store old coordinates of body[r]
		bodytempY = body[r].ycoord;
		bodytempX = body[r].xcoord; 
		// assign new coordinates
		body[r].xcoord = tempHeadOldX; 
		body[r].ycoord = tempHeadOldY;
		// reassign temp values
		tempHeadOldX = bodytempX; 
		tempHeadOldY = bodytempY;

	}
	
}
void moveUp(){
	
	unsigned char r;
	
	// move head
	unsigned char tempHeadOldX = getX(head);
	unsigned char tempHeadOldY = getY(head);

	
	head.xcoord = tempHeadOldX; // body[0]
	head.ycoord = tempHeadOldY << 1; // body[0]

	// move body - each block first up by 1 block, then 1 right by 1 block until reached the length of body
	
	
	for (r = 0; r < (body->size); r++)
	{
		unsigned char bodytempX;
		unsigned char bodytempY;
		// store old coordinates of body[r]
		bodytempY = body[r].ycoord;
		bodytempX = body[r].xcoord;
		// assign new coordinates
		body[r].ycoord = tempHeadOldY;
		body[r].xcoord = tempHeadOldX;
		// reassign temp values
		tempHeadOldX = bodytempX;
		tempHeadOldY = bodytempY;
		}
}
void moveDown(){
	
	unsigned char r;
	
	// move head
	unsigned char tempHeadOldX = getX(head);
	unsigned char tempHeadOldY = getY(head);

	
	head.xcoord = tempHeadOldX; // body[0]
	head.ycoord = tempHeadOldY >> 1; // body[0]

	// move body - each block first up by 1 block, then 1 right by 1 block until reached the length of body
	
	
	for (r = 0; r < (body->size); r++)
	{
		unsigned char bodytempX;
		unsigned char bodytempY;
		// store old coordinates of body[r]
		bodytempY = body[r].ycoord;
		bodytempX = body[r].xcoord;
		// assign new coordinates
		body[r].ycoord = tempHeadOldY;
		body[r].xcoord = tempHeadOldX;
		// reassign temp values
		tempHeadOldX = bodytempX;
		tempHeadOldY = bodytempY;
	}
}

void clearEatenFood(){
	foodY = 0x00;
}

void grow(){
	
	unsigned char a, p; 
	unsigned char tempHX; 
	unsigned char tempHY; 
	
	tempHX = head.xcoord; // save old head's coordinates
	tempHY = head.ycoord; // save old head's coordinates
	
	// save old body's coordinates in the array
	gridObject oldbodycoords[body->size]; 

	for (a = 0; a < body->size; a++){
		oldbodycoords[a].xcoord = getX(body[a]); 
		oldbodycoords[a].ycoord = getY(body[a]);
	}
	
	// increase body size
	(body->size)++; 
	
	body[0].xcoord = tempHX; 
	body[0].ycoord = tempHY; 
	
	for (p = 1; p < body->size - 1; p++){
	body[p].xcoord = getX(oldbodycoords[p]);
	body[p].ycoord = getY(oldbodycoords[p]); 
	}
	
	// reassign head:
	head.xcoord = foodX;
	head.ycoord = foodY;
	clearEatenFood();
	
}

void generateFood(){


foodcoords = rand() % 4;
switch(foodcoords){
	
	 case 0:
	 foodX = 0x40;
	 foodY = 0x40;
	 break;
	
	case 1:
	foodX = 0x10;
	foodY = 0x20;
	break;
	
	case2:
	foodX = 0x04;
	foodY = 0x08;
	break; 
	
	case 3: 
	foodX = 0x20;
	foodY = 0x04;
	break;
	
}

eaten = 0; 



}

void checkifeatenfood(){
	if((foodX == head.xcoord) && (foodY == head.ycoord)){
		grow(); 
		eaten = 1; 
		IncrementScore();
		generateFood(); 
		
	}
}

// update matrix board
void updateMatrix(){
unsigned char w, s, b; 
for (w = 0; w < 8; w++){
	for (s = 0; s < 8; s++){
 		PORTB = (~(head.xcoord));
		PORTC = head.ycoord;
		for (b = 0; b < body->size; b++){
			PORTB = (~(body[b].xcoord));
			PORTC = body[b].ycoord;
		}
	}
}
PORTB = ~foodX;
PORTC = foodY;
}

// custom character functions
void LCD_build(unsigned char location, unsigned char *ptr){
	unsigned char i;
	if(location<8){
		LCD_WriteCommand(0x40+(location*8));
		for(i=0;i<8;i++)
		LCD_WriteData(ptr[i]);
	}
}

void makeCustomChar() {
	static unsigned char foodpiece[8] = {
		 0b00011,
		 0b00110,
		 0b00100,
		 0b01110,
		 0b11011,
		 0b10001,
		 0b11011,
		 0b01110
	};
	LCD_build(0, foodpiece);
}


// EEPROM functions 
void saveScore (unsigned char s){
	eeprom_write_byte(0, s); 
}
unsigned char loadScore(){
	unsigned char h = eeprom_read_byte(0); 
	if (h == 255){
		highscore = 0;
	}
	else{
		highscore = h; 
	}
	
	return highscore; 
}



void loadCustomChar() {
	LCD_WriteData(0);
}

//--------Find GCD function --------------------------------------------------
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
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;


enum foodStates{Init6, food_present, nofood};
int food_tick(int state){
	//transitions 
	switch(state){
		case Init6: 
			if (eaten){
				state = nofood; 
			}
			else
			{
				state = food_present; 
			}
		break; 
		
		case food_present: 
		if (!eaten){
			state = state; 
		}
		else
		{
			state = nofood; 
		}
		break; 
		
		case nofood:
		if (eaten)
		{
			state = state; 
		}
		else{
			state = food_present;
		}
		break; 
	}
		switch(state){
			
			case Init6:  
			break; 
			
			case food_present: 
			break; 
			
			case nofood: 
			generateFood(); 
			break; 
		}
	return state; 
}


// GAME TASK
enum move_states {InitGame, WaitMove, Left, LeftR, Right, RightR, Up, UpR, Down, DownR, EndGame, Reset};
int move_tick(int state) {	
	//transition: 
	switch(state){
		case InitGame: 
			if (start_btn){
				state = WaitMove; 
			}
			else{
				state = state; 
			}
		break; 
				
		case WaitMove: 
			if (left_btn){
				state = Left; 
			}
			else if (right_btn){
				state = Right; 
			}
			else if (up_btn){
				state = Up;
			}
			else if(down_btn){
				state = Down; 
			}
			else if (reset_btn){
				state = Reset; 
			}
			else{
				state = state; 
			}
		break; 
	
		case Left: 
		if (!(left_btn)){
			state = LeftR; 
		}
		else{
			state = state; 
		}
		break; 
	
		case LeftR:
		if (!end){
			if (right_btn){
				state = Right; 
			}
			else if (up_btn){
				state = Up; 
			}
			else if (down_btn){
				state = Down; 
			}
			else if (reset_btn){
				state = Reset; 
			}
			else{
				state = state; 
			}
		}
		else{
			state = EndGame; 
		}
		break; 
	
		case Right: 
		if (!(right_btn)){
			state = RightR; 
		}
		else{
			state = state; 
		}
		break; 
	
		case RightR:
		if (!end){ 
			if (left_btn){
				state = Left; 
			}
			else if (up_btn){
				state = Up; 
			}
			else if (down_btn){
				state = Down; 
			}
			else if (reset_btn){
				state = Reset;
			}
			else{
				state = state; 
			}
		}
		else{
			state = EndGame; 
		}
		break; 
	
		case Up: 
		if (!(up_btn)){
			state = UpR; 
		}
		else{
			state = state; 
		}
		break; 
	
		case UpR: 
		
		if (!end){
			if(left_btn){
				state = Left;
			}
			else if (right_btn){
				state = Right;
			}
			else if (down_btn){
				state = Down;
			}
			else if (reset_btn){
				state = Reset;
			}
			else{
				state = state;
			}
		}
		else {
			state = EndGame; 
		}
		break; 
	
		case Down: 
			if (!(down_btn)){
				state = DownR; 
			}
			else{
				state = state; 
			}
		break; 
	
		case DownR: 
			if (!end) {
				
				if(left_btn){
				state = Left; 
			}
			else if (right_btn){
				state = Right; 
			}
			else if (up_btn){
				state = Up; 
			}
			else if (reset_btn){
				state = Reset;
			}
			else{
				state = state; 
			}
		}
		else {
			state = EndGame; 
		}
		break; 
		
		case Reset: 
			state = InitGame; 
		break; 
		
		case EndGame: 
			if (start_btn){
				state = InitGame; 
			}		
			else if (reset_btn){
				state = InitGame; 
			}
			else{
				state = state; 
			}
			break; 
		}

	//actions:
	switch(state){
		case InitGame:
		highscore = loadScore();
		InitSnake();
		initFood();
		gameover = 0; 
		break;
		
		case WaitMove: 
		start = 1; 
		break;
		
		case Left:
		break;
		
		case LeftR:
		endGame(); 
		if (!end){
			moveLeft(); 
			checkifeatenfood(); 
		}
		break;
		
		case Right:
		break;
		
		case RightR:
		endGame();
		if (!end){
			moveRight();
			checkifeatenfood(); 
			} 
		break;
		
		case Up:
		break;
		
		case UpR:
		endGame();
		if (!end){
			moveUp(); 
			checkifeatenfood();
			}
		break;
		
		case Down:
		break;
		
		case DownR:
		endGame();
		if (!end){
			moveDown(); 
			checkifeatenfood(); 
		}
		break;
		
		case Reset: 
		reset = 1; 
		break; 
		
		case EndGame: 
		gameover = 1; 
		saveScore(highscore);
		break; 
	}	
	
return state; 
}

// MATRIX TASK
enum matrix_states {Init4, UpdateBoard, Gameover};
int matrix_tick(int state) {
	//transitions
	switch(state){
		case Init4:
		state = UpdateBoard;  
		break; 
				
		case UpdateBoard:
		if (!end){
			state = state; 
		}
		else {
			state = Gameover;
		}
		break; 
		
		case Gameover: 
		PORTB = 0xFF;
		PORTC = 0x00;
		break; 
	}
	// actions
	switch(state){
				
		case Init4:
		break;
			
		case UpdateBoard:
			updateMatrix(); 
		break;
			
		case Gameover:
		break;
	}
	return state;
}

// DISPLAY TASK
enum display_tick {Init5, Instructions, Score, Gameover_LCD};
int display_tick(int state) {
	
	static unsigned char tempScore = 0;
	static unsigned char hundreds = 0;
	static unsigned char tens = 0;
	static unsigned char ones = 0;
	static const unsigned char instr[32] = {
		'P', 'R', 'E', 'S', 'S', ' ', 'G', 'R', 'E', 'E', 'N', ' ', 'B', 'T', 'N', ' ',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
	};
	
	static unsigned char score[32] = {
		'@', ' ', 'S', 'C', 'O', 'R', 'E', ':', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
	};
	
	static unsigned char gameovermes[32] = {
		'G', 'A', 'M', 'E', 'O', 'V', 'E', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		'H', 'I', 'G', 'H', ' ', 'S', 'C', 'O', 'R', 'E', ':', ' ', ' ', ' ', ' ', ' '
	};
	
	
	//transitions
	switch(state){
		case Init5: 
			state = Instructions; 
		break; 
		
		case Instructions: 
			if (start){
				state = Score;
			}
			else{
				state = state; 
			}
		break; 
		
		case Score: 
			if (reset){
				state = Instructions;
			}
			else if (gameover){
				state = Gameover_LCD; 
			}
			else{
				state = state; 
			}
		break; 
		
		case Gameover_LCD: 
		break; 
	}
	//actions
	switch(state){
		case Init5:
		break;
		
		case Instructions:
		for (unsigned char i = 0; i < 32; i++) {
			LCD_Cursor(i + 1);
			LCD_WriteData(instr[i]);
			LCD_Cursor(0);
		}
		break;
		
		case Score:

		tempScore = pscore;
		
		ones = tempScore % 10;
		tempScore = tempScore - ones;
		
		tens = (tempScore % 100) / 10;
		tempScore = tempScore - tens;
		
		hundreds = (tempScore % 1000) / 100;
		tempScore = tempScore - hundreds;
		
		score[9] = '0' + hundreds;
		score[10] = '0' + tens;
		score[11] = '0' + ones;
		
		for (unsigned char i = 0; i < 32; i++) {
			LCD_Cursor(i + 1);
			 if (score[i] == '@') {
				 loadCustomChar();}
			LCD_WriteData(score[i]);
			LCD_Cursor(0);
		}
		break;
		
		case Gameover_LCD:
				
		if (highscore > pscore){
			highscore = highscore;
			tempScore = highscore;}
			else
			{
			highscore = pscore;
			tempScore = highscore;
			}
		
		ones = tempScore % 10;
		tempScore = tempScore - ones;
		
		tens = (tempScore % 100) / 10;
		tempScore = tempScore - tens;
		
		hundreds = (tempScore % 1000) / 100;
		tempScore = tempScore - hundreds;
		
		gameovermes[27] = '0' + hundreds;
		gameovermes[28] = '0' + tens;
		gameovermes[29] = '0' + ones;
		
		for (unsigned char i = 0; i < 32; i++) {
			LCD_Cursor(i + 1);
			LCD_WriteData(gameovermes[i]);
			LCD_Cursor(0);
		}
		break;
	}	
	return state;
}






int main(void)
{
	// Set Data Direction Registers
	DDRA = 0xFF; PORTA = 0x00; // output LCD
	DDRB = 0xFF; PORTB = 0x00; // matrix columns
	DDRC = 0xFF; PORTC = 0x00; // matrix rows
	DDRD = 0xC0; PORTD = 0x3F; // input buttons + LCD


	// Period for the tasks

	unsigned long int move_calc = 200;
	unsigned long int matrix_calc = 1; 
	unsigned long int LCD_Display_calc = 200; 
	unsigned long int food_period_calc = 1; 

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	
	tmpGCD = findGCD(move_calc, LCD_Display_calc);
	tmpGCD = findGCD(tmpGCD, food_period_calc); 
	tmpGCD = findGCD(tmpGCD, matrix_calc); 
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
// 	unsigned long int inputProcessor_period = inputProcessor_calc/GCD;
	unsigned long int move_period = 200;
	unsigned long int matrix_period = 1;
	unsigned long int LCD_Display_period = 200;
	unsigned long int food_period = 50; 
	
	//Declare an array of tasks
	static task task3, task4, task5, task6;
	task *tasks[] = { &task3, &task4, &task5, &task6};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Game Controller
	task3.state = InitGame;//Task initial state.
	task3.period = move_period;//Task Period.
	task3.elapsedTime = 0;//Task current elapsed time.
	task3.TickFct = &move_tick;//Function pointer for the tick.
	
	// Matrix 
	task4.state = Init4;//Task initial state.
	task4.period = matrix_period;//Task Period.
	task4.elapsedTime = 0;//Task current elapsed time.
	task4.TickFct = &matrix_tick;//Function pointer for the tick.
	
	// LCD screen
	task5.state = Init5;//Task initial state.
	task5.period = LCD_Display_period;//Task Period.
	task5.elapsedTime = 0;//Task current elapsed time.
	task5.TickFct = &display_tick;//Function pointer for the tick.
	
	// Generate food 
	task6.state = Init6;//Task initial state.
	task6.period = food_period;//Task Period.
	task6.elapsedTime = 0;//Task current elapsed time.
	task6.TickFct = &food_tick;//Function pointer for the tick.
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
		LCD_init();
		makeCustomChar(); 
		
	unsigned short i; // Scheduler for-loop iterator

    while (1) 
    {
// 		LCD_Cursor(1);
// 		LCD_WriteData('0');
		
		
		
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
			}
			while(!TimerFlag);
			TimerFlag = 0;
    }
	
	// Error: Program should not exit!
	return 0; 
}
