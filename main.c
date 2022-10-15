

#include "reg52.h"			 //此文件中定义了单片机的一些特殊功能寄存器
#define KEY P1
typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

sbit LSA=P2^2; // 138译码器输入引脚
sbit LSB=P2^3;
sbit LSC=P2^4;
sbit BEEP=P3^7; // 蜂鸣器引脚

u8 code smgduan[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
					0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};// 共阴数码管显示0~F的值
u8 keyvalue; // 矩阵键盘值
char ssec,sec,min,hour; // 毫秒，秒，分
u8 DisplayData[8];	// 数码管段选显示数组

					
// 非精确延时函数 1ms					
void delay(u16 i)
{
	while(i--);	
}

// 中断初始化函数
void InterruptInit()
{
	TMOD=0X01; // 选择为定时器0模式，工作方式1，仅用TR0打开启动。
	TH0=0Xd8;	 // 给定时器赋初值，定时10ms
	TL0=0Xf0;	

	TR0=1; // 打开定时器	
	ET0=1; // 打开定时器0中断允许
	EA=1; // 打开总中断
	EX0=1; // 打开外部中断0
	EX1=1; // 打开外部中断1
	
	IT0=1; // 外部中断0,1负跳变触发
	IT1=1;
	PX0=1; //设置中断优先等级:外部中断优于内部中断
	PX1=1; 
}


// 数码管显示函数
void DigDisplay()
{
	u8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 // 位选，选择点亮的数码管，
		{
			case(0):
				LSA=1;LSB=1;LSC=1; break; // 显示第7位
			case(1):
				LSA=0;LSB=1;LSC=1; break; // 显示第6位
			case(2):
				LSA=1;LSB=0;LSC=1; break; // 显示第5位
			case(3):
				LSA=0;LSB=0;LSC=1; break; // 显示第4位
			case(4):
				LSA=1;LSB=1;LSC=0; break; // 显示第3位 
			case(5):
				LSA=0;LSB=1;LSC=0; break; // 显示第2位 
			case(6):
				LSA=1;LSB=0;LSC=0; break; // 显示第1位 
			case(7):
				LSA=0;LSB=0;LSC=0; break; // 显示第0位	
		}
		P0=DisplayData[i]; // 发送段码
		delay(100); // 间隔一段时间扫描	
		P0=0x00; // 消隐
	}
}

// 数码管段选赋值函数
void datapros()
{
	DisplayData[7]=0x00;	// 正常工作状态前两位不显示
	DisplayData[6]=0x00;	
	DisplayData[5]=smgduan[hour/10];	// 小时十位数值
	DisplayData[4]=smgduan[hour%10] + 0x80; // 小时个位数值 , 加0x80 显示小数点
	DisplayData[3]=smgduan[min/10];   // 分钟十位数值
	DisplayData[2]=smgduan[min%10] + 0x80; // 分钟个位数值，同样加小数点隔开
	DisplayData[1]=smgduan[sec/10];	// 秒十位值
	DisplayData[0]=smgduan[sec%10];	 // 秒个位
}

// 时间溢出处理函数
void updateTime()
{
	keyvalue=0x00; // 矩阵按键值，初始为0x00
	if(sec>=60)
	{
		sec=0;
	}
	if(sec<0)
	{
		sec=59;
	}
	if(min>=60)
	{
		min=0;
	}
	if(min<0)
	{
		min=59;
	}
	if(hour>=24)
	{
		hour=0;
	}
	if(hour<0)
	{
		hour=23;
	}
}

