#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <ctype.h>
#include <signal.h>

#define C

#define pi 3.141592653589793238462643383
#define e 2.71828182846

#define STACKSIZE 99
#define PRGMSIZE 1000

#ifdef PC
	#define BTIME 13000
	#define ARROW "%c",17
	#define INTO "$"
	#define LARR "<"
	#define RARR ">"
	#define UARR "^"
	#define DARR "v"

	#define SQUARESYM ":X2"
	#define SQRSYM ":Sqr"
	#define CBRSYM ":Cbr"

	#define ENTER 13
	#define ESC 27
	#define UP 200
	#define DOWN 208
	#define LEFT 203
	#define RIGHT 205
	#define DEL 8
	#define PLUS 43
	#define MULT 42
	#define DIV 47
	#define SUB 45
	#define ALPHA 9
	#define SHIFT 96
	#define CTRL 316
	#define POW 317
	#define LOG 301
	#define LN  302
	#define SIN 303
	#define COS 304
	#define TAN 305
	#define SQUARE 261
	#define XROOT 306
	#define ALOG 307
	#define EXP 308
	#define ASIN 309
	#define ACOS 310
	#define ATAN 311
	#define SQR 319
	#define CBR 313
	#define INV 314
	#define SIGN 318
	#define PI 300
	#define INS 8+256
	#define ON 195
	#define RIGHT 205
	#define EXPO 317
	#define F1 187
	#define F2 188
	#define F3 189
	#define F4 190
	#define F5 191
	#define F6 192
	#define VARS 194
	#define PRGM VARS+256
	#define OPTN 193
	#define indirect 196
#else
	#define BTIME 40
	#define ARROW "%c%c",0xF6,188
	#define INTO "%c%c",0xF6,157
	#define LARR "%c%c",0xF6,156
	#define RARR "%c%c",0xF6,157
	#define UARR "%c%c",0xF6,158
	#define DARR "%c%c",0xF6,159

	#define SQUARESYM ":%c",146
	#define SQRSYM ":%c",203
	#define CBRSYM ":%c%c",184,203

	#define ENTER 156
	#define ESC 129
	#define UP 200
	#define DOWN 208
	#define LEFT 203
	#define RIGHT 205
	#define DEL 211
	#define PLUS 88
	#define MULT 83
	#define DIV 84
	#define SUB 89
	#define ALPHA 184
	#define SHIFT 170
	#define CTRL 157
	#define POW 94
	#define LOG 66
	#define LN 67
	#define SIN 68
	#define COS 69
	#define TAN 70
	#define SQUARE 72
	#define XROOT 94+256
	#define ALOG 66+256
	#define EXP 67+256
	#define ASIN 68+256
	#define ACOS 69+256
	#define ATAN 70+256
	#define SQR 72+256
	#define CBR 73+256
	#define INV 74+256
	#define SIGN 140
	#define PI 210+256
	#define INS 211+256
	#define ON 197
	#define RIGHT 205
	#define EXPO 210
	#define F1 187
	#define F2 188
	#define F3 189
	#define F4 190
	#define F5 191
	#define F6 192
	#define VARS 198
	#define PRGM VARS+256
	#define OPTN 186
	#define indirect 304
#endif

enum SAS {ABS=1,INT,RAND,TODEG,TORAD,GCD,INTDIV,DUP,DUPN,
					ISPRIME,LCM,MOD,CEIL,FLOOR,DROPN,DEPTH,PICK,OVER,
					SINH,COSH,TANH,MAXR,MINR,ROLL,ROLLD,ROT,E,
					XYEQ,XYNE,XYGT,XYLT,XYGE,XYLE,
					X0EQ,X0NE,X0GT,X0LT,X0GE,X0LE,
					STO_i,GTO_i,VAR_i,IND_i,STOP,
					VAR_A,VAR_Z=VAR_A+25,STO_A,STO_Z=STO_A+25,
					LBL_A,LBL_Z=LBL_A+25,GTO_A,GTO_Z=GTO_A+25,
					NUMBER,KEYSTART,ERROR,DRPS,DUPS,PICKS,ROLLS,
					ROLLDS,GOTO,KEEP,LEVEL};

#define FALSE 0
#define TRUE 1

#define DrawS(n,m) DrawStack(n,os,vis,arrow,nums,m,mnu,&sub,&StackSub)
#define DUP() DupN(&vis,nums)
#define DROP(n) DropN(n,&vis,nums,&arrow,&os)
#define check1 if (!vis){ck=TFA();break;}
#define check2 if (vis<2){ck=TFA();break;}
#define checknew if (vis==STACKSIZE)\
								 {           \
									 ck=err("Stack Full!");  					 \
									 break;                          \
								 }                                 \
								 if (vis) DUP();                   \
								 else vis=1;

#define cn(x,y)\
clrscr(); \
printf(x,y); \
ngetchx();

char alpha,ctrl,shift,none,right;
char ins,deg;
#ifdef PC
int allow(int k)
{
	if (strchr("0123456789abcdefghijklmnopqrstuvwxyz.@ ",k)) return toupper(k);
	else return FALSE;
}
#else
int allow(int k)
{
	const char nums[]={55,56,57,52,53,54,83,84,49,50,51,88,89,48,0};
	char*i;
	i=strchr(nums,k);
	if (alpha)
	{
		if ((k>=65)&&(k<=76)) return k;
		else if (k==46) return 32;
		else if (i) return ((int)(i-nums)+77);
	}
	else if(none)
	{
		if ((k>=48)&&(k<=57)) return k;
		if (k==46) return 46;
		if (k==76) return '@';
		if (k==EXPO) return 'E';
	}
	return FALSE;
}
#endif

void DrawStat();
int TFA();
int err(const char*msg);
void TopDark();

int MatchF(int mnu,int sub,int *fuel)
{
	int retval;
	int MathCK[]={0,ALOG,0,EXP,
								0,0,0,0,
								INV,0,LN,LOG,
								0,0,0,XROOT};
	int TrigCK[]={ACOS,ASIN,ATAN,COS,
								0,SIN,0,TAN,
								0,0,0,0};
	int ConsCK[]={PI,0,0,0};
	int StakCK[]={ON,0,0,DEL,
								ENTER,0,0,0,
								0,0,0,RIGHT};

	int MathCo[]={ABS,0,CEIL,0,
								FLOOR,GCD,INTDIV,INT,
								0,LCM,0,0,
								MOD,ISPRIME,RAND,0,};
	int TrigCo[]={0,0,0,0,
								COSH,0,SINH,0,
								TANH,TODEG,TORAD,0};
	int ConsCo[]={0,E,MINR,MAXR};
	int StakCo[]={0,DEPTH,DROPN,0,
								0,DUPN,OVER,PICK,
								ROLL,ROLLD,ROT,0};
	int InstCo[]={DRPS,DUPS,GOTO,KEEP,
								LEVEL,PICKS,ROLLS,ROLLDS};
	if (mnu==2)
	{retval=MathCo[sub*4+*fuel];
	*fuel=MathCK[sub*4+*fuel];}
	else if (mnu==3)
	{retval=TrigCo[sub*4+*fuel];
	*fuel=TrigCK[sub*4+*fuel];}
	else if (mnu==4)
	{retval=ConsCo[sub*4+*fuel];
	*fuel=ConsCK[sub*4+*fuel];}
	else if (mnu==5)
	{retval=StakCo[sub*4+*fuel];
	*fuel=StakCK[sub*4+*fuel];}
	else if (mnu==0)
	{retval=InstCo[sub*4+*fuel];
	*fuel=0;}
	return retval;
}

void ShiftMem(unsigned videomem,unsigned start,int bits)
{
	int cl=0,cm=0;
	int i,j;
	for (i=4;i;i--)
	{
		j=start-i*64;
		cl=(int)((unsigned char)peekb(videomem,j+64));
		cm=(int)((unsigned char)peekb(videomem,j));
		#ifndef PC
			pokeb(videomem,j,(char)((cl<<8-bits)+cm));
			if (i>1) pokeb(videomem,j+64,(char)(cl>>bits));
			else pokeb(videomem,j+64,(char)(cl>>bits<<2));
		#endif
	}
}

