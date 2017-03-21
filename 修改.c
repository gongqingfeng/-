#include "reg51.h"

sbit LED_L = P3^2;
sbit LED_R = P3^3;
sbit PWM1 = P2^0;//���ֵ������ʹ��
sbit PWM2 = P2^1;//���ֵ������ʹ��
sbit motor_control_1 = P0^0;//����ǰ��
sbit motor_control_2 = P0^1;//���ֺ���
sbit motor_control_3 = P0^2;//����ǰ��
sbit motor_control_4 = P0^3;//���ֺ���
sbit SEV_PWM = P0^7;  //���¶���ź�λ
sbit SEH_PWM = P0^6;  //���Ҷ���ź�λ

unsigned char count = 0;
unsigned char count1 = 0;
unsigned char count2 = 0;
unsigned char SEV_count = 3;
unsigned char SEH_count = 11;
unsigned char ucBluetoothData;

void initial_myself();
void initial_peripheral();
void T0_time();
void usart_service(void);
void delay_long(unsigned int uiDelayLong);
void delay(unsigned int xms);
void go_forward(void);//ǰ��
void fall_back(void);//����
void turn_left(void);//��ת
void turn_right(void);//��ת
void stop();//ɲ��

void main()
{
        initial_myself();
        delay_long(100);
        initial_peripheral();
        LED_L =1;
        LED_R =1;
        while(1)
        {
                usart_service();
                
                
        
        }

}
//���ڷ�����
void usart_service()
{
                
                switch(ucBluetoothData)
                {
	         case 0xFF:	 //�����λ			
			 								ucBluetoothData = 0x02;//����һֱ����
											SEH_count = 11;		  //����������̬
											SEV_count = 3;		  
											count = 0;
											break;
					 
	         case 0x50:  //�������ת
			 							ucBluetoothData = 0x02;//����һֱ����							
										 SEV_count--;
											if(SEV_count <= 3)
											{
												SEV_count = 3;	
											}
										 //SEV_count = 3;	
										 count = 0;
										 break;  
	         case 0x53:	 //�������ת		
			 								ucBluetoothData = 0x02;//����һֱ����	
			 								count = 0;									
					            			SEV_count++;
											if(SEV_count >= 9)
											{
													SEV_count = 9;	  
											}
											//SEV_count=9;
										
											break;
	           
	         case 0x51:	  //�������ת
			 								ucBluetoothData = 0x02;//����һֱ����										
										    SEH_count--;
												if(SEH_count <= 4)
												{
														SEH_count = 4;	 
												}
											//SEH_count=4;
											count = 0;
											break;
	           
	         case 0x52:	  //�������ת
			 								ucBluetoothData = 0x02;//����һֱ����									
										 SEH_count++;
												if(SEH_count >= 25)
												{
														SEH_count = 25;  
												}
										 //SEH_count=25;
					           count = 0;
										 break;
	
	         case 0x4A:  //����� 
			 							ucBluetoothData = 0x02;//����һֱ����
										if(LED_L==1)
										{ 
											LED_L =0;
										}else
										{
										  LED_L =1;
									   	}
										 break;
	
			 case 0x4B:  //�ҵ��� 
			 							ucBluetoothData = 0x02;//����һֱ����									
										 if(LED_R==1)
										{ 
											LED_R = 0;
										}else
										{
										  LED_R =1;
									   	}
										break;
	
			  case 0x4E:  //ȫ�� 
			 							ucBluetoothData = 0x02;//����һֱ����								
										 if(LED_R==0&&LED_L==0)
										{ 
											LED_R = 1;
											LED_L = 1;
										}else
										{
										  LED_R =0;
										  LED_L =0;
									   	}
										break;
	
			  case 0x4C:  //�����˸ 
			 							ucBluetoothData = 0x02;//����һֱ����
										
										while(count1<=5)
										{ count1++;
										  LED_L =0;
										  delay(500);
										  LED_L =1;
										  delay(500);
										}
										  count1=0;									
										  LED_L =1;									
																				   
										 break;
			 case 0x4F: //ȫ��˸ 
			 							ucBluetoothData = 0x02;//����һֱ����
										
										while(count1<=5)
										{ count1++;
										  LED_R =0;
										  LED_L =0;
										 
										  delay(500);
										  LED_L =1;
										   LED_R =1;
										  delay(500);
										}
										  count1=0;									
										  LED_L =1;	
										   LED_R =1;								
																				   
										 break;
	
			  case 0x4D:  //�ҵ���˸ 
			 							ucBluetoothData = 0x02;//����һֱ����
										
										while(count2<=5)
										{ count2++;
										  LED_R =0;
										  delay(500);
										  LED_R =1;
										  delay(500);
										}
										  count2=0;									
										  LED_R =1;																												   
										 break;
	
			 case 0x41:  //ǰ��												
	                                ucBluetoothData = 0x02;//����һֱ����
	                                go_forward();                                
	                                break;
	          case 0x42  : //��ת					
	                            ucBluetoothData = 0x02;//����һֱ����
	                                turn_left();                               
	                                break;
	          case 0x44: //��ת					
	                                ucBluetoothData = 0x02;//����һֱ����
	                                turn_right();
	                                break;
	           case 0x45 : //����																
	                             ucBluetoothData = 0x02;//����һֱ����
	                                fall_back();                               
	                                break;
	           case 0x43 : 	 //ֹͣ
	                                ucBluetoothData = 0x02;//����һֱ����
	                                stop();                                       
	                                break;
	            default:	 
										break; 
    }               
}
void T0_time() interrupt 1
{
        TF0 = 0;//����жϱ�־
        TR0 = 0;//�ض�ʱ��
       
    //���1
    if(count <= SEH_count) //����ռ�ձ�����
    {
        //���count�ļ���С�ڣ�5-25��Ҳ����0.5ms-2.5ms�����Сt���ڳ����ߵ�ƽ����������
        SEH_PWM = 1;
    }
    else
    {
        SEH_PWM = 0;
    }
    //���2
    if(count <= SEV_count) //����ռ�ձ�����
    {
        SEV_PWM = 1;
    }
    else
    {
        SEV_PWM = 0;
    }
    count++;
    if (count >= 250) //T = 20ms��ʱ������������0
    {
        count = 0;
    }
         TH0 = 0xff;   //��װ��ֵ0.1ms
         TL0 = 0xa4;
         TR0 = 1;///����ʱ��

}

