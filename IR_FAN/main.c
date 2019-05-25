#include<reg52.h>
#define DataPort P0
sbit IR=P3^2;
sbit DU=P2^0;
sbit WE=P2^1;
sbit mada=P3^1;
sbit led88=P2^2;
unsigned int zl_count=0;
unsigned int bj_flag=0;
unsigned int  time=0;
unsigned int i=0;
unsigned char pwm;
unsigned char code dofly_DuanMa[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char code fz[8]={0xfe,0xfc,0xfd,0xf9,0xfb,0xf3,0xf7,0xf6}; //��ת
unsigned char code zz[8]={0xf6,0xf7,0xf3,0xfb,0xf9,0xfd,0xfc,0xfe}; //��ת
unsigned char  irtime;
bit irpro_ok,irok;
unsigned char IRcord[4];
unsigned char irdata[33];
unsigned int yt_flag=0;


void Ir_work(void);
void Ircordpro(void);
void bjdj();
void delay(unsigned int xms);
void delay(unsigned int xms)
{
    unsigned int x,y;
    for(x=xms;x>0;x--)
        for(y=110;y>0;y--);
}

void tim0_isr (void) interrupt 1
{
  irtime++;
}




void mada_con() interrupt 3{
	if(time==14000)time=0;
	TH1=(65536-1000)/256;
	TL1=(65536-1000)%256;
	zl_count++;
	time++;
	if(zl_count==pwm){
		mada=0;
	}
	
	if(zl_count==10){
		mada=1;
		zl_count=0;
	}
}






void EX0_ISR (void) interrupt 0 //�ⲿ�ж�0������
{
  static unsigned char  i;             //���պ����źŴ���
  static bit startflag;                //�Ƿ�ʼ�����־λ

if(startflag)                         
   {
	if(irtime<63&&irtime>=32)//������ TC9012��ͷ�룬9ms+4.5ms
						i=0;
			irdata[i]=irtime;//�洢ÿ����ƽ�ĳ���ʱ�䣬�����Ժ��ж���0����1
			irtime=0;
			i++;
			 if(i==33)
	  			{
	  			 irok=1;
				 i=0;
	  			}
	}
	 else
		{
		irtime=0;
		startflag=1;
		}
}


void TIM0init(void)//��ʱ��0��ʼ��
{

	TH0=0x00; //����ֵ
	TL0=0x00; //��ʼ��ֵ
	ET0=1;    //���ж�
	TR0=1;    
}

void TIM1init(void)//��ʱ��1��ʼ��
{
	TMOD=0x12;
	TH1=(65536-1000)/256;
	TL1=(65536-1000)%256;
	pwm=0;
	mada=0;
	ET1=1;
	TR1=0;

}



void EX0init(void)
{
 IT0 = 1;   //ָ���ⲿ�ж�0�½��ش�����INT0 (P3.2)
 EX0 = 1;   //ʹ���ⲿ�ж�
 EA = 1;    //�����ж�
}


void Ir_work(void)//�����ֵɢת����
{
	switch(IRcord[2])//�жϵ���������ֵ
	     {
		 case 0x0c:DataPort=dofly_DuanMa[1];TR1=1;mada=1;bj_flag=1;pwm=2;break;//1 ��ʾ��Ӧ�İ���ֵ
		 case 0x18:DataPort=dofly_DuanMa[2];pwm=5;break;//2
		 case 0x5e:DataPort=dofly_DuanMa[3];pwm=0;break;//3
	     default:break;
		 }
	  irpro_ok=0;//������ɱ�־

}

void Ircordpro(void)//������ֵ������
{ 
	unsigned char i, j, k;
	unsigned char cord,value;
	
	k=1;
	for(i=0;i<4;i++)      //����4���ֽ�
	 {
	  for(j=1;j<=8;j++) //����1���ֽ�8λ
	     {
	      cord=irdata[k];
	      if(cord>7)//����ĳֵΪ1������;����о��Թ�ϵ������ʹ��12M���㣬��ֵ������һ�����
	         value|=0x80;
	      if(j<8)
		    {
			 value>>=1;
			}
	       k++;
	     }
	 IRcord[i]=value;
	 value=0;     
	 } 
	 irpro_ok=1;//������ϱ�־λ��1
}


void bjdj(){
	if(bj_flag)
	{
		if(time<5000)
		{
			for(i=0;i<7;i++)
			{
				P1=zz[i];
				delay(5);
			}
		}
		
		if(time>=5000 && time<7000)
		{
			P1=0x0;
		}
		
		if(time>=7000 && time<12000)
		{
			for(i=0;i<7;i++)
			{
				P1=fz[i];
				delay(5);
			}
		}
		
		if(time>=12000 && time<14000)
		{
			P1=0x0;
		}
	}	
}




void main()
{
	EX0init();
	TIM0init();
	TIM1init();
	led88=0;
	DataPort=0xfe;
	WE=0;
	DataPort=0x0;
 while(1)
   {
	bjdj();
	if(irok)                        //������պ��˽��к��⴦��
	  {   
	   Ircordpro();
 	   irok=0;
	  }

    if(irpro_ok)                   //�������ú���й��������簴��Ӧ�İ�������ʾ��Ӧ�����ֵ�
	  {
	   Ir_work();
  	  }
   }
}
  
  