void ColMen()
{
	unsigned int videomem,j,i,buf;
	/*unsigned int far videomem;*/
	videomem=*(unsigned int far*)0x4E0;
	for (i=0;i<4;i++)
		poke(videomem,2*i,0);
	for (j=1;j<4;j++)
		for (i=1;i<8;i++) ShiftMem(videomem,64*(j*4)+i,8-j*2);
	for (j=1;j<16;j++)
	{
		for (i=0;i<4;i++)
		{
			buf=peek(videomem,j*64+(i*2+1));
			if (!(j%4)) buf=buf^0x101;
			buf=~buf;
			#ifndef PC
				poke(videomem,j*64+(i*2),buf);
			#endif
		}
	}
	#ifndef PC
	for (i=0;i<4;i++)
		poke(videomem,i*2,~peek(videomem,i*2+1)^0x101);
	#endif
}
int DrawMenu(int mnu,int *sub)
{
	char **set=NULL;
	int i,j;
	char *cats[]={"math","trig","const","stack"};
	char *math[]={" abs ","alog ","ceil "," exp ",
								"floor"," gcd ","idiv "," int ",
								" inv "," lcm "," ln  "," log ",
								" mod ","prime","rand ","root "};
	char *trig[]={"acos ","asin ","atan "," cos ",
								"cosh "," sin ","sinh "," tan ",
								"tanh ","@deg ","@rad ","     "};
	char *cons[]={" pi  "," e   ","minr ","maxr "};
	char *stak[]={"clear","depth","dropn","drop ",
								" dup ","dupn ","over ","pick ",
								"roll ","rolld"," rot ","swap ",};
	char *Inst[]={"dropn","dupn ","goto ","keep ",
								"level","pick ","roll ","rolld"};

		gotoxy(1,8);
		printf("                    ");
		switch (mnu)
		{
			case 1:
				for (i=0;i<4;i++)
				{
					gotoxy(i*5+1,8);
					printf(cats[i]);
				}
				break;
			case 0:
				if (*sub==2) *sub=0;
				if (*sub==-1) *sub=1;
				set=Inst;
			case 2:
				if (!set)
				{
					if (*sub==4) *sub=0;
					if (*sub==-1) *sub=3;
					set=math;
				}
			case 3:
				if (!set)
				{
					if (*sub==3) *sub=0;
					if (*sub==-1) *sub=2;
					set=trig;
				}
			case 4:
				if (!set){set=cons;*sub=0;}
			case 5:
			if (!set)
				{
					if (*sub==3) *sub=0;
					if (*sub==-1) *sub=2;
					set=stak;
				}
				for (i=*sub*4;i<(*sub*4+4);i++)
				{
					gotoxy(i%4*5+1,8);
					printf("%s",set[i]);
					for (j=0;j<strlen(set[i]);j++)
					{
						if (set[i][j]=='@')
						{
							gotoxy(i%4*5+1+j,8);
							printf(INTO);
						}
					}
				}
		}
	ColMen();
}

int ngetchx()
{
	int k;
	while(!kbhit()){}
	k=getch();
	if (k) return k+shift*256;
	else
	{
		k=getch();
		if (k==1) return ESC;
		else return k+128+shift*256;
	}
}

int opp(int k)
{
	/*int StatOpps[]={SHIFT,ALPHA,CTRL};*/
	int NoneOpps[]={MULT,DIV,PLUS,SUB,POW,LOG,LN,SIN,COS,TAN,SQUARE};
	int ShiftOpps[]={XROOT,ALOG,EXP,ASIN,ACOS,ATAN,SQR,CBR,INV};
	int i;
	if (none)
	{
		for (i=0;i<sizeof(NoneOpps)/2;i++)
			if (k==NoneOpps[i]) return NoneOpps[i];
	}
	else if(shift)
	{
		for (i=0;i<sizeof(ShiftOpps)/2;i++)
			if (k==ShiftOpps[i]) return ShiftOpps[i];
	}
	return FALSE;
}

unsigned char KeyCorr(int key)
{
	int i=0;
	int Coded[]={LOG,LN,SIN,COS,TAN,XROOT,ALOG,EXP,ASIN,ACOS,ATAN,INV,PI,
					 ON,RIGHT,DEL,ENTER,MULT,SUB,PLUS,DIV,POW,SQUARE,SQR,CBR,0};
		do
		{
			if (Coded[i]==key)
			{
				return i+KEYSTART;
			}
			i++;
		}while (Coded[i]);
	return 0;
}

int RevKeyCorr(int key)
{
	int Coded[]={LOG,LN,SIN,COS,TAN,XROOT,ALOG,EXP,ASIN,ACOS,ATAN,INV,PI,
					 ON,RIGHT,DEL,ENTER,MULT,SUB,PLUS,DIV,POW,SQUARE,SQR,CBR,0};
	return Coded[key-KEYSTART];
}

