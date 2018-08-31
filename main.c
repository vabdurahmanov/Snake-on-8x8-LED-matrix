#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <ucr/timer.h>
// BUTTONS:
#define start_btn ~PINA & 0x02 // A1
#define reset_btn ~PINA & 0x08 // A3
#define right_btn ~PINA & 0x01 // A0
#define left_btn ~PINA & 0x20 // A5
#define up_btn ~PINA & 0x04 // A2
#define down_btn ~PINA & 0x10 // A4

unsigned char tempBoardArray[8][8];
unsigned char rowData[8];
unsigned char columnData[8];
unsigned char start; // boolean for start btn press
unsigned char reset; //boolean for reset btn press
unsigned char end;  // boolean for end game
unsigned short score; 
unsigned char gameover; 
unsigned char foodX; 
unsigned char foodY; 
unsigned char gamerunning; 
unsigned char highscore; 
unsigned char input; 
unsigned char direction, left_dir, right_dir, up_dir, down_dir; // direction booleans
unsigned char eaten; // is food eaten? 

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
	end = 0x00;
	unsigned char x = 0x00;
	unsigned char y = 0x00;
	
	x = getX(head);
	y = getY(head);
	// left bound
	if (x == 0x80){
		end = 0x01;
	}
	// right bound
	else if (x == 0x01){
		end = 0x01;
	}
	// up bound
	else if (y == 0x80){
		end = 0x01;
	}
	else if (y == 0x01){
		end = 0x01;
	}
	else {
		end = 0x00;
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

void grow(){
	
	unsigned char a, p; 
	unsigned char oldSize; 
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
	
}

void generateFood(){
foodX = 0x08; 
foodY = 0x02; 
}

void checkifeatenfood(){
	if((foodX == head.xcoord) && (foodY == head.ycoord)){
		grow(); 
		eaten = 1; 
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

void newGame(){
	InitSnake(); // draw snake; 
	score = 0x00; // null the score
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
		break; 
		
		case Reset: 
			state = WaitMove; 
		break; 
		
		case EndGame: 
			if (start_btn){
				state = WaitMove; 
			}		
			else if (reset_btn){
				state = WaitMove; 
			}
			else{
				state = state; 
			}
		}

	//actions:
	switch(state){
		case InitGame:
		break;
		
		case WaitMove: 
					newGame();
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
		break; 
		
		case EndGame: 
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


int main(void)
{
	// Set Data Direction Registers
	DDRA = 0x00; PORTA = 0xFF; // input buttons
	DDRB = 0xFF; PORTB = 0x00; // matrix columns
	DDRC = 0xFF; PORTC = 0x00; // matrix rows
	


	// Period for the tasks
	//unsigned long int timeKeeper_calc = 200; 
// 	unsigned long int inputProcessor_calc = 100;
	unsigned long int move_calc = 100;
	unsigned long int matrix_calc = 1; 
	unsigned long int food_period_calc = 1; 

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	
// 	tmpGCD = findGCD(inputProcessor_calc, move_calc);
	tmpGCD = findGCD(move_calc, matrix_calc);
	tmpGCD = findGCD(tmpGCD, food_period_calc); 
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
// 	unsigned long int inputProcessor_period = inputProcessor_calc/GCD;
	unsigned long int move_period = 200;
	unsigned long int matrix_period = 1;
	unsigned long int food_period = 50; 
	
	//Declare an array of tasks
	static task task3, task4;
	task *tasks[] = { &task3, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// 	TimeKeeper
// 		task1.state = Init;//Task initial state.
// 		task1.period = timeKeeper_period;//Task Period.
// 		task1.elapsedTime = 0;//Task current elapsed time.
// 		task1.TickFct = &timekeeper_tick;//Function pointer for the tick.
// 
// 	// Input Processor
// 	task2.state = ;//Task initial state.
// 	task2.period = inputProcessor_period;//Task Period.
// 	task2.elapsedTime = 0;//Task current elapsed time.
// 	task2.TickFct = &input_tick;//Function pointer for the tick.

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

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();

	unsigned short i; // Scheduler for-loop iterator

    while (1) 
    {		
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

