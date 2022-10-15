

#include "reg52.h"			 //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
#define KEY P1
typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;

sbit LSA=P2^2; // 138��������������
sbit LSB=P2^3;
sbit LSC=P2^4;
sbit BEEP=P3^7; // ����������

u8 code smgduan[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
					0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};// �����������ʾ0~F��ֵ
u8 keyvalue; // �������ֵ
char ssec,sec,min,hour; // ���룬�룬��
u8 DisplayData[8];	// ����ܶ�ѡ��ʾ����

					
// �Ǿ�ȷ��ʱ���� 1ms					
void delay(u16 i)
{
	while(i--);	
}

// �жϳ�ʼ������
void InterruptInit()
{
	TMOD=0X01; // ѡ��Ϊ��ʱ��0ģʽ��������ʽ1������TR0��������
	TH0=0Xd8;	 // ����ʱ������ֵ����ʱ10ms
	TL0=0Xf0;	

	TR0=1; // �򿪶�ʱ��	
	ET0=1; // �򿪶�ʱ��0�ж�����
	EA=1; // �����ж�
	EX0=1; // ���ⲿ�ж�0
	EX1=1; // ���ⲿ�ж�1
	
	IT0=1; // �ⲿ�ж�0,1�����䴥��
	IT1=1;
	PX0=1; //�����ж����ȵȼ�:�ⲿ�ж������ڲ��ж�
	PX1=1; 
}


// �������ʾ����
void DigDisplay()
{
	u8 i;
	for(i=0;i<8;i++)
	{
		switch(i)	 // λѡ��ѡ�����������ܣ�
		{
			case(0):
				LSA=1;LSB=1;LSC=1; break; // ��ʾ��7λ
			case(1):
				LSA=0;LSB=1;LSC=1; break; // ��ʾ��6λ
			case(2):
				LSA=1;LSB=0;LSC=1; break; // ��ʾ��5λ
			case(3):
				LSA=0;LSB=0;LSC=1; break; // ��ʾ��4λ
			case(4):
				LSA=1;LSB=1;LSC=0; break; // ��ʾ��3λ 
			case(5):
				LSA=0;LSB=1;LSC=0; break; // ��ʾ��2λ 
			case(6):
				LSA=1;LSB=0;LSC=0; break; // ��ʾ��1λ 
			case(7):
				LSA=0;LSB=0;LSC=0; break; // ��ʾ��0λ	
		}
		P0=DisplayData[i]; // ���Ͷ���
		delay(100); // ���һ��ʱ��ɨ��	
		P0=0x00; // ����
	}
}

// ����ܶ�ѡ��ֵ����
void datapros()
{
	DisplayData[7]=0x00;	// ��������״̬ǰ��λ����ʾ
	DisplayData[6]=0x00;	
	DisplayData[5]=smgduan[hour/10];	// Сʱʮλ��ֵ
	DisplayData[4]=smgduan[hour%10] + 0x80; // Сʱ��λ��ֵ , ��0x80 ��ʾС����
	DisplayData[3]=smgduan[min/10];   // ����ʮλ��ֵ
	DisplayData[2]=smgduan[min%10] + 0x80; // ���Ӹ�λ��ֵ��ͬ����С�������
	DisplayData[1]=smgduan[sec/10];	// ��ʮλֵ
	DisplayData[0]=smgduan[sec%10];	 // ���λ
}