const char *TextCorr(int coded)
{
	/*XYEQ,XYNE,XYGT,XYLT,XYGE,XYLE,
	X0EQ,X0NE,X0GT,X0LT,X0GE,X0LE,*/
	char *buff="       ";
	char *Otro[]={"ABS","INT","RAND","@DEG","@RAD","GCD","IDIV",
								 "DUP","DUPN","ISPRIME","LCM","MOD","CEIL","FLOOR",
								 "DROPN","DEPTH","PICK","OVER","SINH","COSH","TANH",
								 "MAXR","MINR","ROLL","ROLLD","ROT","e",
								 "x=y?","xÈy?","x>y?","x<y?","xÊy?","xÉy?",
								 "x=0?","xÈ0?","x>0?","x<0?","xÊ0?","xÉ0?",
								 "@i","GOTO i","*i","@*i","STOP"};

	char *Keys[]={"LOG","LN","SIN","COS","TAN","XROOT","ALOG","EXP",
									"ASIN","ACOS","ATAN","INV","PI","CLEAR","SWAP",
									"DROP","DUP","À","-","+","Á","^","SQUARE","SQR",
									"CBR"};
	memset(buff,0,7);
	if ((coded>=ABS)&&(coded<=STOP)) return Otro[coded-1];
	else if (coded>=KEYSTART) return Keys[coded-KEYSTART];
	else if ((coded>=VAR_A)&&(coded<=VAR_Z))
		{buff[0]=coded-VAR_A+65;}
	else if ((coded>=STO_A)&&(coded<=STO_Z))
		{buff[0]='@';buff[1]=coded-STO_A+65;}
	else if ((coded>=LBL_A)&&(coded<=LBL_Z))
		{strcpy(buff,"LBL ");buff[4]=coded-LBL_A+65;}
	else if ((coded>=GTO_A)&&(coded<=GTO_Z))
		{strcpy(buff,"GOTO ");buff[5]=coded-GTO_A+65;}
	return buff;
}
int Input(double *b,int *control,int *ck,unsigned char *SO,int key,int *numb,int *mnu,int *sub,char *in)
{
	int k=0,fp=1,cp=2,offset=0,js,aoc=0,blink=0,bye=FALSE,retsign=FALSE;
	int i,j=0,l,lp;
	/*char inp[21]={0,0},redrum;*/
	char inp[256]={0,0},redrum,dinp[22],i2[256];
	char *C1Name[]={"LOG","LN","SIN","COS","TAN","XROOT","ALOG","EXP","ASIN",
									 "ACOS","ATAN","INV","PI","CLEAR","SWAP","DROP","DUP","SQR","CBR"};
	int C1Num[]={LOG,LN,SIN,COS,TAN,XROOT,ALOG,EXP,ASIN,ACOS,ATAN,INV,PI,ON,
								 RIGHT,DEL,ENTER,SQR,CBR};
	char *C2Name[]={"ABS","INT","RAND","@DEG","@RAD","GCD","IDIV","LCM","MOD",
								 "CEIL","FLOOR","DROPN","DEPTH","PICK","OVER","DUPN","ROT",
								 "SINH","COSH","TANH","MAXR","MINR","ROLL","ROLLD"};
	unsigned char C2Num[]={ABS,INT,RAND,TODEG,TORAD,GCD,INTDIV,LCM,MOD,CEIL,
												FLOOR,DROPN,DEPTH,PICK,OVER,DUPN,ROT,SINH,COSH,TANH,
												MAXR,MINR,ROLL,ROLLD};
	inp[0]=key;
	if (alpha==1) {none=TRUE;alpha=0;}
	if (shift) {none=TRUE;shift=FALSE;}
	if (ctrl) {none=TRUE;ctrl=FALSE;}
	if (!strlen(in)) DrawStat();
	do{
		bye=TRUE;
		do{
			k=ENTER;
			if (!strlen(in))
			{
				gotoxy(1,1);
				memcpy(dinp,inp+offset,21);
				dinp[21]=0;
				gotoxy(1,7);
				printf("                     ");
				gotoxy(1,7);
				printf("%s",dinp);
				if (offset)
				{
					gotoxy(1,7);
					printf(LARR);
				}
				if (offset+21<fp)
				{
					gotoxy(21,7);
					printf(RARR);
				}
				j=0;
				do
				{
					if (dinp[j]=='@')
					{
						gotoxy(j+1,7);
						printf(INTO);
					}
					j++;
				} while (dinp[j]!=0);
				gotoxy(cp-offset,7);
				printf(ARROW);
				while (!kbhit())
				{
					blink++;
					if (blink==BTIME)
					{
						blink=0;
						aoc=!aoc;
						gotoxy(cp-offset,7);
						if (aoc)
						{
							if ((offset)&&(cp-offset==1)) printf(LARR);
							else if ((offset+21<fp)&&(cp-offset==21)) printf(RARR);
							else printf("%c",inp[cp-1]);
						}
						else printf(ARROW);
					}
				}
				k=ngetchx();
				redrum=FALSE;
				aoc=0;blink=0;
				if ((k==ESC)||(k==ENTER)||(k==ON));
				/*else if (k==ESC)
				{
					if (*mnu!=1)
					{
						*mnu=1;
						*sub=0;
						k=0;
					}
				}*/
				else if ((k==POW)&&(alpha)) {deg++;if (deg==3) deg=0;DrawStat();}
				else if (opp(k)&&(!alpha));
				else if (k==LEFT)
				{
					if (cp>1) cp--;
					if (cp==offset) offset--;
				}
				else if (k==RIGHT)
				{
					if (cp<(fp+1)) cp++;
					if (cp-offset>21) offset++;
				}
				else if (k==INS)
				{
					ins=!ins;
					DrawStat();
				}
				else if (k==DEL)
				{
					if (fp)
					{
						if (cp>fp)
						{
							fp--;
							cp--;
							inp[fp]=0;
							if ((offset)&&(cp-offset==1)) offset--;
						}
						else
						{
							memmove(inp+cp-1,inp+cp,fp-(cp-1));
							fp--;
						}
					}
				}
				else if (k==ALPHA)
				{
					if(!alpha) {none=FALSE;alpha=1;}
					else {none=TRUE;alpha=0;}
					shift=FALSE;ctrl=FALSE;
					DrawStat();
				}
				else if (k==ALPHA+256)
				{
					alpha=2;none=FALSE;
					shift=FALSE;ctrl=FALSE;
					DrawStat();
				}

				else if (k==SHIFT)
				{
					none=shift;
					shift=!shift;
					alpha=FALSE;ctrl=FALSE;
					DrawStat();
				}
				else if (k==CTRL)
				{
					none=ctrl;
					ctrl=!ctrl;
					alpha=FALSE;shift=FALSE;
					DrawStat();
				}
				else if ((k>=F1)&&(k<=F4))
				{
					if (*mnu==1)
					{
						*mnu=k-F1+2;
						*sub=0;
						k=0;
						DrawMenu(*mnu,sub);
						redrum=FALSE;
					}
				}
				else if (k==F5)
				{
					(*sub)++;
					DrawMenu(*mnu,sub);
					redrum=FALSE;
				}
				else if (k==F6)
				{
					*mnu=1;
					DrawMenu(*mnu,sub);
					redrum=FALSE;
				}
				else if (k==SIGN)
				{
					js=1;

					l=cp-1;
					i2[0]=0;
					while ((inp[l]!=' ')&&(l!=strlen(inp))) l++;
					strrev(inp);
					l=strlen(inp)-l;
					if (strchr(inp+l,' '))
					{
						for (i=l;;i++)
						{
							i2[i-l]=inp[i];
							if (inp[i]==' ') break;
						}
						i2[i-l]=0;
						j=strlen(inp)-i;
						strrev(i2);
						strrev(inp);
					}
					else
					{
						j=0;
						l=strlen(inp)-l;
						strrev(inp);
						memcpy(i2,inp,l);
						i2[l]=0;
					}

					if (!i2[0])
					{
						js=0;
						k=ENTER;
						retsign=TRUE;
					}

					if (strchr(i2,'E'))
					if (strchr(strchr(i2,'E')+1,'E')) js=0;
					if ((cp>1)&&(i2[cp-2-j]=='E')&&(js))
					{
						if ((ins)||(cp==fp+1))
						{
							memmove(inp+cp,inp+cp-1,fp-(cp-1));
							inp[cp-1]='-';
							inp[fp+1]=0;
							cp++;
							fp++;
						}
						else
						{
							inp[cp-1]='-';
							cp++;
						}
					}
					else if (!retsign)
					{
						if (inp[j]=='-')
						{
							fp--;
							/*memmove(inp,inp+1,(fp+1));*/
							memmove(inp+j,inp+j+1,(fp+1));
							cp--;
						}
						else
						{
							/*memmove(inp+1,inp,(fp));*/
							memmove(inp+j+1,inp+j,fp);
							fp++;
							cp++;
							inp[j]='-';
						}
					}
				}
				/*else if (fp<19)*/
				if (fp<255)
				{
					/*if ((k==46)&&(strchr(inp,46)));*/
					/*else*/
					if (allow(k))
					{
						if ((ins)||(cp==fp+1))
						{
							memmove(inp+cp,inp+cp-1,fp-(cp-1));
							inp[cp-1]=allow(k);
							inp[fp+1]=0;
							cp++;
							fp++;
							if (!((offset)&&(cp-offset<21))&&(cp>21)) offset++;
						}
						else
						{
							inp[cp-1]=allow(k);
							cp++;
						}
					}
				}
				j=opp(k);
				if ((k==POW)&&(alpha)) j=0;
				if ((k!=ALPHA)&&(k!=CTRL)&&(k!=SHIFT))
				{
					if (alpha==1) {alpha=0;none=TRUE;redrum=TRUE;}
					if (ctrl) {ctrl=FALSE;none=TRUE;redrum=TRUE;}
					if (shift) {shift=FALSE;none=TRUE;redrum=TRUE;}
				}
				if (redrum) DrawStat();
			}/*if*/
		} while((k!=ESC)&&(k!=ENTER)&&(!j)&&(k!=ON)&&!((k>=F1)&&(k<=F4)));
			*numb=0;
			*control=0;
			if (k==ESC) return FALSE;
			if (k==ON) return FALSE;
			/*{
				*numb=1;
				(SO[0])=0;
				(ck[0])=ON;
				(b[0])=0;
				return FALSE;
			}*/
			*control=j;
			l=0;
			i2[0]=0;
			lp=0;
			if (strlen(in)) strcpy(inp,in);
			for (i=0;i<strlen(inp);i++)
			{
				i2[i-lp]=inp[i];
				i2[i-lp+1]=0;
				if (inp[i]==' ') i2[i-lp]=0;
				if((inp[i]==' ')||(!inp[i+1]))
				{
							if (!i2[0])
							{
								lp=i+1;
								continue;
							}
							(*numb)++;
							(SO[l])=0;
							(ck[l])=0;
							(b[l])=0;
						/*if (inp[0]==0) return FALSE;*/

					for (j=0;j<20;j++)
						if (!strcmp(i2,C1Name[j])) ck[l]=C1Num[j];
					if (!strcmp(i2,"SQUARE")) ck[l]=SQUARE;

					/*if (!strcmp(i2,"LOG")) {(ck[l])=LOG;}
					else if (!strcmp(i2,"LN")) {(ck[l])=LN;}
					else if (!strcmp(i2,"SIN")) {(ck[l])=SIN;}
					else if (!strcmp(i2,"COS")) {(ck[l])=COS;}
					else if (!strcmp(i2,"TAN")) {(ck[l])=TAN;}
					else if (!strcmp(i2,"XROOT")) {(ck[l])=XROOT;}
					else if (!strcmp(i2,"ALOG")) {(ck[l])=ALOG;}
					else if (!strcmp(i2,"EXP")) {(ck[l])=EXP;}
					else if (!strcmp(i2,"ASIN")) {(ck[l])=ASIN;}
					else if (!strcmp(i2,"ACOS")) {(ck[l])=ACOS;}
					else if (!strcmp(i2,"ATAN")) {(ck[l])=ATAN;}
					else if (!strcmp(i2,"INV")) {(ck[l])=INV;}
					else if (!strcmp(i2,"PI")) {(ck[l])=PI;}
					else if (!strcmp(i2,"CLEAR")) {(ck[l])=ON;}
					else if (!strcmp(i2,"SWAP")) {(ck[l])=RIGHT;}
					else if (!strcmp(i2,"DROP")) {(ck[l])=DEL;}
					else if (!strcmp(i2,"DUP")) {(ck[l])=ENTER;}
					else if (!strcmp(i2,"SQUARE")) {(ck[l])=SQUARE;}
					else if (!strcmp(i2,"SQR")) {(ck[l])=SQR;}
					else if (!strcmp(i2,"CBR")) {(ck[l])=CBR;}*/

					for (j=0;j<24;j++)
						if (!strcmp(i2,C2Name[j])) SO[l]=C2Num[j];

					if (SO[l]||ck[l]);
					else if (!strcmp(i2,"ISPRIME")) SO[l]=ISPRIME;

					/*if (!strcmp(i2,"ABS")) {(SO[l])=ABS;}
					else if (!strcmp(i2,"INT")) {(SO[l])=INT;}
					else if (!strcmp(i2,"RAND")) {(SO[l])=RAND;}
					else if (!strcmp(i2,"@DEG")) {(SO[l])=TODEG;}
					else if (!strcmp(i2,"@RAD")) {(SO[l])=TORAD;}
					else if (!strcmp(i2,"GCD")) {(SO[l])=GCD;}
					else if (!strcmp(i2,"IDIV")) {(SO[l])=INTDIV;}
					/*else if (!strcmp(i2,"ISPRIME")) {(SO[l])=ISPRIME;}*/
					else if (!strcmp(i2,"LCM")) {(SO[l])=LCM;}
					else if (!strcmp(i2,"MOD")) {(SO[l])=MOD;}
					else if (!strcmp(i2,"CEIL")) {(SO[l])=CEIL;}
					else if (!strcmp(i2,"FLOOR")) {(SO[l])=FLOOR;}
					else if (!strcmp(i2,"DROPN")) {(SO[l])=DROPN;}
					else if (!strcmp(i2,"DEPTH")) {(SO[l])=DEPTH;}
					else if (!strcmp(i2,"PICK")) {(SO[l])=PICK;}
					else if (!strcmp(i2,"OVER")) {(SO[l])=OVER;}
					else if (!strcmp(i2,"DUPN")) {(SO[l])=DUPN;}
					else if (!strcmp(i2,"ROT")) {(SO[l])=ROT;}
					else if (!strcmp(i2,"SINH")) {(SO[l])=SINH;}
					else if (!strcmp(i2,"COSH")) {(SO[l])=COSH;}
					else if (!strcmp(i2,"TANH")) {(SO[l])=TANH;}
					/*else if (!strcmp(i2,"E")) {(SO[l])=E;}*/
					else if (!strcmp(i2,"MAXR")) {(SO[l])=MAXR;}
					else if (!strcmp(i2,"MINR")) {(SO[l])=MINR;}
					else if (!strcmp(i2,"ROLL")) {(SO[l])=ROLL;}
					else if (!strcmp(i2,"ROLLD")) {(SO[l])=ROLLD;}*/

					if ((!i2[1])&&(i2[0]>='A')&&(i2[0]<='Z'))
						SO[l]=VAR_A+i2[0]-65;
					else if ((i2[0]=='@')&&(i2[2]==0)&&(i2[1]>='A')&&(i2[1]<='Z'))
						SO[l]=STO_A+i2[1]-65;
					if ((!SO[l])&&(!ck[l]))
						{
						(b[l])=atof(i2);
						if (!(b[l]))
						for (j=0;j<strlen(i2);j++)
						{
							if (i2[j]=='-');
							else if (i2[j]=='0');
							else if (i2[j]=='.');
							else if (i2[j]=='E');
							else if (!inp[j]) j=strlen(i2);
							else
							{
								if (!strlen(in))
								{
									gotoxy(1,1);
									printf("                     ");
									gotoxy(1,1);
									printf("Invalid:'%s'",i2);
									for (i=0;i<strlen(i2);i++)
									if (i2[i]=='@')
									{
										gotoxy(10+i,1);
										printf(INTO);
									}
									TopDark();
									ngetchx();
									DrawStat();
								}
								bye=FALSE;
								break;
							}
						}
					}
				lp=i+1;
				l++;
			}/*endif*/
		}/*end for*/
		/*(*numb)=l;*/
		if (retsign)
		{
			(b[*numb])=0;
			(ck[*numb])=SIGN;
			(SO[*numb])=0;
			(*numb)++;
		}
		if ((k>=F1)&&(k<=F4))
		{
			(b[*numb])=0;
			(ck[*numb])=k-F1;
			(SO[*numb])=MatchF(*mnu,*sub,&ck[*numb]);
			(*numb)++;
		}
		if ((!bye)&&(strlen(in)))
		{
			(SO[0])=ERROR;
			bye=TRUE;
		}
	} while(!bye);
	 return TRUE;
}

