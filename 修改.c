#include "reg51.h"

sbit LED_L = P3^2;
sbit LED_R = P3^3;
sbit PWM1 = P2^0;//左轮电机驱动使能
sbit PWM2 = P2^1;//右轮电机驱动使能
sbit motor_control_1 = P0^0;//左轮前进
sbit motor_control_2 = P0^1;//左轮后退
sbit motor_control_3 = P0^2;//右轮前进
sbit motor_control_4 = P0^3;//右轮后退
sbit SEV_PWM = P0^7;  //上下舵机信号位
sbit SEH_PWM = P0^6;  //左右舵机信号位

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
void go_forward(void);//前进
void fall_back(void);//后退
void turn_left(void);//左转
void turn_right(void);//右转
void stop();//刹车

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
//串口服务函数
void usart_service()
{
                
                switch(ucBluetoothData)
                {
	         case 0xFF:	 //舵机复位			
			 								ucBluetoothData = 0x02;//避免一直触发
											SEH_count = 11;		  //保持正常姿态
											SEV_count = 3;		  
											count = 0;
											break;
					 
	         case 0x50:  //舵机向上转
			 							ucBluetoothData = 0x02;//避免一直触发							
										 SEV_count--;
											if(SEV_count <= 3)
											{
												SEV_count = 3;	
											}
										 //SEV_count = 3;	
										 count = 0;
										 break;  
	         case 0x53:	 //舵机向下转		
			 								ucBluetoothData = 0x02;//避免一直触发	
			 								count = 0;									
					            			SEV_count++;
											if(SEV_count >= 9)
											{
													SEV_count = 9;	  
											}
											//SEV_count=9;
										
											break;
	           
	         case 0x51:	  //舵机向左转
			 								ucBluetoothData = 0x02;//避免一直触发										
										    SEH_count--;
												if(SEH_count <= 4)
												{
														SEH_count = 4;	 
												}
											//SEH_count=4;
											count = 0;
											break;
	           
	         case 0x52:	  //舵机向右转
			 								ucBluetoothData = 0x02;//避免一直触发									
										 SEH_count++;
												if(SEH_count >= 25)
												{
														SEH_count = 25;  
												}
										 //SEH_count=25;
					           count = 0;
										 break;
	
	         case 0x4A:  //左灯亮 
			 							ucBluetoothData = 0x02;//避免一直触发
										if(LED_L==1)
										{ 
											LED_L =0;
										}else
										{
										  LED_L =1;
									   	}
										 break;
	
			 case 0x4B:  //右灯亮 
			 							ucBluetoothData = 0x02;//避免一直触发									
										 if(LED_R==1)
										{ 
											LED_R = 0;
										}else
										{
										  LED_R =1;
									   	}
										break;
	
			  case 0x4E:  //全亮 
			 							ucBluetoothData = 0x02;//避免一直触发								
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
	
			  case 0x4C:  //左灯闪烁 
			 							ucBluetoothData = 0x02;//避免一直触发
										
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
			 case 0x4F: //全闪烁 
			 							ucBluetoothData = 0x02;//避免一直触发
										
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
	
			  case 0x4D:  //右灯闪烁 
			 							ucBluetoothData = 0x02;//避免一直触发
										
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
	
			 case 0x41:  //前进												
	                                ucBluetoothData = 0x02;//避免一直触发
	                                go_forward();                                
	                                break;
	          case 0x42  : //左转					
	                            ucBluetoothData = 0x02;//避免一直触发
	                                turn_left();                               
	                                break;
	          case 0x44: //右转					
	                                ucBluetoothData = 0x02;//避免一直触发
	                                turn_right();
	                                break;
	           case 0x45 : //后退																
	                             ucBluetoothData = 0x02;//避免一直触发
	                                fall_back();                               
	                                break;
	           case 0x43 : 	 //停止
	                                ucBluetoothData = 0x02;//避免一直触发
	                                stop();                                       
	                                break;
	            default:	 
										break; 
    }               
}
void T0_time() interrupt 1
{
        TF0 = 0;//清除中断标志
        TR0 = 0;//关定时器
       
    //舵机1
    if(count <= SEH_count) //控制占空比左右
    {
        //如果count的计数小于（5-25）也就是0.5ms-2.5ms则这段小t周期持续高电平。产生方波
        SEH_PWM = 1;
    }
    else
    {
        SEH_PWM = 0;
    }
    //舵机2
    if(count <= SEV_count) //控制占空比上下
    {
        SEV_PWM = 1;
    }
    else
    {
        SEV_PWM = 0;
    }
    count++;
    if (count >= 250) //T = 20ms则定时器计数变量清0
    {
        count = 0;
    }
         TH0 = 0xff;   //重装初值0.1ms
         TL0 = 0xa4;
         TR0 = 1;///开定时器

}

void initial_myself()
{
        TMOD = 0x01;//设置定时器0为工作方式1
        TH0 = 0xff;
        TL0 = 0xa4;

        //配置串口
        SCON = 0x50;//串口工作方式1:10位异位收发（8位数据），波特率可变（由定时器1的溢出率控制）
        TMOD = 0x21;//设置定时器1为工作方式2,8位初值自动重装的8位定时器，设置定时器0为工作方式1
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
        EA = 1;//开总中断
        ES = 1;//允许串口中断
        ET0 = 1;//允许定时器中断
        TR0 = 1;//启动定时器
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

void stop()//停止
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
void turn_left()//左转
{
       
        motor_control_1 = 0;
        motor_control_2 = 0;
        motor_control_3 = 1;
        motor_control_4 = 0;
        
}
void turn_right()//右转
{
       
        motor_control_1 = 1;
        motor_control_2 = 0;
        motor_control_3 = 0;
        motor_control_4 = 0;
}