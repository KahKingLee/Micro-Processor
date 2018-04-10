#include <at89x52.h>
#include <intrins.h>
#define speed 1000 //(unit: mini-second)
#define timerlimit 1000 //(unit: micro)
#define RS P1_0
#define RW P1_1
#define E  P1_2
#define DB P2

int inkey, lastkey, k_scan =0; //keyboard scanning
int year,hr,min,sec;
int month = 1,day = 1;
char string1[13] = {"0000Y 01M 01D"};
char string2[11] = {"AM 00:00:00"};
int flash; // to flash the light
int msec; //to count how long for delay
int add_sub; //0 no work, 1 addition, 2 subtraction
int setting; //0 start, 1~4 year, 5~6 month, 7~8 day, 9 am,pm, 10~11 hr, 12~13 min, 14~15 sec			 
//AD.1998Y 12M 01D				  
//AM 23:59:59				 


int keyboard(void);
void wordscan(void);
void init(void);
void position(char line,column);
void wrins(char instruction);
void display(char *string);
void wrdata(char d);
void intrpt_init(void);
void intrpt_timer1(void);
void delay(int in_msec);
void clock_mode(void);
void clock_display(void);
void control_mode(void);
void count(void);
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void main()
{
  	intrpt_init();
	init();
	position(1,1);
	wrdata('A');
	wrdata('D');
	wrdata('.');
  	while(1){
		wordscan();
		if(setting == 0){
		sec++;
			clock_mode();
			delay(speed);
		}
		else{
			count();
			add_sub = 0;
		}
		clock_display();
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void intrpt_init(void){	
	IE = 0x88;	   //Timer1 enable
	TMOD = 0x10;   //Timer1 use mode 1
}
void intrpt_timer1(void) interrupt 3
{
	msec--;
	TH1 = (65536-timerlimit)/256; //set initial munber of timer1 = 1ms(1000us)
	TL1 = (65536-timerlimit)%256;
	TR1 = 1;  //open timer
	wordscan();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void count(void){ 
	//0 start, 1~4 year, 5~6 month, 7~8 day, 9 am,pm, 10~11 hr, 12~13 min, 14~15 sec	 
	int i,j =1;
	if(add_sub == 1){ //add
		switch(setting){   
			case 1:	// year
			case 2:
			case 3:
			case 4:											
				for(i = setting; i<4; i++)j = j*10;				
				year = (year + j)%10000;
				break;
			case 5:	// month
				if(month < 3)month = month + 10;
				else if(month > 10)month = month - 10;
				break;
			case 6:	
				month = month + 1;
				if(month == 13)month = 1; 						
				break;
			case 7:	// day
			case 8:	
				for(i = setting; i<8; i++)j = j*10;
				day = day + j;
				control_mode();
				break;
			case 9:	// am,pm
				 if(hr < 12)hr = hr + 12;
				 else hr = hr - 12;
				 break;
			case 10:// hr
				if((hr%12) < 2)hr = hr + 10;
				else if((hr%12) > 9)hr = hr - 10;
				break;
			case 11:
				if(hr >= 12) hr = (hr + 1)%12 + 12;
				else hr = (hr + 1)% 12;
				break;
			case 12: //min
			case 13:
				for(i = setting; i<13; i++)j = j*10;
				min = (min + j)%60; 
				break;
			case 14: //sec
			case 15: 
				for(i = setting; i<15; i++)j = j*10;
				sec = (sec + j)%60;
				break;
		}
	}
	else if (add_sub == 2){	//sub		
		switch(setting){ 
			case 1:	// year
			case 2:
			case 3:
			case 4:											
				for(i = setting; i<4; i++)j = j*10;				
				year = (10000 - j + year)%10000;
				break;
			case 5:	// month
				if(month < 3)month = month + 10;
				else month = month % 10;
				break;
			case 6:	
				month --;
				if(month == 0)month = 12;
				break;
			case 7:	// day
			case 8:	
				for(i = setting; i<8; i++)j = j*10;
				day = day - j;
				control_mode();
				break;
			case 9:	// am,pm
				 if(hr<12)hr = hr + 12;
				 else hr = hr - 12;
				 break;
			case 10:	// hr
				if((hr%12) < 2)hr = hr + 10;
				else if((hr%12) > 9)hr = hr - 10;
				break;
			case 11:
				if(hr >= 12) hr = (hr - 1)%12 + 12;
				else hr = hr - 1;
				if(hr < 0)hr = 11;
				break;
			case 12: //min
			case 13:
				for(i = setting; i<13; i++)j = j*10;
				min = min - j; 
				if(min < 0)min = min + 60;
				break;
			case 14: //sec
			case 15: 
				for(i = setting; i<15; i++)j = j*10;
				sec = sec - j;
				if(sec < 0) sec = sec + 60;
				break;				 
		}	
	}	
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void control_mode(void){
	switch(month){
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if(day< 1) day = 31;
			else if (day>31) day = 1;
			break;
		case 2:
			if((year%4==0)&&(year%100!=0)||(year%400==0)){
				if(day < 1) day = 29;
				else if(day > 29) day = 1;
			}
			else if(day < 1) day = 28;
			else if(day > 28) day = 1;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if(day< 1)day = 30;
			else if (day > 30) day = 1;
			break;		   
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void init(void)
{
  delay(30);
  wrins(0x38);
  wrins(0x38);
  wrins(0x38);
  wrins(0x38);

  wrins(0x08);
  wrins(0x01);

  wrins(0x06);
  wrins(0x0c);
}


void position(char line,column)
{
  unsigned char instruction;
  line--;
  column--;
  instruction=0x80+(0x40*line + column);
  wrins(instruction);
} 


void wrins(char instruction)
{
  RS=0;
  RW=0;
  E=0;
  delay(1);
  E=1;
  DB=instruction;
  delay(1);
  E=0;
  delay(8);
}


void display(char *string)
{
  char k=0;
  while(string[k] !=0x00 )
   {
     wrdata(string[k]);
     k++;
   }
}

 
void wrdata(char d)
{
  RS=1;
  RW=0;
  E=0;
  delay(1);
  E=1;
  DB=d;
  delay(1);
  E=0;
  delay(1);
}				
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void clock_mode(void){
	//-------------------------------------clock mode------------------------------------
	if(sec>59){
		sec = 0;
		min++;
	}
	if(min>59){
		min = 0;
		hr++;
	}
	if(hr>23){
	hr = 0;
	day++;
	}
	switch(month){
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
			if(day>31){
				day = 1;
				month++;
			}
			break;
		case 2:
			if(day - ((year%4==0)&&(year%100!=0)||(year%400==0)) > 28){
				day = 1;
				month++;
			}
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if(day>30){
				day = 1;
				month++;
			}
			break;
		case 12:
			if(day>31){
				day = 1;
				month = 1;
				year ++;
			}
			break;
		default:
			month = 1;
			year ++;
	}
	if(year>9999)year = 0;				 
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void clock_display(void){
	//string1[13] = "0000Y 01M 01D";
	//string2[11] = "AM 00:00:00";	
	int i,j;
	j = year;
	for(i=0; i<4; i++, j/=10){
		string1[3-i] = j%10 + '0';
	}
	j = month;
	for(i=0; i<2; i++, j/=10){
		string1[7-i] = j%10 + '0';
	}
	j = day;
	for(i=0; i<2; i++, j/=10){
		string1[11-i] = j%10 + '0';
	}
	j = hr;
	if(j<12)string2[0] = 'A';
	else{
		string2[0] = 'P';
		j=j-12;
	}
	j++;
	for(i=0; i<2; i++, j/=10){
		string2[4-i] = j%10 + '0';
	}
	j = min;
	for(i=0; i<2; i++, j/=10){
		string2[7-i] = j%10 + '0';
	}
	j = sec;
	for(i=0; i<2; i++, j/=10){
		string2[10-i] = j%10 + '0';
	}
	position(1,4);
	display(string1);
	position(2,1);
	display(string2);
	//-----------------------------------flash------------------------------------------
	if(setting != 0){
		delay(200);
		//0 start, 1~4 year, 5~6 month, 7~8 day, 9 am,pm, 10~11 hr, 12~13 min, 14~15 sec	 
		switch(setting){
			case 1:
				string1[0] = ' ';
				break;
			case 2:
				string1[1] = ' ';
				break;
			case 3:	
				string1[2] = ' ';
				break;
			case 4:	
				string1[3] = ' ';
				break;
			case 5:
				string1[6] = ' ';
				break;
			case 6:
				string1[7] = ' ';
				break; 
			case 7:
				string1[10] = ' ';
				break;
			case 8:
				string1[11] = ' ';
				break;
			case 9:
				string2[0] = ' ';
				break;
			case 10:
				string2[3] = ' ';
				break;
			case 11: 
				string2[4] = ' ';
				break;
			case 12:
				string2[6] = ' ';
				break;
			case 13: 
				string2[7] = ' ';
				break;
			case 14:
				string2[9] = ' ';
				break;
			case 15:
				string2[10] = ' ';
				break;
		}
		position(1,4);
		display(string1);
		position(2,1);
		display(string2);
		delay(200);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void wordscan(void){ 
	lastkey = inkey;
	inkey = keyboard();
	if((inkey!=0xff)&&(lastkey==0xff)){
		switch(inkey){ 
			case 2:	 //stop	(setting mode)
				setting = 1;
				break;
			case 3: //start
				setting = 0;
				break;
			case 6:	 //last 
				setting --;
				if(setting < 1) setting = 15;
				break;
			case 7:	 //next
				setting ++;
				if(setting > 15) setting = 1;
				break;
			case 10://add 1
				add_sub = 1;
				break;
			case 14://decrease 1
				add_sub = 2;
				break;
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
int keyboard(void){	
	int i;
	int key = 0xff;
	P0 = 0xef;						 
	for(i=0;i<4;i++){
		if(P0_0 == 0) key = 4*i+0;
		if(P0_1 == 0) key = 4*i+1;
		if(P0_2 == 0) key = 4*i+2;
		if(P0_3 == 0) key = 4*i+3;
		P0 = ~P0;
		P0 = _crol_(P0,1);
		P0 = ~P0;
	}
	return key;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void delay(int in_msec)
{
	msec = in_msec;
	TH1 = (65536-timerlimit)/256; //set initial munber of timer1 = 1ms(1000us)
	TL1 = (65536-timerlimit)%256;
	TR1 = 1; //open timer
	while(msec!=0);
	TR1 = 0; //close timer
}			 