void initial_myself()
{
        TMOD = 0x01;//���ö�ʱ��0Ϊ������ʽ1
        TH0 = 0xff;
        TL0 = 0xa4;

        //���ô���
        SCON = 0x50;//���ڹ�����ʽ1:10λ��λ�շ���8λ���ݣ��������ʿɱ䣨�ɶ�ʱ��1������ʿ��ƣ�
        TMOD = 0x21;//���ö�ʱ��1Ϊ������ʽ2,8λ��ֵ�Զ���װ��8λ��ʱ�������ö�ʱ��0Ϊ������ʽ1
        TH1 = TL1 = 0xfd;
        IP = 0x10;
		SEH_PWM=0;
	    SEV_PWM=0;
		stop();				 
        PWM1 = 1;
        PWM2 = 1;

}

void initial_peripheral()
{
        EA = 1;//�����ж�
        ES = 1;//�������ж�
        ET0 = 1;//����ʱ���ж�
        TR0 = 1;//������ʱ��
        TR1 = 1;//
}
void receive(void) interrupt 4
{

        if(RI == 1)
        {
                RI = 0;
                ucBluetoothData = SBUF;
        }

	  
}

void delay_long(unsigned int uiDelayLong)
{
        unsigned int i;
        unsigned int j;
        for(i = 0 ; i < uiDelayLong ; i++)
        {
                for(j = 0; j < 500; j++);
        
        }

}
void delay(unsigned int xms)
{
    unsigned int i;
    unsigned int j;
	for(i =xms ; i > 0 ; i--)
      for(j = 110; j > 0; j--);
}

void stop()//ֹͣ
{
        
        motor_control_1 = 0;
        motor_control_2 = 0;
        motor_control_3 = 0;
        motor_control_4 = 0;
}
void fall_back()
{
       
        motor_control_1 = 0;
        motor_control_2 = 1;
        motor_control_3 = 0;
        motor_control_4 = 1;
}
void go_forward()
{
        
        motor_control_1 = 1;
        motor_control_2 = 0;
        motor_control_3 = 1;
        motor_control_4 = 0;
}
void turn_left()//��ת
{
       
        motor_control_1 = 0;
        motor_control_2 = 0;
        motor_control_3 = 1;
        motor_control_4 = 0;
        
}
void turn_right()//��ת
{
       
        motor_control_1 = 1;
        motor_control_2 = 0;
        motor_control_3 = 0;
        motor_control_4 = 0;
}