void DrawStat()
{
	gotoxy(1,1);
	printf("                     ");
	gotoxy(1,1);
	if (alpha==1) printf("Alpha");
	else if (alpha==2) printf("A-Lock");
	else if (ctrl) printf("Ctrl");
	else if (shift) printf("Shift");
	gotoxy(8,1);
	if (ins) printf("Insert");
	gotoxy(15,1);
	if (!deg)  printf("Radians");
	else if (deg==1) printf("Degrees");
	else printf("Grads");
}
void DrawStack(int ctp,int os,int vis,int arrow,double *nums,int place,int mnu,int*sub,int*StackSub)
{
	int i;/*char form[8];*/
	DrawStat();
	for (i=1;i<=ctp;i++)
	{
		gotoxy(1,place-i);
		printf("%d:",i+os);
		if (i+os<=vis)
		{
			if (i+os<10)
			{
				gotoxy(3,place-i);
				if (right) printf("%19.12g",nums[i+os-1]);
				else printf("%.12g",nums[i+os-1]);
			}
			else
			{
				gotoxy(4,place-i);
				if (right) printf("%18.11g",nums[i+os-1]);
				else printf("%.11g",nums[i+os-1]);
			}
		}
		if (i==arrow)
		{
			if (i+os<10) gotoxy(2,place-i);
			else gotoxy(3,place-i);
			#ifdef PC
				printf("%c",16);
			#else
				printf("%c%c",0xF6,189);
			#endif
		}
	}

	if (arrow) DrawMenu(0,StackSub);
	else DrawMenu(mnu,sub);
}
void TopDark()
{
	#ifndef PC
	unsigned i,j,videomem;
	int buf;
	videomem=peek(0x4E,0);
	for (i=0;i<16;i++)
	for (j=0;j<4;j++)
	{
		buf=~peek(videomem,56+i*64+j*2);
		poke(videomem,56+i*64+j*2,buf);
	}
	#endif
}
int TFA()
{
	unsigned i,j,videomem;
	gotoxy(1,1);
	printf("                     ");
	gotoxy(1,1);
	printf("Too few arguments!");
	TopDark();
	return ngetchx();
}
int DupN(char *vis,double *nums)
{
	if (!(*vis)) return TFA();
	else if (*vis==STACKSIZE) return err("Stack is full!");
	else
	{
		memmove(nums+1,nums,*vis*8);
		(*vis)++;
	}
	return 0;
}
int DropN(int el,char *vis,double *nums,int *arrow,int *os)
{
	if (!(*vis)) return TFA();
	else
	{
		memmove(nums+(el-1),nums+el,(((*vis)-el)*8));
		(*vis)--;
		if (*arrow)
		{
			if (*arrow+*os>*vis) *arrow=*vis-*os;
			if (!(*arrow)&&(*os)) {(*os)--;*arrow=1;}
		}
	}
	return 0;
}
int err(const char *msg)
{
	gotoxy(1,1);
	printf("                     ");
	gotoxy(1,1);
	printf(msg);
	TopDark();
	return ngetchx();
}
int wa(int pao,unsigned char*prgm)
{
	int i=0,j=0;
	for (i=0;i<pao;i++)
	{
		if (prgm[j]==NUMBER) j+=8;
		j++;
	}
	return j;
}
int arlen (unsigned char*prgm)
{
	int i=0,j=0;
	while(prgm[j])
	{
		if (prgm[j]==NUMBER) j+=8;
		j++;i++;
	}
	return i;
}

int matherr(struct exception *a)
{
	errno=a->type;
	return 1;
}

void PE(int ctrl,int alpha,int shift)
{
	gotoxy(1,1);
	if (ctrl) printf("Ctrl  ");
	else if (shift) printf("Shift ");
	else if (alpha==1) printf("Alpha ");
	else if (alpha==2) printf("A-Lock");
	else printf("%c     ",32);
}

int PrgmEd(unsigned char *prgm,int *cks,double *bns,unsigned char *SO,
					 int *Ppro,int *Ppra,char *in,int *Pkey,
					 int *Pcontrol,int *Pnumb,int *Psub,int *Pmnu)