// 矩阵按键检测 + 时间调整函数
void changeDataPros()
{
	KEY = 0x0f; // 扫描列
	if(KEY!=0x0f)
	{
		delay(10);
		if(KEY!=0x0f) // 消抖流程
		{
			KEY = 0x0f; 
			switch(KEY)
			{
				case (0x07):
					keyvalue = 0x07;                                                                 
					break;
				case (0x0b):
					keyvalue = 0x0b;
					break;
				case (0x0d):
					keyvalue = 0x0d;
					break;
				case (0x0e):
					keyvalue = 0x0e;
					break;
			}
			KEY = 0xf0;  // 扫描行
			switch(KEY)
			{
				case (0x70):
					keyvalue = keyvalue + 0x70;
					break;
				case (0xb0):
					keyvalue = keyvalue + 0xb0;
					break;
				case (0xd0):
					keyvalue = keyvalue + 0xd0;
					break;
				case (0xe0):
					keyvalue = keyvalue + 0xe0;
					break;
			}
		}
			while(KEY!=0xf0);
	}
	switch (keyvalue)
	{
		case(0x77):	// 第1列第1行 小时加1
			hour++;	
			updateTime();
			break;
		case(0xb7):	// 第1列第2行 小时减1
			hour--;
			updateTime();
			break;
		case(0xd7):	// 第1列第3行 小时加10
			hour+=10;
			updateTime();	
			break;
		case(0xe7):	// 第1列第4行 小时减10
			hour-=10;
			updateTime();
			break;
		
		case(0x7b): // 第2列第1行 分钟加1
			min++;
			updateTime();
			break;
		case(0xbb): // 第2列第2行 分钟减1
			min--;
			updateTime();
			break;
		case(0xdb): // 第2列第3行 分钟加10
			min+=10;
			updateTime();
			break;
		case(0xeb): // 第2列4行 分钟减10
			min-=10;
			updateTime();
			break;
		
		case(0x7d):	// 第3列第1行 秒钟加1
			sec++;
			updateTime();
			break;
		case(0xbd): // 第3列第2行 秒钟减1
			sec--;
			updateTime();
			break;
		case(0xdd): // 第3列第3行 秒钟加10
			sec+=10;
			updateTime();
			break;
		case(0xed): // 第3列第4行 秒钟减10
			sec-=10;
			updateTime();
			break;
		
		case(0xee):
			keyvalue=0xff; // 调时模式结束标志
			break;
	}
	DisplayData[7]=0x40;	// 调时模式前两位提示符 -- 
	DisplayData[6]=0x40;	
	DisplayData[5]=smgduan[hour/10];	
	DisplayData[4]=smgduan[hour%10] + 0x80;
	DisplayData[3]=smgduan[min/10];
	DisplayData[2]=smgduan[min%10] + 0x80;
	DisplayData[1]=smgduan[sec/10];
	DisplayData[0]=smgduan[sec%10];
}

// 蜂鸣器控制函数
void beepTime()
{
		if(hour==23&&min==59&&(sec>=50&&sec<59)&&sec%2==0) // 23使59分50秒起2秒响一次共5次
		{
			BEEP=0;
			delay(1990);
			BEEP=1;
			delay(10);
		}
}

// 主函数
void main()
{	
	InterruptInit();  //定时器0初始化
	while(1)
	{
		datapros();  // 数码管段选赋值
		DigDisplay();	// 数码管显示
	}		
}

// 外部中断0 调时中断函数
void timeSet() interrupt 0
{
		EX0 = 0; // 关闭外部中断0 防止嵌套
	  while(1)
		{
			changeDataPros(); // 调时按键检测
			DigDisplay(); // 数码管显示
			if(keyvalue==0xff) // 退出调时函数
			{
				keyvalue=0x00;
				break;
			}
		}
		EX0 = 1;  // 开启外部中断0
}

// 外部中断1 60s倒计时中断函数
void timeCD() interrupt 2
{
	u8 nsec;	// 秒钟局部变量 范围0~59
		nsec=59;
		EX0 = 0;
		TH0=0Xd8;	//给定时器赋初值，定时10ms 
		TL0=0Xf0;	
	  while(1)
		{
			ssec++;
			delay(100);
			if(nsec==0) // 倒计时结束，退出倒计时中断
			{
				BEEP=0;
				delay(1990);
				BEEP=1;
				delay(10);
				break;
			}
			changeDataPros(); // 矩阵键盘检测
			DisplayData[7]=0x40;	// 倒计时模式：前6位提示符 ------
			DisplayData[6]=0x40;
			DisplayData[5]=0x40;	
			DisplayData[4]=0x40;	
			DisplayData[3]=0x40;	
			DisplayData[2]=0x40;	
			DisplayData[1]=smgduan[nsec/10]; // 倒计时秒数显示
			DisplayData[0]=smgduan[nsec%10];
			DigDisplay();
			if(ssec>=100)
			{
				ssec=0;
				nsec--;
			}
			if(keyvalue==0xff) // 按键退出倒计时中断
			{
				keyvalue=0x00;
				break;
			}
		}
		EX0 = 1;
}

// 定时器0中断函数 10ms中断一次 100次为1s
void Timer0() interrupt 1
{
	TH0=0Xd8;	//给定时器赋初值，定时10ms 1101100011110000
	TL0=0Xf0;
	ssec++;
	if(ssec>=100)  //1s
	{
		ssec=0;
		sec++;
		beepTime();
		if(sec>=60)  // 溢出控制
		{
			sec=0;
			min++;
			if(min>=60)
			{
				min=0;
				hour++;
				if(hour>=24)
				{
					hour=0; 
				}
			}
		}	
	}	
}