// ʱ�����������
void updateTime()
{
	keyvalue=0x00; // ���󰴼�ֵ����ʼΪ0x00
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

// ���󰴼���� + ʱ���������
void changeDataPros()
{
	KEY = 0x0f; // ɨ����
	if(KEY!=0x0f)
	{
		delay(10);
		if(KEY!=0x0f) // ��������
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
			KEY = 0xf0;  // ɨ����
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
		case(0x77):	// ��1�е�1�� Сʱ��1
			hour++;	
			updateTime();
			break;
		case(0xb7):	// ��1�е�2�� Сʱ��1
			hour--;
			updateTime();
			break;
		case(0xd7):	// ��1�е�3�� Сʱ��10
			hour+=10;
			updateTime();	
			break;
		case(0xe7):	// ��1�е�4�� Сʱ��10
			hour-=10;
			updateTime();
			break;
		
		case(0x7b): // ��2�е�1�� ���Ӽ�1
			min++;
			updateTime();
			break;
		case(0xbb): // ��2�е�2�� ���Ӽ�1
			min--;
			updateTime();
			break;
		case(0xdb): // ��2�е�3�� ���Ӽ�10
			min+=10;
			updateTime();
			break;
		case(0xeb): // ��2��4�� ���Ӽ�10
			min-=10;
			updateTime();
			break;
		
		case(0x7d):	// ��3�е�1�� ���Ӽ�1
			sec++;
			updateTime();
			break;
		case(0xbd): // ��3�е�2�� ���Ӽ�1
			sec--;
			updateTime();
			break;
		case(0xdd): // ��3�е�3�� ���Ӽ�10
			sec+=10;
			updateTime();
			break;
		case(0xed): // ��3�е�4�� ���Ӽ�10
			sec-=10;
			updateTime();
			break;
		
		case(0xee):
			keyvalue=0xff; // ��ʱģʽ������־
			break;
	}
	DisplayData[7]=0x40;	// ��ʱģʽǰ��λ��ʾ�� -- 
	DisplayData[6]=0x40;	
	DisplayData[5]=smgduan[hour/10];	
	DisplayData[4]=smgduan[hour%10] + 0x80;
	DisplayData[3]=smgduan[min/10];
	DisplayData[2]=smgduan[min%10] + 0x80;
	DisplayData[1]=smgduan[sec/10];
	DisplayData[0]=smgduan[sec%10];
}

// ���������ƺ���
void beepTime()
{
		if(hour==23&&min==59&&(sec>=50&&sec<59)&&sec%2==0) // 23ʹ59��50����2����һ�ι�5��
		{
			BEEP=0;
			delay(1990);
			BEEP=1;
			delay(10);
		}
}

// ������
void main()
{	
	InterruptInit();  //��ʱ��0��ʼ��
	while(1)
	{
		datapros();  // ����ܶ�ѡ��ֵ
		DigDisplay();	// �������ʾ
	}		
}

// �ⲿ�ж�0 ��ʱ�жϺ���
void timeSet() interrupt 0
{
		EX0 = 0; // �ر��ⲿ�ж�0 ��ֹǶ��
	  while(1)
		{
			changeDataPros(); // ��ʱ�������
			DigDisplay(); // �������ʾ
			if(keyvalue==0xff) // �˳���ʱ����
			{
				keyvalue=0x00;
				break;
			}
		}
		EX0 = 1;  // �����ⲿ�ж�0
}

// �ⲿ�ж�1 60s����ʱ�жϺ���
void timeCD() interrupt 2
{
	u8 nsec;	// ���Ӿֲ����� ��Χ0~59
		nsec=59;
		EX0 = 0;
		TH0=0Xd8;	//����ʱ������ֵ����ʱ10ms 
		TL0=0Xf0;	
	  while(1)
		{
			ssec++;
			delay(100);
			if(nsec==0) // ����ʱ�������˳�����ʱ�ж�
			{
				BEEP=0;
				delay(1990);
				BEEP=1;
				delay(10);
				break;
			}
			changeDataPros(); // ������̼��
			DisplayData[7]=0x40;	// ����ʱģʽ��ǰ6λ��ʾ�� ------
			DisplayData[6]=0x40;
			DisplayData[5]=0x40;	
			DisplayData[4]=0x40;	
			DisplayData[3]=0x40;	
			DisplayData[2]=0x40;	
			DisplayData[1]=smgduan[nsec/10]; // ����ʱ������ʾ
			DisplayData[0]=smgduan[nsec%10];
			DigDisplay();
			if(ssec>=100)
			{
				ssec=0;
				nsec--;
			}
			if(keyvalue==0xff) // �����˳�����ʱ�ж�
			{
				keyvalue=0x00;
				break;
			}
		}
		EX0 = 1;
}

// ��ʱ��0�жϺ��� 10ms�ж�һ�� 100��Ϊ1s
void Timer0() interrupt 1
{
	TH0=0Xd8;	//����ʱ������ֵ����ʱ10ms 1101100011110000
	TL0=0Xf0;
	ssec++;
	if(ssec>=100)  //1s
	{
		ssec=0;
		sec++;
		beepTime();
		if(sec>=60)  // �������
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