{
	int i,j,DelIn=FALSE;
	int pro=*Ppro,pra=*Ppra,key=*Pkey,control=*Pcontrol,
			numb=*Pnumb,sub=*Psub,mnu=*Pmnu;
	double buff;
	#ifndef PC
	j=peek(0x4E,0);
	for (i=0;i<4;i++)
	poke((unsigned)j,(unsigned)i*2,0);
	#endif
	do{
		cks[0]=0;
		SO[0]=0;
		bns[0]=0;
		clrscr();
		PE(ctrl,alpha,shift);
		gotoxy(18,1);
		printf("%3d",pro+pra);
		for (i=2;i<9;i++)
		{
			gotoxy(1,i);
			if (!prgm[wa(pro+i-2,prgm)]) printf(":");
			else if (prgm[wa(pro+i-2,prgm)]==NUMBER)
			{
				memcpy(&buff,prgm+wa(pro+i-2,prgm)+1,8);
				printf(":%.12g",buff);
			}
			else
			{
				if (!strcmp(TextCorr(prgm[wa(pro+i-2,prgm)]),"SQUARE"))
					printf(SQUARESYM);
				else if (!strcmp(TextCorr(prgm[wa(pro+i-2,prgm)]),"SQR"))
					printf(SQRSYM);
				else if (!strcmp(TextCorr(prgm[wa(pro+i-2,prgm)]),"CBR"))
					printf(CBRSYM);
				else
				{
					sprintf(in,"%s",TextCorr(prgm[wa(pro+i-2,prgm)]));
					printf(":%s",in);
					j=0;
					do
					{
						if (in[j]=='@')
						{
							gotoxy(j+2,i);
							printf(INTO);
						}
					j++;
					}while(in[j]);
					in[0]=0;
				}
			}
		}
		if (pro)
		{
			gotoxy(1,2);
			printf(UARR);
		}
		if ((prgm[wa(7,prgm)])&&(prgm[wa(pro+7,prgm)]))
		{
			gotoxy(1,8);
			printf(DARR);
		}
		gotoxy(1,pra+1);
		#ifdef PC
		printf("%c",16);
		#else
		printf("%c%c",0xF6,189);
		#endif
		key=0;
		if (prgm[wa(pro+pra-1,prgm)]==NUMBER)
		{
			memcpy(&buff,prgm+wa(pro+pra-1,prgm)+1,8);
			sprintf(in,"%.12g",buff);
		}
		else strcpy(in,TextCorr(prgm[wa(pro+pra-1,prgm)]));
		/*if ((prgm[wa(pro+pra-1,prgm)]<XYEQ)||
			(prgm[wa(pro+pra-1,prgm)]>X0LE))
			{
				i=0;
				do
				{
					if (
				}while(in[i]);
			}*/
		if (DelIn) {in[0]=0;DelIn=FALSE;}
		i=FALSE;
		while((!i)&&(key!=DOWN)&&(key!=UP)&&(key!=ENTER)&&(key!=ESC)&&!(opp(key))&&(key!=UP+256)&&(key!=DOWN+256)&&(key!=PI))
		{
			if ((key!=ALPHA)&&(key!=CTRL)&&(key!=SHIFT)&&(alpha!=2))
			{
				none=TRUE;ctrl=FALSE;
				alpha=FALSE;shift=FALSE;
			}
			PE(ctrl,alpha,shift);
			i=FALSE;
			gotoxy(2,pra+1);
			printf("%c                  ",32);
			/*ngetchx();*/
			gotoxy(2,pra+1);
			printf("%s_",in);
			j=0;
			do
			{
				if (in[j]=='@')
				{
					gotoxy(j+2,pra+1);
					printf(INTO);
				}
			j++;
			}while(in[j]);
			while(!kbhit());
			key=ngetchx();
			if (((allow(key))||(key==indirect)||(opp(key)==MULT))&&(strlen(in)<18)&&(key!=EXPO))
			{
				if ((allow(key)==' ')&&(strcmp("LBL",in))&&(strcmp("GOTO",in)));
				else
				{
					in[strlen(in)+1]=0;
					if (key==indirect) in[strlen(in)]='i';
					else if (opp(key)==MULT)
					{
						in[strlen(in)]='*';
						key=0;
					}
					else in[strlen(in)]=allow(key);
				}
			}
			else if (key==ON)
			{
				memset(prgm,0,PRGMSIZE);
				in[0]=0;
				pra=1;
				pro=0;
				i=TRUE;
			}
			else if (key==OPTN)
			{
				none=TRUE;alpha=FALSE;
				ctrl=FALSE;shift=FALSE;
				clrscr();
				printf("Select a branch\ninstruction");
				for (i=XYEQ;i<=X0LE;i++)
				{
					if (i-XYEQ<6) gotoxy(1,i-XYEQ+3);
					else gotoxy(11,i-XYEQ-3);
					printf("%s",TextCorr(i));
				}
				i=0;
				gotoxy(6,3);
				printf(ARROW);
				do
				{
					key=ngetchx();
					if ((key==UP)||(key==DOWN)||(key==LEFT)||(key==RIGHT))
					{
						if (i<6) gotoxy(6,i+3);
						else gotoxy(16,i-3);
						printf(" ");
						if ((key==UP)&&(i)) i--;
						else if ((key==DOWN)&&(i<11)) i++;
						else if ((key==LEFT)&&(i>5)) i-=6;
						else if ((key==RIGHT)&&(i<6)) i+=6;
						if (i<6) gotoxy(6,i+3);
						else gotoxy(16,i-3);
						printf(ARROW);
					}
				}while((key!=ENTER)&&(key!=ESC));
				if (key==ENTER)
				{
					in[0]='!';
					in[1]=i;
					in[2]=0;
				}
				else
				{
					key=OPTN;
					i=TRUE;
				}
			}
			else if (key==ENTER)
			{
				if (!in[0]) key=0;
			}
			else if (key==ALPHA)
			{
				gotoxy(1,1);
				if (!alpha) {none=FALSE;alpha=1;printf("Alpha ");}
				else {none=TRUE;alpha=0;printf("%c     ",32);}
				shift=FALSE;ctrl=FALSE;
			}
			else if (key==ALPHA+256)
			{
				gotoxy(1,1);
				alpha=2;none=FALSE;
				shift=FALSE;ctrl=FALSE;
				gotoxy(1,1);
				printf("A-Lock");
			}
			else if ((key==SHIFT)||(key==SHIFT+256))
			{
				gotoxy(1,1);
				none=shift;
				shift=!shift;
				if (shift) printf("Shift ");
				else printf("%c     ",32);
				alpha=FALSE;ctrl=FALSE;
			}
			else if (key==CTRL)
			{
				gotoxy(1,1);
				none=ctrl;
				ctrl=!ctrl;
				if (ctrl) printf("Ctrl  ");
				else printf("%c      ",32);
				alpha=FALSE;shift=FALSE;
			}
			else if (key==DEL)
			{
				if (strlen(in)) in[strlen(in)-1]=0;
				else
				{
					if (prgm[wa(pro+pra-1,prgm)]==NUMBER)
					{
						j=wa(pro+pra-1,prgm);
						memmove(prgm+j,prgm+j+9,PRGMSIZE-9-j);
						memset(prgm+PRGMSIZE-9,0,8);
					}
					else
					{
						j=wa(pro+pra-1,prgm);
						memmove(prgm+j,prgm+j+1,PRGMSIZE-1-j);
						prgm[PRGMSIZE-1]=0;
					}
					if (wa(pro+pra-1,prgm))
					{
						if (pra==1) pro--;
						else pra--;
					}
					i=TRUE;
				}
			}
			else if(key==SIGN)
			{
				if (in[0]=='-')
				{
					memmove(in,in+1,strlen(in)-1);
					in[strlen(in)-1]=0;
				}
				else
				{
					memmove(in+1,in,strlen(in));
					in[0]='-';
				}
			}
		}
		if (((opp(key))&&(opp(key)!=MULT))||(key==PI))
		{
			j=wa(pro+pra-1,prgm);
			if (prgm[j]==NUMBER)
			{
				memmove(prgm+j+1,prgm+j+9,PRGMSIZE-8-j);
				memset(prgm+PRGMSIZE-8,0,9);
			}
			if (key==PI) prgm[j]=KeyCorr(PI);
			else prgm[j]=KeyCorr(opp(key));
			if (pra==7) pro++;
			else pra++;
			in[0]=0;
		}
		else if (key==UP+256)
		{
			pro=0;
			pra=1;
		}
		else if (key==UP)
		{
			/*if (prgm[wa(pro+pra-1,prgm)])*/
			/*{*/
				if ((prgm[wa(pro+pra-1,prgm)])||(!prgm[wa(pro+pra,prgm)]))
				{
					if (pra==1)
						{if(pro) pro--;}
					else pra--;
				}
			/*}*/
		}
		else if (key==DOWN+256)
		{
			if (arlen(prgm)<7) pra=arlen(prgm);
			else
			{
				pra=7;
				pro=arlen(prgm)-7;
			}
		}
		else if(key==DOWN)
		{
			if (prgm[wa(pro+pra-1,prgm)])/*in[0])*/
			{
				if (prgm[wa(pro+pra,prgm)])
				{
					if (pra==7) pro++;
					else pra++;
				}
			}
		}
		else if (key==ENTER)
		{
			for (j=0;j<12;j++)
			{
				if (!strcmp(in,TextCorr(j+XYEQ)))
				{
					in[0]='!';
					in[1]=j;
					break;
				}
			}
			if ((strlen(in)==5)&&(!strncmp(in,"LBL ",4))&&
				(in[4]>='A')&&(in[4]<='Z'))SO[0]=LBL_A+in[4]-65;
			else if ((strlen(in)==6)&&(!strncmp(in,"GOTO ",5))&&
				(in[5]>='A')&&(in[5]<='Z'))SO[0]=GTO_A+in[5]-65;
			else if (in[0]=='!') SO[0]=XYEQ+in[1];
			else if (!strcmp(in,"STOP")) SO[0]=STOP;
			else if (!strcmp(in,"GOTO i")) SO[0]=GTO_i;
			else if (!strcmp(in,"@i")) SO[0]=STO_i;
			else if (!strcmp(in,"*i")) SO[0]=VAR_i;
			else if (!strcmp(in,"@*i")) SO[0]=IND_i;
			else if ((!in[1])&&(strchr("ÀÁ+-*/^",in[0])))
			{
				switch(in[0])
				{
					case '+':
						cks[0]=PLUS;break;
					case '-':
						cks[0]=SUB;break;
					case '*':
					case 'À':
						cks[0]=MULT;break;
					case '/':
					case 'Á':
						cks[0]=DIV;break;
					case '^':
						cks[0]=POW;break;
				}
			}
			else Input(&bns[0],&control,&cks[0],&SO[0],0,&numb,&mnu,&sub,in);
			/*clrscr();
			printf("%d %d %g",SO[0],cks[0],bns[0]);
			ngetchx();*/
			if (SO[0]==ERROR)
			{
				gotoxy(1,1);
				printf("                     ");
				gotoxy(1,1);
				printf("Invalid:'%s'",in);
				for (j=0;j<strlen(in);j++)
				{
					if (in[j]=='@')
					{
						gotoxy(10+j,1);
						printf(INTO);
					}
				}
				TopDark();
				ngetchx();
			}
			else
			{
				if (wa(pro+pra-1,prgm)>=PRGMSIZE)
				{
					gotoxy(1,1);
					printf("Program memory full. ");
					TopDark();
					ngetchx();
				}
				else
				{
					i=TRUE;
					if (SO[0])
					{
						j=wa(pro+pra-1,prgm);
						/*if (prgm[wa(pro+pra-1,prgm)]==NUMBER)
						{
							memmove(prgm+j+1,prgm+j+9,PRGMSIZE-j-9);
							memset(prgm+PRGMSIZE-8,0,8);
						}*/
						/*cn("%d",wa(pra+pro,prgm));*/
						/*if (prgm[wa(pra+pro,prgm)])
						{*/
							if (prgm[j]==NUMBER)
							{
								prgm[j]=0;
								memmove(prgm+j+1,prgm+j+9,PRGMSIZE-8-j);
								memset(prgm+PRGMSIZE-8,0,9);
								memmove(prgm+j+1,prgm+j,PRGMSIZE-1-j);
							}
							else
							{
								prgm[j]=0;
								memmove(prgm+j+1,prgm+j,PRGMSIZE-1-j);
							}
						/*}*/
						prgm[j]=SO[0];
					}
					else if (cks[0])
					{
						j=wa(pro+pra-1,prgm);
						/*if (prgm[wa(pro+pra-1,prgm)]==NUMBER)
						{
							memmove(prgm+j+1,prgm+j+9,PRGMSIZE-j-9);
							memset(prgm+PRGMSIZE-8,0,8);
						}*/
						/*if (prgm[wa(pra+pro,prgm)])
						{ */
							if (prgm[j]==NUMBER)
							{
								prgm[j]=0;
								memmove(prgm+j+1,prgm+j+9,PRGMSIZE-8-j);
								memset(prgm+PRGMSIZE-8,0,9);
								memmove(prgm+j+1,prgm+j,PRGMSIZE-1-j);
							}
							else
							{
								prgm[j]=0;
								memmove(prgm+j+1,prgm+j,PRGMSIZE-1-j);
							}
						/*}*/
						prgm[j]=KeyCorr(cks[0]);
					}
					else /*if (bns[0])*/
					{
						if (wa(pro+pra-1,prgm)+8>=PRGMSIZE)
						{
							gotoxy(1,1);
							printf("Program memory full. ");
							TopDark();
							ngetchx();
							i=FALSE;
						}
						else
						{
							j=wa(pro+pra-1,prgm);
							if (prgm[wa(pro+pra,prgm)])
							{
								/*prgm[j]=0;*/
								memmove(prgm+j+9,prgm+j,PRGMSIZE-9-j);
								prgm[j]=NUMBER;
								memcpy(prgm+j+1,&bns[0],8);
								DelIn=TRUE;
							}
							else
							{
								prgm[j]=NUMBER;
								memcpy(prgm+j+1,&bns[0],8);
							}
							/*prgm[wa(pro+pra-1,prgm)]=NUMBER;*/
						}
					}
					if (i)
					{
						if (pra==7) pro++;
						else pra++;
						in[0]=0;
					}
				}
			}
		}
	}while(key!=ESC);
	in[0]=0;
	numb=0;
	key=VARS;
	(*Ppro)=pro;
	(*Ppra)=pra;
	(*Pkey)=key;
	(*Pcontrol)=control;
	(*Pnumb)=numb;
	(*Psub)=sub;
	(*Pmnu)=mnu;
}

