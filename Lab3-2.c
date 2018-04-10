#include <AT89X52.h>
#include <intrins.h>
#define RS P1_0
#define RW P1_1
#define E  P1_2
#define DB P2
#define RR(x)_cror_(x,1)
#define RL(x)_cror_(x,1)
void init(void);
void wrcgram(void);
void position(char line,column);
void wrins(char instruction);
void display(char *string);
void wrdata(char d);
void delay10ms(unsigned int time);
char pattern[] = { 	0x01,0x03,0x07,0x0f,
										0x1F,0x03,0x03,0x03,
										0x10,0x18,0x1C,0x1E,
										0x1F,0x18,0x18,0x18 
									};
code char string1[] = {"INT 0 happens "};									//0~13
code char string3[] = {"INT 1 happens "};	
char string2[] = {0x08,0x09,"NTUST is the best"};					//0~18
									
main(){
	char i,j;
	IE = 0x85;														//INT0,INT1 as IE(Interrupt Enable)=10000101
	TCON = 5;															//INT0,INT1 as low level triggering
	//PX0 = 1;															//INT0 has the first priority
	IT0 = 0;															//set INT0 interrupt type(1:Falling 0:Low level)
	//IT1 = 0;
	EX0 = 1;															//enable INT0 interrupt
	EA = 1;																//open global interrupt switch
	PX0 = 1;															//INT0 has the first priority
	init();
  wrcgram();
	position(1,1);
	while(1){
		for(i=0;i<19;i++){
			init();
			for(j=0;j<16;j++){
				wrdata(string2[(i+j)%19]);
				//delay10ms(5);
			}
			delay10ms(100);
		}
	}
}

void EXINT0(void) interrupt 0		//INT0 Interrupt function
{
	int i,j,k;
	wrins(0x01);
	
	for(i=k;i<k+20;i++){
		init();
		for(j=0;j<16;j++){
			wrdata(string1[(i+j)%14]);
			//delay10ms(10);
		}
		delay10ms(10);
	}
	k=i;												//return value
}

void EXINT1(void)interrupt 2     //INT1 Interrupt function
{
	int i,j,k;
	wrins(0x01);
	
	for(i=k;i<k+20;i++){
		init();
		for(j=0;j<16;j++){
			wrdata(string3[(i+j)%14]);
			//delay10ms(10);
		}
		delay10ms(10);
	}
	k=i;		
}

void init(void)
{
  delay10ms(30);
  wrins(0x38);
  wrins(0x38);
  wrins(0x38);
  wrins(0x38);

  wrins(0x08);
  wrins(0x01);

  wrins(0x06);
  wrins(0x0c);
}

/*void wrcgram(void)
{
  char m;
	int k;
	for(k=0;k<35;k++){
  wrins(0x40);
  for(m=16; m>=0; m--)
    {
			position(1,m);
      wrdata(pattern[m+k]);
    }
	}
}*/

void wrcgram(void)
{
	char m;
	wrins(0x40);
	for(m=0;m<19;m++){
		wrdata(pattern[m]);
	}
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
  delay10ms(1);
  E=1;
  DB=instruction;
  delay10ms(1);
  E=0;
  delay10ms(8);
}

void display(char *string)
{
  char k=0;
  while(string[k] != 0x00 )
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
  delay10ms(1);
  E=1;
  DB=d;
  delay10ms(1);
  E=0;
  delay10ms(1);
}

void delay10ms(unsigned int time)
{
  unsigned int n;
  while(time>0)
   {
     n=120;
     while(n>0)  n--;
     time--;
   }
}  