int main()
{
int i,j,key,red=TRUE;
int mnu=1,sub=0,fuel=1,StackSub=0;
int vis=0,os=0,arrow=0,ck=0,control=0,bye=FALSE,AandO=FALSE;
double *nums,buff,b2,vars[26];
int cks[64],counter=1,numb=0,lbl[26],var_i=0;
unsigned char SO[64];
char GotoBuf[5]={0};
double bns[64];
unsigned char *prgm;
char in[20]={0};
int pc=0,pra=1,pro=0;
/*int ec=0,ctp=7;
char opps4[]={MULT,DIV,SUB,PLUS,0};*/
	alpha=0;ctrl=FALSE;shift=FALSE,none=TRUE;
	deg=0;ins=TRUE;right=TRUE;
	nums=malloc(STACKSIZE*sizeof(double));
	prgm=malloc(PRGMSIZE);
	if ((!nums)||(!prgm))
	{
		free(prgm);
		free(nums);
		printf("\nNot enough memory to allocate stack!");
		ngetchx();
		return 1;
	}
	memset(vars,0,208);
	memset(prgm,0,PRGMSIZE);
	randomize();
	do{
		if (red)
		{
			clrscr();
			DrawS(6,8);
		}
		if (errno)
		{
			switch (errno)
			{
				case DOMAIN:
					ck=err("Domain error!");break;
				case SING:
					ck=err("Singularity error!");break;
				case OVERFLOW:
					ck=err("Overflow error!");break;
				case UNDERFLOW:
					ck=err("Underflow error!");break;
				case TLOSS:
					ck=err("Precision error!");break;
			}
			DROP(1);
			errno=0;
		}
		if (pc) red=FALSE;
		else red=TRUE;
		/*if (ck) {key=ck;ck=0;}*/
		if((ck)&&(!pc))
		{
			key=ck;
			ck=0;
		}
		else if (numb)
		{
			key=cks[counter];
			red=FALSE;
			/*gotoxy(1,1);
			printf("%d-%d",SIN,key);
			ngetchx();*/
		}
		else if (control) {key=control;control=0;}
		else if (!pc) key=ngetchx();

		if ((key==POW)&&(alpha));
		else if (opp(key)) {arrow=0;os=0;}

		if ((alpha)&&(key!=POW)&&(!numb))
		{
			none=TRUE;
			if (opp(key))  {key+=512;AandO=TRUE;}
			none=FALSE;
		}
		/*if (strchr(opps4,key)&&(alpha)) {key+=512;AandO=TRUE;} */
		if (key==SHIFT+256) key=SHIFT;
		switch(key)
		{
			case 0:
				break;
			case ENTER:
				if (arrow) {os=0;arrow=0;StackSub=0;}
				else ck=DUP();
				break;
			case UP:
				red=FALSE;
				/*if kbhit, exit drawstack*/
				if ((arrow==6)&&(arrow+os<vis)){os++;red=TRUE;}
				else if (arrow+os<vis)
				{
					if (arrow)
					{
						if (arrow+os<10) gotoxy(2,8-arrow);
						else gotoxy(3,8-arrow);
						printf(":");
					}
					else DrawMenu(0,&StackSub);
					arrow++;
					if (arrow+os<10) gotoxy(2,8-arrow);
					else gotoxy(3,8-arrow);
					#ifdef PC
						printf("%c",16);
					#else
						printf("%c%c",0xF6,189);
					#endif
				}
				break;
			case DOWN:
				red=FALSE;
				if ((arrow==1)&&(arrow+os>1)){os--;red=TRUE;}
				else if (arrow>1)
				{
					if (arrow+os<10) gotoxy(2,8-arrow);
					else gotoxy(3,8-arrow);
					printf(":");
					arrow--;
					if (arrow+os<10) gotoxy(2,8-arrow);
					else gotoxy(3,8-arrow);
					#ifdef PC
						printf("%c",16);
					#else
						printf("%c%c",0xF6,189);
					#endif
				}
				break;
			case ESC:
				if (arrow){os=0;arrow=0;StackSub=0;}
				else if (mnu!=1) {mnu=1;DrawMenu(mnu,&sub);red=FALSE;}
				else bye=TRUE;
				break;
			case DEL:
				if (arrow) ck=DROP(os+arrow);
				else ck=DROP(1);
				break;
			case PLUS:
				if (vis<2){ck=TFA();break;}
				nums[0]=nums[1]+nums[0];
				DROP(2);
				break;
			case SUB:
				if (vis<2){ck=TFA();break;}
				nums[0]=nums[1]-nums[0];
				DROP(2);
				break;
			case MULT:
				if (vis<2){ck=TFA();break;}
				nums[0]=nums[1]*nums[0];
				DROP(2);
				break;
			case DIV:
				if (vis<2){ck=TFA();break;}
				if (!nums[0])
				{
					ck=err("Can't divide by zero!");
					break;
				}
				nums[0]=nums[1]/nums[0];
				DROP(2);
				break;
			case ALPHA:
				if(!alpha) {none=FALSE;alpha=1;}
				else {none=TRUE;alpha=0;}
				shift=FALSE;ctrl=FALSE;
				DrawStat();
				red=FALSE;
				break;
			case (ALPHA+256):
				alpha=2;none=FALSE;
				shift=FALSE;ctrl=FALSE;
				DrawStat();
				red=FALSE;
				break;
			case SHIFT:
				none=shift;
				shift=!shift;
				alpha=FALSE;ctrl=FALSE;
				DrawStat();
				red=FALSE;
				break;
			case CTRL:
				none=ctrl;
				ctrl=!ctrl;
				alpha=FALSE;shift=FALSE;
				DrawStat();
				red=FALSE;
				break;
			case POW:
				if (alpha)
				{
					deg++;
					if (deg==3) deg=0;
					if (alpha==1) {alpha=0;shift=FALSE;none=TRUE;}
					DrawStat();
					red=FALSE;
				}
				else
				{
					if (vis<2){ck=TFA();break;}
					nums[0]=pow(nums[1],nums[0]);
					DROP(2);
				}
				break;
			case LOG:
				if (!vis){ck=TFA();break;}
				nums[0]=log10(nums[0]);
				break;
			case LN:
				if (!vis){ck=TFA();break;}
				nums[0]=log(nums[0]);
				break;
			case SIN:
				/*printf("SINE:-c:%d n:%d",counter,numb);
				getch();*/
				if (!vis){ck=TFA();break;}
				if (deg==1) nums[0]=sin(nums[0]/180*pi);
				else if (deg==2) nums[0]=sin(nums[0]/200*pi);
				else nums[0]=sin(nums[0]);
				break;
			case COS:
				if (!vis){ck=TFA();break;}
				if (deg==1) nums[0]=cos(nums[0]/180*pi);
				else if (deg==2) nums[0]=cos(nums[0]/200*pi);
				else nums[0]=cos(nums[0]);
				break;
			case TAN:
				if (!vis){ck=TFA();break;}
				if (deg==1) nums[0]=tan(nums[0]/180*pi);
				else if (deg==2) nums[0]=tan(nums[0]/200*pi);
				else nums[0]=tan(nums[0]);
				break;
			case SQUARE:
				if (!vis){ck=TFA();break;}
				nums[0]=pow(nums[0],2);
				break;
			case XROOT:
				if (vis<2){ck=TFA();break;}
				nums[0]=pow(nums[1],1/nums[0]);
				DROP(2);
				break;
			case ALOG:
				if (!vis){ck=TFA();break;}
				nums[0]=pow(10.0,nums[0]);
				break;
			case EXP:
				if (!vis){ck=TFA();break;}
				nums[0]=exp(nums[0]);
				break;
			case ASIN:
				if (!vis){ck=TFA();break;}
				if (deg==1) nums[0]=asin(nums[0])/pi*180;
				else if (deg==2) nums[0]=asin(nums[0])/pi*200;
				else nums[0]=asin(nums[0]);
				break;
			case ACOS:
				if (!vis){ck=TFA();break;}
				if (deg==1) nums[0]=acos(nums[0])/pi*180;
				else if (deg==2) nums[0]=acos(nums[0])/pi*200;
				else nums[0]=acos(nums[0]);
				break;
			case ATAN:
				if (!vis){ck=TFA();break;}
				if (deg==1) nums[0]=atan(nums[0])/pi*180;
				else if (deg==2) nums[0]=atan(nums[0])/pi*200;
				else nums[0]=atan(nums[0]);
				break;
			case SQR:
				if (!vis){ck=TFA();break;}
				if (nums[0]<0)
				{
					ck=err("Bad argument type!");
					break;
				}
				nums[0]=sqrt(nums[0]);
				break;
			case CBR:
				if (!vis){ck=TFA();break;}
				nums[0]=pow(nums[0],1./3.);
				break;
			case INV:
				if (!nums[0])
				{
					ck=err("Bad argument type!");
					break;
				}
				if (!vis){ck=TFA();break;}
				nums[0]=pow(nums[0],-1);
				break;
			case PI:
				if (vis==STACKSIZE)
				{
					ck=err("Stack is full!");
					break;
				}
				if (vis) DUP();
				else vis=1;
				nums[0]=pi;
				break;
			case INS:
				ins=!ins;
				shift=FALSE;none=TRUE;
				DrawStat();
				red=FALSE;
				break;
			case SIGN:
				if ((vis)&&(nums[0])) nums[0]=-nums[0];
				else red=FALSE;
				break;
			case ON:
				if (arrow){arrow=0;os=0;}
				else if (vis) vis=0;
				else red=FALSE;
				break;
			case RIGHT:
				if (ctrl) right=TRUE;
				else
				{
					check2
					buff=nums[0];
					nums[0]=nums[1];
					nums[1]=buff;
				}
				break;
			case F5:
				if (arrow) {StackSub++;DrawMenu(0,&StackSub);}
				else if (mnu!=1) {sub++;DrawMenu(mnu,&sub);}
				red=FALSE;
				break;
			case LEFT:
				if (ctrl) right=FALSE;
				else red=FALSE;
				break;
			case F5+256:
				if (arrow) {StackSub--;DrawMenu(0,&StackSub);}
				else {sub--;DrawMenu(mnu,&sub);}
				red=FALSE;
				break;
			case F6:
				if (mnu!=1)
				{
					mnu=1;sub=0;
					DrawMenu(mnu,&sub);
				}
				red=FALSE;
				break;
			case F1:
			case F2:
			case F3:
			case F4:
				red=FALSE;
				if ((mnu==1)&&(!arrow)){mnu=key-F1+2;sub=0;DrawMenu(mnu,&sub);break;}
				fuel=key-F1;
				if (arrow) SO[0]=MatchF(0,StackSub,&fuel);
				else SO[0]=MatchF(mnu,sub,&fuel);
				cks[0]=0;
				if (SO[0]) {numb=1;counter=-1;}
				ck=fuel;
				break;
			case PRGM:
				{
					memset(lbl,0,52);
					i=0;
					while(prgm[wa(i,prgm)])
					{
						j=prgm[wa(i,prgm)];
						if((j>=LBL_A)&&(j<=LBL_Z))
						{
							if (lbl[j-LBL_A])
							{
								clrscr();
								printf("Program Error\n\n");
								printf("Duplicate labels\n");
								printf("detected. Each label\n");
								printf("may be used only once");
								TopDark();
								ngetchx();
							}
							else lbl[j-LBL_A]=i+1;
						}
						i++;
					}
					red=FALSE;
					pc=1;
					key=0;
					break;
				}
			case VARS:
				/*PrgmEd(unsigned char *prgm,int *cks,double *bns,unsigned char *SO,
					 int *Ppro,int *Ppra,double *nums,char *in,int *Pkey,
					 int *Pcontrol,int *Pnumb,int *Psub,int *Pmnu)*/
				PrgmEd(&prgm[0],&cks[0],&bns[0],&SO[0],&pro,&pra,&in[0],
							 &key,&control,&numb,&sub,&mnu);
				break;
			default:
				if (AandO) {AandO=FALSE;key-=512;}
				if ((key>=F1)&&(key<=F4));
				else
				{
					if ((!allow(key))||(arrow)) {red=FALSE;break;}
					clrscr();
					DrawS(5,7);
					if (vis==STACKSIZE)
					{
						ck=err("Stack is full!");
						break;
					}
					buff=0;
					Input(&bns[0],&control,&cks[0],&SO[0],allow(key),&numb,&mnu,&sub,in);
					counter=-1;
					/*8-24*/
					if (numb) red=FALSE;/*CHANGE*/
				}
		}

		if (pc)
		{
			if (kbhit())
			{
				if (ngetchx()==ON)
				{
					clrscr();
					printf("Program Halted\n\n");
					/*if (cks[0])
						printf("Instruction %s\n",TextCorr(KeyCorr(cks[0])));
					else if (prgm[wa(pc-1,prgm)]==NUMBER)
						printf("Instruction %g\n",bns[0]);
					else
						printf("Instruction %s\n",TextCorr(SO[0]));*/
					printf("Program halted at %d.\n",pc-1);
					/*printf("was the last executed\n");
					printf("instruction.");*/
					TopDark();
					ngetchx();
					pc=0;
					key=0;
					red=TRUE;
				}
			}
			else if (ck)
			{
				clrscr();
				printf("Program Error\n\nUnable to execute\n");
				/*if (cks[0])
					printf("instruction %s\n",TextCorr(KeyCorr(cks[0])));
				else
					printf("instruction %s\n",TextCorr(SO[0]));*/
				printf("instruction at \nprogram step %d.",pc-1);
				TopDark();
				ngetchx();
				ck=0;
				pc=0;
				red=TRUE;
			}
			else if ((pc==PRGMSIZE)||(!prgm[wa(pc-1,prgm)]))
			{
				pc=0;
				while (kbhit()) getch();
			}
			else
			{
				SO[0]=0;
				cks[0]=0;
				bns[0]=0;
				numb=1;
				counter=-1;
				if ((prgm[wa(pc-1,prgm)]>=XYEQ)&&(prgm[wa(pc-1,prgm)]<=XYLE))
				{
					if (vis<2) {ck=TFA();}
					SO[0]=prgm[wa(pc-1,prgm)];
				}
				if ((prgm[wa(pc-1,prgm)]>=X0EQ)&&(prgm[wa(pc-1,prgm)]<=X0LE))
				{
					if (!vis) {ck=TFA();}
					SO[0]=prgm[wa(pc-1,prgm)];
				}
				if (prgm[wa(pc-1,prgm)]==NUMBER) memcpy(&bns[0],prgm+wa(pc-1,prgm)+1,8);
				else if ((prgm[wa(pc-1,prgm)]>=GTO_A)&&(prgm[wa(pc-1,prgm)]<=GTO_Z))
				{
					SO[0]=prgm[wa(pc-1,prgm)];
					if(!lbl[prgm[wa(pc-1,prgm)]-GTO_A])
					{
						clrscr();
						printf("Program Error\n\n");
						printf("Unable to %s.\n",TextCorr(SO[0]));
						printf("Matching label not\nfound.");
						TopDark();
						ngetchx();
						pc=-1;
						red=TRUE;
					}
					else pc=lbl[prgm[wa(pc-1,prgm)]-GTO_A];
				}
				else if (prgm[wa(pc-1,prgm)]==GTO_i)
				{
					SO[0]=prgm[wa(pc-1,prgm)];
					if ((var_i<1)||(var_i>26))
					{
						clrscr();
						printf("Program Error\n\n");
						printf("The variable i is\n");
						printf("out of range. Valid\n");
						printf("values are 1 to 26.");
						TopDark();
						ngetchx();
						pc=-1;
						red=TRUE;
					}
					if(!lbl[var_i-1])
					{
						clrscr();
						printf("Program Error\n\n");
						printf("Unable to GOTO %c.\n",65+var_i-1);
						printf("Matching label not\nfound.");
						TopDark();
						ngetchx();
						pc=-1;
						red=TRUE;
					}
					else pc=lbl[var_i-1];
				}
				else if (prgm[wa(pc-1,prgm)]==STOP) {pc=-1;counter=0;}
				else if (prgm[wa(pc-1,prgm)]==XYEQ) {if((nums[0]!=nums[1])&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==XYNE) {if((nums[0]==nums[1])&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==XYGT) {if((nums[0]<=nums[1])&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==XYLT) {if((nums[0]>=nums[1])&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==XYGE) {if((nums[0]<nums[1])&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==XYLE) {if((nums[0]>nums[1])&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==X0EQ) {if((nums[0])&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==X0NE) {if((!nums[0])&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==X0GT) {if((nums[0]<=0)&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==X0LT) {if((nums[0]>=0)&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==X0GE) {if((nums[0]<0)&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]==X0LE) {if((nums[0]>0)&&(!ck)) pc++;}
				else if (prgm[wa(pc-1,prgm)]>=KEYSTART)
					cks[0]=RevKeyCorr(prgm[wa(pc-1,prgm)]);
				else SO[0]=prgm[wa(pc-1,prgm)];
				/*if (vars[4])
				{
					clrscr();
					printf("%s PC:%d SO:%d cks:%d bns:%g A:%g",
						TextCorr(prgm[wa(pc-1,prgm)]),pc,SO[0],cks[0],bns[0],vars[0]);
					if (ngetchx()==ESC) exit(0);
				}*/
				pc++;
			}
		}

		if (numb) counter++;
		if (counter==numb)/*&&(!pc))*/
		{
			if (red||1)
			{
				clrscr();
				DrawS(6,8);
			}
			numb=0;
		}

		if (numb)
		{
			if ((!SO[counter])&&(!cks[counter]))
			{
				if (vis==STACKSIZE) ck=err("Stack is full!");
				else
				{
					if (vis) DUP();
					else vis=1;
					nums[0]=bns[counter];
				}
			}
			else if (SO[counter]==VAR_i)
			{
				if (vis==STACKSIZE) ck=err("Stack is full!");
				else
				{
					if ((var_i<1)||(var_i>26))
					{
						clrscr();
						printf("Program Error\n\n");
						printf("The variable i is\n");
						printf("out of range. Valid\n");
						printf("values are 1 to 26.");
						TopDark();
						ngetchx();
						pc=0;
						red=TRUE;
					}
					else
					{
						if (vis) DUP();
						else vis=1;
						nums[0]=vars[var_i-1];
					}
				}
			}
			else if ((SO[counter]>=VAR_A)&&(SO[counter]<=VAR_Z))
			{
				if (vis==STACKSIZE) ck=err("Stack is full!");
				else
				{
					if (vis) DUP();
					else vis=1;
					nums[0]=vars[SO[counter]-VAR_A];
				}
			}
			else if ((SO[counter]>=STO_A)&&(SO[counter]<=STO_Z))
			{
				if (!vis) ck=TFA();
				else
				{
					vars[SO[counter]-STO_A]=nums[0];
					DROP(1);
				}
			}
			else if (SO[counter]==STO_i)
			{
				if (!vis) ck=TFA();
				else
				{
					var_i=nums[0];
					DROP(1);
				}
			}
			else if (SO[counter]==IND_i)
			{
				if (!vis) ck=TFA();
				else if (var_i<1||var_i>26)
				{
					clrscr();
					printf("Program Error\n\n");
					printf("The variable i is\n");
					printf("out of range. Valid\n");
					printf("values are 1 to 26.");
					TopDark();
					ngetchx();
					pc=0;
					red=TRUE;
				}
				else
				{
					vars[var_i-1]=nums[0];
					DROP(1);
				}
			}
			else switch (SO[counter])
			{
				case 0:
					break;
				/*case ON:
					vis=0;
					break;*/
				case ABS:
					if (!vis){ck=TFA();break;}
					nums[0]=fabs(nums[0]);
					break;
				case INT:
					if (!vis){ck=TFA();break;}
					modf(nums[0],&buff);
					nums[0]=buff;
					break;
				case RAND:
					checknew
					nums[0]=(double)rand()/(double)RAND_MAX;
					break;
				case TODEG:
					check1
					nums[0]=nums[0]/pi*180;
					break;
				case TORAD:
					check1
					nums[0]=nums[0]/180*pi;
					break;
				case GCD:
					check2
					if (modf(nums[0],&buff)||modf(nums[1],&buff))
					{
						ck=err("Bad argument types!");
						break;
					}
					if (!nums[0]) {nums[0]=nums[1];DROP(2);break;}
					if (!nums[1]) {DROP(2);break;}
					nums[0]=fabs(nums[0]);
					nums[1]=fabs(nums[1]);
					if (nums[0]==nums[1]) {DROP(2);break;}
					if (nums[0]<nums[1])
					{
						buff=nums[0];
						nums[0]=nums[1];
						nums[1]=buff;
					}
					modf(nums[0]/2.,&b2);b2++;
					for (buff=b2;buff>0;buff--)
						if (!modf(nums[0]/buff,&b2)&&(!modf(nums[1]/buff,&b2)))
							break;
					DROP(2);
					nums[0]=buff;
					break;
				case LCM:
					check2
					if (modf(nums[0],&buff)||modf(nums[1],&buff))
					{
						ck=err("Bad argument types!");
						break;
					}
					if (!nums[0]||!nums[1]) {DROP(2);nums[0]=0;break;}
					i=(nums[0]<0)^(nums[1]<0);
					if (i) i=-1;
					else i=1;
					nums[0]=fabs(nums[0]);
					nums[1]=fabs(nums[1]);
					if (nums[0]<nums[1])
					{
						buff=nums[0];
						nums[0]=nums[1];
						nums[1]=buff;
					}
					for (buff=nums[0];buff<=nums[0]*nums[1];buff+=nums[0])
						if (!modf(buff/nums[1],&b2)) break;
					DROP(2);
					nums[0]=i*buff;
					break;
				case INTDIV:
					check2
					if (!nums[0])
					{
						ck=err("Can't divide by zero!");
						break;
					}
					modf(nums[1]/nums[0],&buff);
					nums[0]=buff;
					DROP(2);
					break;
				case ISPRIME:
					check1
					i=TRUE;
					if ((nums[0]<=1)||modf(nums[0],&b2)) {break;}
					modf(sqrt(nums[0]),&b2);
					for (buff=b2+1;buff>1;buff--)
					{
						if (!modf(nums[0]/buff,&b2))
						{
							i=FALSE;
							break;
						}
					}
					nums[0]=i;
					break;
				case MOD:
					check2
					if (!nums[0])
					{
						ck=err("Can't divide by zero!");
						break;
					}
					nums[0]=fmod(nums[1],nums[0]);
					DROP(2);
					break;
				case CEIL:
					check1
					nums[0]=ceil(nums[0]);
					break;
				case FLOOR:
					check1
					nums[0]=floor(nums[0]);
					break;
				case DROPN:
					if (nums[0]<=0) {DROP(1);break;}
					if ((vis-1)<ceil(nums[0])) {ck=TFA();break;}
					for (i=ceil(nums[0])+1;i>0;i--) DROP(1);
					break;
				case DEPTH:
					checknew
					nums[0]=vis-1;
					break;
				case PICK:
					check1
					if (nums[0]<=0) break;
					if ((vis-1)<ceil(nums[0])) {ck=TFA();break;}
					nums[0]=nums[ceil(nums[0])];
					break;
				case OVER:
					checknew
					nums[0]=nums[2];
					break;
				case DUPN:
					check2
					if (ceil(nums[0])<=0) {DROP(1);break;}
					if ((vis-1)<ceil(nums[0])) {ck=TFA();break;}
					if (vis+nums[0]>STACKSIZE)
					{
						ck=err("Stack is full!");
						break;
					}
					i=ceil(nums[0]);
					DROP(1);
					memmove(nums+i,nums,vis*8);
					vis+=i;
					break;
				case ROT:
					check2
					buff=nums[vis-1];
					DROP(vis);
					DUP();
					nums[0]=buff;
					break;
				case SINH:
					check1
					if (deg==1) nums[0]=sinh(nums[0]/180*pi);
					else if (deg==2) nums[0]=sinh(nums[0]/200*pi);
					else nums[0]=sinh(nums[0]);
					break;
				case COSH:
					check1
					if (deg==1) nums[0]=cosh(nums[0]/180*pi);
					else if (deg==2) nums[0]=cosh(nums[0]/200*pi);
					else nums[0]=cosh(nums[0]);
					break;
				case TANH:
					check1
					if (deg==1) nums[0]=tanh(nums[0]/180*pi);
					else if (deg==2) nums[0]=tanh(nums[0]/200*pi);
					else nums[0]=tanh(nums[0]);
					break;
				case E:
					checknew
					nums[0]=e;
					break;
				case MAXR:
					checknew
					nums[0]=1.7E+308;
					break;
				case MINR:
					checknew
					nums[0]=1.7E-307;
					break;
				case DRPS:
					j=arrow+os;
					for(i=0;i<j;i++)
					DROP(1);
					break;
				case DUPS:
					memmove(nums+os+arrow,nums,vis*8);
					vis+=os+arrow;
					break;
				case GOTO:
					clrscr();
					printf("Goto Level ?");
					GotoBuf[0]=0;
					do
					{
						gotoxy(1,8);
						printf("%s_ ",GotoBuf);
						i=ngetchx();
						if ((strlen(GotoBuf)<4)&&(i>=48)&&(i<=57))
						{
							GotoBuf[strlen(GotoBuf)+1]=0;
							GotoBuf[strlen(GotoBuf)]=i;
						}
						if ((strlen(GotoBuf))&&(i==DEL)) GotoBuf[strlen(GotoBuf)-1]=0;
						if (i==ENTER)
						{
							i=atoi(GotoBuf);
							if (i)
							{
								if (i<=vis){arrow=1;os=i-1;}
								else
								{
									if (vis<6){os=0;arrow=vis;}
									else{arrow=6;os=vis-6;}
								}
							}
							i=ESC;
						}
					}while((i!=ESC)&&(i!=ON));
					break;
				case KEEP:
					vis=os+arrow;
					break;
				case LEVEL:
					checknew
					nums[0]=os+arrow;
					break;
				case PICKS:
					checknew
					nums[0]=nums[os+arrow];
					break;
				case ROLLS:
					buff=nums[os+arrow-1];
					memmove(nums+1,nums,(os+arrow-1)*8);
					nums[0]=buff;
					break;
				case ROLLDS:
					buff=nums[0];
					memmove(nums,nums+1,(os+arrow-1)*8);
					nums[os+arrow-1]=buff;
					break;
				case ROLL:
					check1
					i=(int)nums[0];
					if (i>=vis) {ck=TFA();break;}
					DROP(1);
					if (i<1) break;
					buff=nums[i-1];
					memmove(nums+1,nums,(i-1)*8);
					nums[0]=buff;
					break;
				case ROLLD:
					check1
					i=(int)nums[0];
					if (i>=vis) {ck=TFA();break;}
					DROP(1);
					if (i<1) break;
					buff=nums[0];
					memmove(nums,nums+1,(i-1)*8);
					nums[i-1]=buff;
					break;
			}
		}
		if ((key!=ALPHA)&&(key!=CTRL)&&(key!=SHIFT))
		{
			if (alpha==1) {alpha=0;none=TRUE;DrawStat();}
			if (ctrl) {ctrl=FALSE;none=TRUE;DrawStat();}
			if (shift) {shift=FALSE;none=TRUE;DrawStat();}
		}
	}while(!bye);
	free(prgm);
	free(nums);
	return 0;
}