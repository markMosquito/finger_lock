//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wmissing-field-initializers"
//#pragma clang diagnostic ignored "-Wmissing-braces"
//#pragma clang diagnostic ignored "-Wpointer-sign"

#include "display_task.h"
#include "Com2Server.h"
#include "oled.h"
#include "finger.h"
#include "mpr121.h"
#include "Rsing_I2C.h"
#include "TSM12.h"
#include "flash_task.h"
#include <string.h>


extern RTC_TimeTypeDef   RTC_TimeStructure;

#define ZERO 3
#define ONE 11
#define TWO 10
#define THREE 9
#define FOUR 2
#define FIVE 6
#define SIX 8
#define SEVEN 7
#define EIGHT 1
#define NINE 4
#define STAR 5
#define POUND 0

#define PAGE_UP_KEY      2
#define PAGE_DOWN_KEY    9
#define PAGE_CONFIRM_KEY    1 
#define CANCEL_KEY       11

u32 test = 512;
u8 hours = 13;
u8 minutes = 50;
u8 seconds = 15;


u8 identify_run_flag = 0;
u8 menu_select = 0;
u8 time_show_refresh = 1;

u8 passportInput[8];
void TimeShow(void)
{
	OLED_Clear();
	OLED_ShowString_CH(0,0, "ʱ��:",1);
	//OLED_ShowString(32,0, ": ",1);
	OLED_ShowNum(50,0, RTC_TimeStructure.RTC_Hours, 2,16,1);
	OLED_ShowString(68,0, ": ",1);
	OLED_ShowNum(77,0, RTC_TimeStructure.RTC_Minutes, 2,16,1);
	OLED_ShowString(95,0, ": ",1);
	OLED_ShowNum(104,0, RTC_TimeStructure.RTC_Seconds, 2,16,1);
	
	if(registered_flag)//��ע��
	{
		OLED_ShowString_CH(32,24, "��ӭ�ؼ�",1);
	}
	else
	{
		OLED_ShowString_CH(0,17, "��1ע�ᡣ",1);
	}
	
	//OLED_Refresh_Gram();
}
u32 test32 = 0;
u8 checkInterrupt(void)
{
  if(READ_I2C_IRQ)
    return 1;
 
  return 0;
}
u8 data1,data2,data3;
u8 getKeyNumber(void)
{
	
	int j = 0;
    u32 touchNumber;
    u32 touchstatus_pre;
	u16 touchstatus = 0;
	
	vTaskDelay( 100* portTICK_MS);
	while(checkInterrupt())
	{
		vTaskDelay( 1* portTICK_MS);
	}
//	data1 = TSM_ReadOneByte(OUTPUT3);
//	data2 = TSM_ReadOneByte(OUTPUT2);
//	data3 = TSM_ReadOneByte(OUTPUT1);
	touchstatus_pre = TSM_ReadOneByte(OUTPUT3) << 16;
    touchstatus_pre |= TSM_ReadOneByte(OUTPUT2)<<8;
	touchstatus_pre |= TSM_ReadOneByte(OUTPUT1);
	
	for (j=0; j<12; j++)  // Check how many electrodes were pressed
    {
		if((touchstatus_pre&0x03) == 0x03)
		{
			touchNumber++;
			touchstatus = touchstatus|(0x01<<j);
		}
		
		touchstatus_pre = touchstatus_pre>>2;
    }
	test32 = touchstatus;
	if (touchNumber == 1)
    {
      if (touchstatus & (1<<STAR))//�Ǻų���
        return 10;
      else if (touchstatus & (1<<SEVEN))
        return 7;
      else if (touchstatus & (1<<FOUR))
        return 4;
      else if (touchstatus & (1<<ONE))
        return 1;
      else if (touchstatus & (1<<ZERO))
        return 0;
      else if (touchstatus & (1<<EIGHT))
        return 8;
      else if (touchstatus & (1<<FIVE))
        return 5;
      else if (touchstatus & (1<<TWO))
        return 2;
      else if (touchstatus & (1<<POUND))//����ȡ���������
        return 11;
      else if (touchstatus & (1<<NINE))
        return 9;
      else if (touchstatus & (1<<SIX))
        return 6;
      else if (touchstatus & (1<<THREE))
        return 3;
    }
	return -1;
}
u8 getStarAndPound(void)
{	
	int j = 0;
    u32 touchNumber = 0;
    u32 touchstatus_pre;
	u16 touchstatus = 0;
	
	vTaskDelay( 20* portTICK_MS);
	while(checkInterrupt())
	{
		vTaskDelay( 1* portTICK_MS);
	}
	
	touchstatus_pre = TSM_ReadOneByte(OUTPUT3) << 16;
    touchstatus_pre |= TSM_ReadOneByte(OUTPUT2)<<8;
	touchstatus_pre |= TSM_ReadOneByte(OUTPUT1);
	
	for (j=0; j<12; j++)  // Check how many electrodes were pressed
    {
		if((touchstatus_pre&0x03) == 0x03)
		{
			touchNumber++;
			touchstatus = touchstatus|(0x01<<j);
		}
		
		touchstatus_pre = touchstatus_pre>>2;
    }
	test32 = touchstatus;
	if (touchNumber == 2)
    {
		if ((touchstatus & (1<<STAR)) && (touchstatus & (1<<ZERO)))//�Ǻų���
			return 1;
    }
	return -1;
}
extern int cur_id_detected;
extern u8 manager_id_true;
u8 getPassportNumberOrFinger(void)
{
	
	
    int i = 0;
	
	int j = 0;
    u32 touchNumber;
    u32 touchstatus_pre;
	u16 touchstatus = 0;
	
  while(i<8)
  {
	  
	  vTaskDelay( 100* portTICK_MS);
	while(checkInterrupt())
	{
		if(cur_id_detected == member_id[0])
			return 3;//��ʾָ�Ƶ�
		vTaskDelay( 1* portTICK_MS);
	}
	touchstatus_pre = TSM_ReadOneByte(OUTPUT3) << 16;
    touchstatus_pre |= TSM_ReadOneByte(OUTPUT2)<<8;
	touchstatus_pre |= TSM_ReadOneByte(OUTPUT1);
	
	touchNumber = 0;
	touchstatus = 0;
	
	for (j=0; j<12; j++)  // Check how many electrodes were pressed
    {
		if((touchstatus_pre&0x03) == 0x03)
		{
			touchNumber++;
			touchstatus = touchstatus|(0x01<<j);
		}
		
		touchstatus_pre = touchstatus_pre>>2;
    }
    //������Ϣ�Ѵ洢��
    if (touchNumber == 1)
    {
      if (touchstatus & (1<<STAR))//�Ǻų���
	  {
		  passportInput[i] = 10;
		 
	  }
        
      else if (touchstatus & (1<<SEVEN))
	  {
		  
		  passportInput[i] = '7'-'0';
		 
	  }
        
      else if (touchstatus & (1<<FOUR))
	  {
		  
		  passportInput[i] = '4'-'0';
	  }
        
      else if (touchstatus & (1<<ONE))
	  {
		 
		  passportInput[i] = '1'-'0';
		   
	  }
      else if (touchstatus & (1<<ZERO))
	  {
		 
		  passportInput[i] = '0'-'0';
		   
	  }
      else if (touchstatus & (1<<EIGHT))
      {
		  
		  passportInput[i] = '8'-'0';
		  
	  }
      else if (touchstatus & (1<<FIVE))
      {
		  
		  passportInput[i] = '5'-'0';
		  
	  }
      else if (touchstatus & (1<<TWO))
      {
		 
		  passportInput[i] = '2'-'0';
		 
	  }
      else if (touchstatus & (1<<POUND))//����ȡ��������� 
	  {
		 
		  passportInput[i] = 11;
		  
	  }
      else if (touchstatus & (1<<NINE))
      {
		  
		  passportInput[i] = '9'-'0';
		 
	  }
      else if (touchstatus & (1<<SIX))
      {
		 
		  passportInput[i] = '6'-'0';
		  
	  }
      else if (touchstatus & (1<<THREE))
      {
		 
		  passportInput[i] = '3'-'0';
		   
	  }
        
		
	  if (passportInput[i] == CANCEL_KEY)
	  {
		  return 2;
	  }
	  else if(passportInput[i] == 10)
	  {
		  i = i - 1;
		  if(i <= 0)
			  i = 0;
		  OLED_Clear();
		OLED_ShowString_CH(0, 0, "����Ա��Ϣȷ��",1);
		  OLED_ShowAst(32,40,i);
	  }
	  else
	  {
		  i++;
		  OLED_Clear();
		OLED_ShowString_CH(0, 0, "����Ա��Ϣȷ��",1);
		  OLED_ShowAst(32,40,i);
	  }
	  
    }
	  
    else if (touchNumber == 0)
      ;
    //else
      //Serial.println("Only touch ONE button!");
  }
  return 1;
}
//����1�������������ȡ�ɹ�������2����ȡ������
u8 getPassportNumber()
{
    int i = 0;
    int j = 0;
    u32 touchNumber;
    u32 touchstatus_pre;
	u16 touchstatus = 0;
  
  while(i<8)
  {
	  vTaskDelay( 100* portTICK_MS);
    while(checkInterrupt())
	{
		vTaskDelay( 1* portTICK_MS);
	}
	touchstatus_pre = TSM_ReadOneByte(OUTPUT3) << 16;
    touchstatus_pre |= TSM_ReadOneByte(OUTPUT2)<<8;
	touchstatus_pre |= TSM_ReadOneByte(OUTPUT1);
	
	touchNumber = 0;
	touchstatus = 0;
	for (j=0; j<12; j++)  // Check how many electrodes were pressed
    {
		if((touchstatus_pre&0x03) == 0x03)
		{
			touchNumber++;
			touchstatus = touchstatus|(0x01<<j);
		}
		touchstatus_pre = touchstatus_pre>>2;
    }
    
    if (touchNumber == 1)
    {
            if (touchstatus & (1<<STAR))//�Ǻų���
	  {
		  passportInput[i] = 10;
		 
	  }
        
      else if (touchstatus & (1<<SEVEN))
	  {
		  
		  passportInput[i] = '7'-'0';
		 
	  }
        
      else if (touchstatus & (1<<FOUR))
	  {
		  
		  passportInput[i] = '4'-'0';
	  }
        
      else if (touchstatus & (1<<ONE))
	  {
		 
		  passportInput[i] = '1'-'0';
		   
	  }
      else if (touchstatus & (1<<ZERO))
	  {
		 
		  passportInput[i] = '0'-'0';
		   
	  }
      else if (touchstatus & (1<<EIGHT))
      {
		  
		  passportInput[i] = '8'-'0';
		  
	  }
      else if (touchstatus & (1<<FIVE))
      {
		  
		  passportInput[i] = '5'-'0';
		  
	  }
      else if (touchstatus & (1<<TWO))
      {
		 
		  passportInput[i] = '2'-'0';
		 
	  }
      else if (touchstatus & (1<<POUND))//����ȡ��������� 
	  {
		 
		  passportInput[i] = 11;
		  
	  }
      else if (touchstatus & (1<<NINE))
      {
		  
		  passportInput[i] = '9'-'0';
		 
	  }
      else if (touchstatus & (1<<SIX))
      {
		 
		  passportInput[i] = '6'-'0';
		  
	  }
      else if (touchstatus & (1<<THREE))
      {
		 
		  passportInput[i] = '3'-'0';
		   
	  }
	  
        
		
	  if (passportInput[i] == CANCEL_KEY)
	  {
		  return 2;
	  }
	  else if(passportInput[i] == 10)
	  {
		  i = i - 1;
		  if(i <= 0)
			  i = 0;
		  OLED_ShowAst(32,40,i);
	  }
	  else
	  {
		  i++;
		  OLED_ShowAst(32,40,i);
	  }
	  
    }
    else if (touchNumber == 0)
      ;
    //else
      //Serial.println("Only touch ONE button!");
  }
  return 1;
  
}

#define DIGITS  8
extern union Password password;
u8 passport_wrong_count = 0;
u8 passport_right_flag = 0;

u8 passport_set_first_time[8];
u8 passport_set_second_time[8];
u8 passport_set_step = 0;
u8 passport_set_success_flag = 0;


u8 finger_info_first_time_success_flag = 0;
u8 finger_info_second_time_success_flag = 0;
u8 finger_info_third_time_success_flag = 0;

u8 key_number = 0;
int cur_model_id = 0;
u8 passwordCmp(u8 * pass1, u8 * pass2)
{
	int i = 0;
	for(i = 0; i< DIGITS; i++)
	{
		if(pass1[i] != pass2[i] )
		{
			return 0;
		}
		
	}
	return 1;
}
void passwordCpy(unsigned char * dest,unsigned char * src)
{
	int i = 0;
	for(i = 0; i< DIGITS; i++)
	{
		dest[i] = src[i];	
	}
}
int returnTodefault(void)
{
	registered_flag = 0;
	member_number = 0;
	password.ch[0] =1;
	password.ch[1] =2;
	password.ch[2] =3;
	password.ch[3] =4;
	password.ch[4] =5;
	password.ch[5] =6;
	password.ch[6] =7;
	password.ch[7] =8;
	writeRegInfoToFlash();
	
	return 1;
	//return delete_all_model();
}
u8 RegisterShow(void)
{
	u8 status_get_passport = 0;
	int i = 0;
	
	passport_right_flag = 0;
	passport_set_success_flag = 0;
	passport_set_step = 0;
	readRegInfoFromFlash();
	//ɾ��ģ��
	if(delete_all_model() != 1)
	{
		OLED_Clear();	
		OLED_ShowString_CH(0,0, "����ģ��ɾ��ʧ��",0);
		vTaskDelay( 2000 * portTICK_MS);
		return 3;//ɾ��ʧ��
	}
		
	
	OLED_Clear();	
	OLED_ShowString_CH(0,0, "�����豸��ʼ����",0);
	
	OLED_ShowAst(32,40,0);
	
	//OLED_Refresh_Gram();
	while(passport_right_flag == 0)
	{
		status_get_passport = getPassportNumber();
		switch(status_get_passport)
		{
			case 2: return 2;
			case 1: 
				
				if(passwordCmp(passportInput,password.ch) == 1 )
				{
					passport_right_flag = 1;//������ȷ������whileѭ��
				} 
				else
				{
					passport_wrong_count++;
					OLED_ShowString_CH(0,0, "�������!",0);
					OLED_ShowAst(32,40,i);
					if(passport_wrong_count>= 3)
					{
						Buzzer(5, 150,150);//����
					}
				}
			break;
			default:break;
		}
	}
	passport_right_flag = 0;
	//���Ϲ�����������ɹ���
	
	//���������û�����
	OLED_Clear();	
	OLED_ShowString_CH(0,0, "������������:",1);
	//OLED_ShowString(0,16, "",0);
	OLED_ShowAst(32,40,0);
	
	while(passport_set_success_flag == 0)
	{
		status_get_passport = getPassportNumber();
		switch(status_get_passport)
		{
			case 2: return 2;
			case 1: 
				if(passport_set_step == 0)
				{
					passwordCpy(passport_set_first_time, passportInput);
					passport_set_step = 1;
					
					OLED_Clear();	
					OLED_ShowString_CH(0,0, "ȷ������",1);
					//OLED_ShowString(0,16, "Second time ",0);
					OLED_ShowAst(32,40,0);
				}
				else if(passport_set_step == 1)
				{
					passwordCpy(passport_set_second_time, passportInput);
					if(passwordCmp(passport_set_first_time,passport_set_second_time) == 1)
					{
						passport_set_success_flag = 1;
						
						passwordCpy(password.ch,passport_set_first_time);
						OLED_Clear();	
						OLED_ShowString_CH(0,0, "�������óɹ�!",1);
						OLED_ShowString_CH(0,16, "��ʼ¼��ָ���� ",0);
						vTaskDelay( 1000 * portTICK_MS);
						
					}
					else
					{
						passport_set_step = 0;
						OLED_Clear();	
						OLED_ShowString_CH(0,0, "�������벻һ�� ",1);
						OLED_ShowString_CH(0,16, "����������",0);
						vTaskDelay( 1700 * portTICK_MS);
						OLED_Clear();
						OLED_ShowString_CH(0,0, "������������:",1);
						OLED_ShowAst(32,40,0);
					}
				}
				
			break;
			default:break;
		}
	}
	//���������û��������
	
	//����¼��ҳ�ָ����
	OLED_Clear();	
	OLED_ShowString_CH(0,0, "�������óɹ�!",1);
	OLED_ShowString_CH(0,16, "��1����",0);
	
	key_number = getKeyNumber();
	while(key_number != PAGE_CONFIRM_KEY && key_number != CANCEL_KEY )
	{
		key_number = getKeyNumber();
	}
	if(key_number == PAGE_CONFIRM_KEY)
	{
		i++;
		OLED_Clear();
		OLED_ShowString_CH(0, 0, "��ʼ¼��ָ����",1);
		OLED_ShowString_CH(0, 16, "��һ��",1);
		OLED_ShowString_CH(0, 32, "�������ָ",0);
	}
	else if(key_number == CANCEL_KEY)
	{
		return 2;
	}
					
	for(i = 0;i< 3;)
	{
		switch(i)
		{
			case 0:
				if(modeling(i) == 1)
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "��һ���ɹ�",1);
					OLED_ShowString_CH(0,  32, "��1���� ",0);
					key_number = getKeyNumber();
					while(key_number != PAGE_CONFIRM_KEY && key_number != CANCEL_KEY )
					{
						key_number = getKeyNumber();
					}
					if(key_number == PAGE_CONFIRM_KEY)
					{
						i++;
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
						OLED_ShowString_CH(0, 16, "�ڶ���",1);
						OLED_ShowString_CH(0,  32, "�������ָ",0);
					}
					else if(key_number == CANCEL_KEY)
					{
						return 2;
					}
					
				}	
				else
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "��һ�����ʧ��",1);
					OLED_ShowString_CH(0,  32, "�����·���",0);
					vTaskDelay( 600 * portTICK_MS);
				}
				break;
			case 1:
				if(modeling(i) == 1)
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "�ڶ����ɹ�",1);
					OLED_ShowString_CH(0,  32, "��1���� ",0);
					key_number = getKeyNumber();
					while(key_number != PAGE_CONFIRM_KEY && key_number != CANCEL_KEY )
					{
						key_number = getKeyNumber();
					}
					if(key_number == PAGE_CONFIRM_KEY)
					{
						i++;
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
						OLED_ShowString_CH(0, 16, "������",1);
						OLED_ShowString_CH(0,  32, "�������ָ",0);
					}
					else if(key_number == CANCEL_KEY)
					{
						return 2;
					}
				}	
				else
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "�ڶ������ʧ��",1);
					OLED_ShowString_CH(0,  32, "�����·���",0);
					vTaskDelay( 600 * portTICK_MS);
				}
				break;
				
			case 2:
				if(modeling(i) == 1)
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "��������ɣ�",1);
					//OLED_ShowString_CH(0,  32, "�����·���",0);
					cur_model_id = modeling_complete();
					if(cur_model_id>=0)
					{
						registered_flag = 1;
						member_number = 1;
						member_id[0] = cur_model_id;
						if(writeRegInfoToFlash() == 1)
						{
							OLED_Clear();
							OLED_ShowString_CH(0,  0, "ע��ɹ���",1);
							OLED_ShowString_CH(0,  16, "IDΪ:",0);
							OLED_ShowNum(50,16, member_id[0], 2,16,0);
							i++;
							vTaskDelay( 2000 * portTICK_MS);
							return 1;
						}
						else
						{
							OLED_Clear();
							OLED_ShowString_CH(0, 0, "¼��ָ����",1);
							OLED_ShowString_CH(0, 16, "д����Ϣʧ��",1);
							OLED_ShowString_CH(0,  32, "������",0);
							vTaskDelay( 1500 * portTICK_MS);
						}
						
					}
					else
					{
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "¼��ָ����",1);
						OLED_ShowString_CH(0, 16, "��ģʧ��",1);
						OLED_ShowString_CH(0,  32, "������",0);
						vTaskDelay( 1500 * portTICK_MS);
						i = 0;
					}
					
				}	
				else
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "���������ʧ��",1);
					OLED_ShowString_CH(0,  32, "�����·���",0);
					vTaskDelay( 600 * portTICK_MS);
				}
			default:break;
		}
		
	}
	
	return 1;
}



//����Ա��Ϣȷ��
u8 manager_info_identify(void)
{
	u8 status_get_passport = 0;
	
	
	passport_right_flag = 0;
	
	OLED_Clear();
	OLED_ShowString_CH(0, 0, "����Ա���ȷ��",1);
	OLED_ShowString_CH(0, 16,"������������ָ����",0);
	
	identify_run_flag = 1;
	cur_id_detected = -1;
	while(passport_right_flag == 0)
	{
		status_get_passport = getPassportNumberOrFinger();
		switch(status_get_passport)
		{
			case 2:  return 2;
			case 1: 
				
				if(passwordCmp(passportInput,password.ch) == 1 )
				{
					passport_right_flag = 1;//������ȷ������whileѭ��
				} 
				else
				{
					
					passport_wrong_count++;
					OLED_ShowString_CH(0,0, "�������",0);
					vTaskDelay( 1500 * portTICK_MS);
					if(passport_wrong_count>= 3)
					{
						passport_wrong_count = 0;
						passport_right_flag = 0;
						Buzzer(5, 150,150);
						return 0;//�������������̫��
					}
				}
			break;
			case 3: passport_right_flag = 1; break;//ָ����ȷ
			default:break;
		}
	}
	identify_run_flag = 0;
	cur_id_detected = -1;
	Buzzer(1, 250,250);
	return 1;
}

int member_finger_info_add(void)
{
	u8 i = 0;
	OLED_Clear();	
	OLED_ShowString_CH(0,0, "�����³�Ա",1);
	
	//OLED_ShowString(0, 16, "first time",1);	
	OLED_ShowString_CH(0,  16, "��1����",0);
	key_number = getKeyNumber();
	while(key_number != CANCEL_KEY && key_number != PAGE_CONFIRM_KEY )
	{
		key_number = getKeyNumber();
	}
	if(key_number == PAGE_CONFIRM_KEY)
	{
		i++;
		OLED_Clear();
		OLED_ShowString_CH(0, 0, "��ʼ¼��ָ����",1);
		OLED_ShowString_CH(0, 16, "��һ��",1);
		OLED_ShowString_CH(0, 32, "�������ָ",0);
	}
	else if(key_number == CANCEL_KEY)
	{
		return 2;
	}
	for(i = 0;i< 3;)
	{
		switch(i)
		{
			case 0:
				if(modeling(i) == 1)
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "��һ���ɹ�",1);
					OLED_ShowString_CH(0,  32, "��1���� ",0);
					key_number = getKeyNumber();
					while(key_number != PAGE_CONFIRM_KEY && key_number != CANCEL_KEY )
					{
						key_number = getKeyNumber();
					}
					if(key_number == PAGE_CONFIRM_KEY)
					{
						i++;
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
						OLED_ShowString_CH(0, 16, "�ڶ���",1);
						OLED_ShowString_CH(0,  32, "�������ָ",0);
					}
					else if(key_number == CANCEL_KEY)
					{
						return 2;
					}
					
				}	
				else
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "��һ�����ʧ��",1);
					OLED_ShowString_CH(0,  32, "�����·���",0);
					
					vTaskDelay( 1000 * portTICK_MS);
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "��һ��",1);
					OLED_ShowString_CH(0, 32, "�������ָ",0);
				}
				break;
			case 1:
				if(modeling(i) == 1)
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "�ڶ����ɹ�",1);
					OLED_ShowString_CH(0,  32, "��1���� ",0);
					key_number = getKeyNumber();
					while(key_number != PAGE_CONFIRM_KEY && key_number != CANCEL_KEY )
					{
						key_number = getKeyNumber();
					}
					if(key_number == PAGE_CONFIRM_KEY)
					{
						i++;
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
						OLED_ShowString_CH(0, 16, "������",1);
						OLED_ShowString_CH(0,  32, "�������ָ",0);
					}
					else if(key_number == CANCEL_KEY)
					{
						return 2;
					}
				}	
				else
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "�ڶ������ʧ��",1);
					OLED_ShowString_CH(0,  32, "�����·���",0);
					vTaskDelay( 1000 * portTICK_MS);
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "�ڶ���",1);
					OLED_ShowString_CH(0, 32, "�������ָ",0);
				
				}
				break;
				
			case 2:
				if(modeling(i) == 1)
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "��������ɣ�",1);
					//OLED_ShowString_CH(0,  32, "�����·���",0);
					cur_model_id = modeling_complete();
					if(cur_model_id>0)
					{
						registered_flag = 1;
						member_number++;
						member_id[member_number-1] = cur_model_id;
						if(writeRegInfoToFlash() == 1)
						{
							OLED_Clear();
							OLED_ShowString_CH(0,  0, "ע��ɹ���",1);
							OLED_ShowString_CH(0,  16, "IDΪ:",0);
							OLED_ShowNum(50,16, member_id[member_number-1], 2,16,0);
							OLED_ShowString_CH(0,  32, "��ͥ�ܳ�Ա:",1);
							OLED_ShowNum(110,32, member_number, 2,16,1);
							i++;
							vTaskDelay( 2000 * portTICK_MS);
							return 1;
						}
						else
						{
							OLED_Clear();
							OLED_ShowString_CH(0, 0, "¼��ָ����",1);
							OLED_ShowString_CH(0, 16, "д����Ϣʧ��",1);
							OLED_ShowString_CH(0,  32, "������",0);
							vTaskDelay( 1500 * portTICK_MS);
						}
						
					}
					else
					{
						if(cur_model_id == 0)
							delete_model(cur_model_id);
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "¼��ָ����",1);
						OLED_ShowString_CH(0, 16, "��ģʧ��",1);
						OLED_ShowString_CH(0,  32, "������",0);
						vTaskDelay( 1500 * portTICK_MS);
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "��ʼ����ָ����",1);
						OLED_ShowString_CH(0, 16, "��һ��",1);
						OLED_ShowString_CH(0, 32, "�������ָ",0);
						i = 0;
					}
					
				}	
				else
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "¼��ָ���� ",1);
					OLED_ShowString_CH(0, 16, "���������ʧ��",1);
					OLED_ShowString_CH(0,  32, "�����·���",0);
					
					vTaskDelay( 1000 * portTICK_MS);
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "��ʼ����ָ����",1);
					OLED_ShowString_CH(0, 16, "��һ��",0);
					OLED_ShowString_CH(0, 32, "�������ָ",0);
					
				}
			default:break;
		}
		
	}
}

void list_one_member(u8 pos, int id, u8 selected_flag)
{
	OLED_ShowString(0, (pos-1) * 16, "ID: ", selected_flag);
	OLED_ShowNum(30, (pos-1) * 16,id, 6,16,selected_flag);
}

int min(int x,int y)
{
	if(x<= y)
		return x;
	else
		return y;
}


int delete_from_flash_and_dev(u8 index)
{
	int i = 0;
	if(delete_model(member_id[index]) == 1)
	{
		for(i = index;i<member_number-1;i++)
		{
			member_id[i] = member_id[i+1];
		}
		member_number--;
		
		writeRegInfoToFlash();
		return 1;
	}
	else
	{
		return 0;
	}
	
}
int member_finger_info_delete(void)
{
	u8 menu_select_status = 1;
	u8 line = 0;
	
	u8 i = 0;

	OLED_Clear();	
	if(member_number == 1)
	{
		OLED_ShowString_CH(0, 0, "û�г�Ա��ɾ��", 0);
		vTaskDelay(2000*portTICK_MS);
		return 0;
	}
	OLED_ShowString_CH(0, 0, "��Ա��:", 1);
	OLED_ShowNum(64, 0,member_number-1, 2,16,1);
	
	line = menu_select_status % 3;//����������
	switch(line)
	{
		case 0: 
			OLED_ShowString_CH(0, 0, "��Ա��:", 1);
			OLED_ShowNum(64, 0,member_number-1, 2,16,1);
			list_one_member(4,member_id[menu_select_status],0);
			list_one_member(3,member_id[menu_select_status-1],1);
			list_one_member(2,member_id[menu_select_status-2],1);
		break;
		
		case 2:
			OLED_ShowString_CH(0, 0, "��Ա��:", 1);
			OLED_ShowNum(64, 0,member_number-1, 2,16,1);
			list_one_member(3,member_id[menu_select_status],0);
			list_one_member(2,member_id[menu_select_status-1],1);
			if(menu_select_status<member_number-1)
				list_one_member(4,member_id[menu_select_status+1],1);
			
		break;
			
		case 1:
			OLED_ShowString_CH(0, 0, "��Ա��:", 1);
			OLED_ShowNum(64, 0,member_number-1, 2,16,1);
			list_one_member(2,member_id[menu_select_status],0);
			if(menu_select_status<member_number-1)
				list_one_member(3,member_id[menu_select_status+1],1);
			if(menu_select_status+1<member_number-1)
				list_one_member(4,member_id[menu_select_status+2],1);
			
		break;
	}
		
		
	while(1)
	{
		key_number = getKeyNumber();	
		switch(key_number)
		{
			case PAGE_DOWN_KEY:
				
				if(menu_select_status >= member_number-1 )
					menu_select_status = member_number-1;
				else
				{
					menu_select_status++;
					OLED_Clear();
				};
				break;
			case PAGE_UP_KEY:
				
				if(menu_select_status <= 1)
					menu_select_status = 1;
				else
				{
					menu_select_status--;
					OLED_Clear();
				};
				break;
			case PAGE_CONFIRM_KEY:
				if(delete_from_flash_and_dev(menu_select_status))
				{
					OLED_Clear();
					if(member_number == 1)
					{
						OLED_ShowString_CH(0,0,"ɾ���ɹ�",0);
						vTaskDelay(2000*portTICK_MS);
						OLED_ShowString_CH(0, 0, "û�г�Ա��ɾ��", 0);
						vTaskDelay(2000*portTICK_MS);
						return 1;
					}
					else
					{
						OLED_ShowString_CH(0, 0, "��Ա��:", 1);
						OLED_ShowNum(64, 0,member_number-1, 2,16,1);
						OLED_ShowString_CH(30,50,"ɾ���ɹ�",0);
						vTaskDelay(2000*portTICK_MS);
						return 1;
					}
				}
				else
				{
					
				}
						
				break;//����˵�
			case CANCEL_KEY:return 2;
			default: break;
		}
		line = menu_select_status % 3;//����������
		switch(line)
		{
			case 0: 
				OLED_ShowString_CH(0, 0, "��Ա��:", 1);
				OLED_ShowNum(64, 0,member_number-1, 2,16,1);
				list_one_member(4,member_id[menu_select_status],0);
				list_one_member(3,member_id[menu_select_status-1],1);
				list_one_member(2,member_id[menu_select_status-2],1);
			break;
			
			case 2:
				OLED_ShowString_CH(0, 0, "��Ա��:", 1);
				OLED_ShowNum(64, 0,member_number-1, 2,16,1);
				list_one_member(3,member_id[menu_select_status],0);
				list_one_member(2,member_id[menu_select_status-1],1);
				if(menu_select_status<member_number-1)
					list_one_member(4,member_id[menu_select_status+1],1);
				
			break;
				
			case 1:
				OLED_ShowString_CH(0, 0, "��Ա��:", 1);
				OLED_ShowNum(64, 0,member_number-1, 2,16,1);
				list_one_member(2,member_id[menu_select_status],0);
				if(menu_select_status<member_number-1)
					list_one_member(3,member_id[menu_select_status+1],1);
				if(menu_select_status+1<member_number-1)
					list_one_member(4,member_id[menu_select_status+2],1);
				
			break;
		}
	}
}


int modify_passport(void)
{
	u8 status_get_passport = 0;
	passport_set_success_flag = 0;
	
	if(manager_info_identify() != 1)
	{
		return 0;
	}
	passport_set_step = 0;
	OLED_Clear();	
	OLED_ShowString_CH(0,0, "������������ ",1);
	OLED_ShowString_CH(0,16, "����������",0);
	OLED_ShowAst(32,40,0);
	
	while(passport_set_success_flag == 0)
	{
		status_get_passport = getPassportNumber();
		switch(status_get_passport)
		{
			case 2: return 2;
			case 1: 
				if(passport_set_step == 0)
				{
					strcpy(passport_set_first_time, passportInput);
					passport_set_step = 1;
					
					OLED_Clear();	
					OLED_ShowString_CH(0,0, "������������ ",1);
					OLED_ShowString_CH(0,16, "ȷ������",0);
					OLED_ShowAst(32,40,0);
				}
				else if(passport_set_step == 1)
				{
					strcpy(passport_set_second_time, passportInput);
					if(passwordCmp(passport_set_first_time,passport_set_second_time) == 1)
					{
						passport_set_step = 0;
						passport_set_success_flag = 1;//����ѭ��
						
						passwordCpy(password.ch,passport_set_first_time);
						
						
						if(writeRegInfoToFlash() == 1)
						{
							OLED_Clear();	
							OLED_ShowString_CH(0,16, "��������ɹ� ",0);
							vTaskDelay( 1500 * portTICK_MS);
							return 1;
						}
						else
						{
							OLED_Clear();	
							OLED_ShowString_CH(0,16, "д��flashʧ��",0);
							vTaskDelay( 1500 * portTICK_MS);
							return 0;
						}
						
					}
					else
					{
						passport_set_step = 0;
						OLED_Clear();	
						OLED_ShowString_CH(0,0, "�������벻һ��",1);
						OLED_ShowString_CH(0,16, "����������",0);
						OLED_ShowAst(32,40,0);
					}
				}
				
			break;
			default:break;
		}
	}
}
int modify_finger_info(void)
{
	u8 i = 0;
	if(manager_info_identify() != 1)
	{
		return 0;
	}
	
	OLED_Clear();	
	OLED_ShowString_CH(0,0, "��������ָ����",1);
	//OLED_ShowString_CH(0, 16, "��һ��",1);	
	OLED_ShowString_CH(0,  16, "��1����",0);
	
	key_number = getKeyNumber();
	while(key_number != PAGE_CONFIRM_KEY && key_number != CANCEL_KEY )
	{
		key_number = getKeyNumber();
	}
	if(key_number == PAGE_CONFIRM_KEY)
	{
		i++;
		OLED_Clear();
		OLED_ShowString_CH(0, 0, "��ʼ����ָ����",1);
		OLED_ShowString_CH(0, 16, "��һ��",1);
		OLED_ShowString_CH(0, 32, "�������ָ",0);
	}
	else if(key_number == CANCEL_KEY)
	{
		return 2;
	}	
	for(i = 0;i< 3;)
	{
		switch(i)
		{
			case 0:
				if(modeling(i) == 1)
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
					OLED_ShowString_CH(0, 16, "��һ���ɹ�",1);
					OLED_ShowString_CH(0,  32, "��1���� ",0);
					key_number = getKeyNumber();
					while(key_number != PAGE_CONFIRM_KEY && key_number != CANCEL_KEY )
					{
						key_number = getKeyNumber();
					}
					if(key_number == PAGE_CONFIRM_KEY)
					{
						i++;
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
						OLED_ShowString_CH(0, 16, "�ڶ���",1);
						OLED_ShowString_CH(0,  32, "�������ָ",0);
					}
					else if(key_number == CANCEL_KEY)
					{
						return 2;
					}
					
				}	
				else
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
					OLED_ShowString_CH(0, 16, "��һ�����ʧ��",1);
					OLED_ShowString_CH(0,  32, "�����·���",0);
					vTaskDelay( 600 * portTICK_MS);
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
					OLED_ShowString_CH(0, 16, "��һ��",1);
					OLED_ShowString_CH(0, 32, "�������ָ",0);
					
				}
				break;
			case 1:
				if(modeling(i) == 1)
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
					OLED_ShowString_CH(0, 16, "�ڶ����ɹ�",1);
					OLED_ShowString_CH(0,  32, "��1���� ",0);
					key_number = getKeyNumber();
					while(key_number != PAGE_CONFIRM_KEY && key_number != CANCEL_KEY )
					{
						key_number = getKeyNumber();
					}
					if(key_number == PAGE_CONFIRM_KEY)
					{
						i++;
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
						OLED_ShowString_CH(0, 16, "������",1);
						OLED_ShowString_CH(0,  32, "�������ָ",0);
					}
					else if(key_number == CANCEL_KEY)
					{
						return 2;
					}
				}	
				else
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
					OLED_ShowString_CH(0, 16, "�ڶ������ʧ��",1);
					OLED_ShowString_CH(0,  32, "�����·���",0);
					vTaskDelay( 600 * portTICK_MS);
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
					OLED_ShowString_CH(0, 16, "�ڶ���",1);
					OLED_ShowString_CH(0, 32, "�������ָ",0);
				}
				break;
				
			case 2:
				if(modeling(i) == 1)
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
					OLED_ShowString_CH(0, 16, "��������ɣ�",1);
					//OLED_ShowString_CH(0,  32, "�����·���",0);
					cur_model_id = modeling_complete();
					if(cur_model_id>=0)
					{
						if(delete_model(member_id[0]) != 1)
						{
							OLED_Clear();	
							OLED_ShowString_CH(0,0, "�޸�ָ����ʧ��",1);
							OLED_ShowString_CH(0,16, "ԭָ������Ϣ����ʧ�ܣ�������",1);
							vTaskDelay( 2000 * portTICK_MS);
							return 0;
						}
						registered_flag = 1;
						member_number = 1;
						member_id[0] = cur_model_id;
						if(writeRegInfoToFlash() == 1)
						{
							OLED_Clear();
							OLED_ShowString_CH(0,  0, "�޸ĳɹ���",1);
							OLED_ShowString_CH(0,  16, "IDΪ:",0);
							OLED_ShowNum(50,16, member_id[0], 2,16,0);
							i++;
							vTaskDelay( 2000 * portTICK_MS);
							return 1;
						}
						else
						{
							OLED_Clear();
							OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
							OLED_ShowString_CH(0, 16, "д����Ϣʧ��",1);
							OLED_ShowString_CH(0,  32, "������",0);
							vTaskDelay( 1500 * portTICK_MS);
						}
						
					}
					else
					{
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
						OLED_ShowString_CH(0, 16, "��ģʧ��",1);
						OLED_ShowString_CH(0,  32, "������",0);
						vTaskDelay( 1500 * portTICK_MS);
						OLED_Clear();
						OLED_ShowString_CH(0, 0, "��ʼ����ָ����",1);
						OLED_ShowString_CH(0, 16, "������",1);
						OLED_ShowString_CH(0, 32, "�������ָ",0);
						i = 0;
					}
					
				}	
				else
				{
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
					OLED_ShowString_CH(0, 16, "���������ʧ��",1);
					OLED_ShowString_CH(0,  32, "�����·���",0);
					vTaskDelay( 600 * portTICK_MS);
					OLED_Clear();
					OLED_ShowString_CH(0, 0, "�޸�ָ����",1);
					OLED_ShowString_CH(0, 16, "��һ��",0);
					OLED_ShowString_CH(0, 32, "�������ָ",0);
					
				}
			default:break;
		}
		
	}
}


int menu_modify_display(void)
{
	u8 menu_modify_select_status = 1;
	while(1)
	{
		switch(menu_modify_select_status)
		{
			case 1:
				OLED_ShowString_CH(0, 0, "����Ա��Ϣ�޸�",1);
				OLED_ShowString_CH(0, 16,"1.ָ������Ϣ",0);
				OLED_ShowString_CH(0, 32,"2.������Ϣ",1);
				
			break;
			case 2:
				OLED_ShowString_CH(0, 0, "����Ա��Ϣ�޸�",1);
				OLED_ShowString_CH(0, 16,"1.ָ������Ϣ",1);
				OLED_ShowString_CH(0, 32,"2.������Ϣ",0);
				
			break;
		
			default:break;
		}
		key_number = getKeyNumber();	
		switch(key_number)
		{
			case PAGE_DOWN_KEY:
				
				if(menu_modify_select_status >= 2)
					menu_modify_select_status = 2;
				else
				{
					menu_modify_select_status++;
					OLED_Clear();
				};
				break;
			case PAGE_UP_KEY:
				
				if(menu_modify_select_status <= 1)
					menu_modify_select_status = 1;
				else
				{
					menu_modify_select_status--;
					OLED_Clear();
				};
				break;
			case PAGE_CONFIRM_KEY:
				if(menu_modify_select_status == 1)
				{
					modify_finger_info();
					OLED_Clear();
				}
				if(menu_modify_select_status == 2)
				{
					modify_passport();
					OLED_Clear();
				}
				break;//����˵�
			case CANCEL_KEY:
				return 2;
			default: break;
		}
	}
}
u8 manager_info_modify(void)
{
	if(manager_info_identify())
	{
		OLED_Clear();
		menu_modify_display();
		return 1;
	}
	else
	{
		return 0;
	}
}
int member_info_add(void)
{
	if(manager_info_identify())
	{
		member_finger_info_add();
		return 1;
	}
	else
	{
		return 0;
	}
	
}
int member_info_delete_single(void)
{
	if(manager_info_identify())
	{
		member_finger_info_delete();
		return 1;
	}
	else
	{
		return 0;
	}	
}
int member_info_delete_all(void)
{
	int i = 0;
	if(manager_info_identify())
	{
		OLED_Clear();	
		OLED_ShowString_CH(0, 0, "ɾ����...", 0);
		if(member_number == 1)
		{
			OLED_Clear();
			OLED_ShowString(0, 0, "No member to delete", 0);
			vTaskDelay(2000*portTICK_MS);
			return 0;
		}
		else
		{
			for(i = 1; i< member_number;i++)
			{
				if(delete_model(member_id[i]) != 1)
				{
					OLED_Clear();
					OLED_ShowString(0, 0, "delete all failed", 0);
					vTaskDelay(2000*portTICK_MS);
					return -1;
				}
			}
			member_number = 1;
			writeRegInfoToFlash();
			OLED_Clear();
			OLED_ShowString(0, 0, "delete all success", 0);
			vTaskDelay(2000*portTICK_MS);
		}
		return 1;
	}
	else
	{
		return -1;
	}	
}
int member_info_delete(void)
{
	u8 member_delete_select_status = 1;
	while(1)
	{
		switch(member_delete_select_status)
		{
			case 1:
				OLED_Clear();
				OLED_ShowString_CH(0, 0, "ɾ����Ա",1);
				OLED_ShowString_CH(0, 16,"1.ɾ��������Ա ",0);
				OLED_ShowString_CH(0, 32,"2.ɾ�����г�Ա",1);
				
			break;
			case 2:
				OLED_Clear();
				OLED_ShowString_CH(0, 0, "ɾ����Ա",1);
				OLED_ShowString_CH(0, 16,"1.ɾ��������Ա ",1);
				OLED_ShowString_CH(0, 32,"2.ɾ�����г�Ա",0);
				
			break;
		
			default:break;
		}
		key_number = getKeyNumber();	
		switch(key_number)
		{
			case PAGE_DOWN_KEY:
				
				if(member_delete_select_status >= 2)
					member_delete_select_status = 2;
				else
				{
					member_delete_select_status++;
					OLED_Clear();
				};
				break;
			case PAGE_UP_KEY:
				
				if(member_delete_select_status <= 1)
					member_delete_select_status = 1;
				else
				{
					member_delete_select_status--;
					OLED_Clear();
				};
				break;
			case PAGE_CONFIRM_KEY:
				if(member_delete_select_status == 1)
				{
					member_info_delete_single();
					OLED_Clear();
				}
				if(member_delete_select_status == 2)
				{
					member_info_delete_all();
					OLED_Clear();
				}
				break;//����˵�
			case CANCEL_KEY:
				return 2;
			default: break;
		}
	}
}
u8 menu_display(void)
{
	u8 menu2_select_status;
	menu2_select_status = 1;//����Ա��Ϣ�޸�
	while(1)
	{
		switch(menu2_select_status)
		{
			case 1:
				OLED_ShowString_CH(0, 0, "����Ա�˵�:",1);
				OLED_ShowString_CH(0, 16,"1.����Ա��Ϣ�޸�",0);
				OLED_ShowString_CH(0, 32,"2.��ͥ��Ա���",1);
				OLED_ShowString_CH(0, 48,"3.��ͥ��Աɾ��",1);
			break;
			case 2:
				OLED_ShowString_CH(0, 0, "����Ա�˵�:",1);
				OLED_ShowString_CH(0, 16,"1.����Ա��Ϣ�޸�",1);
				OLED_ShowString_CH(0, 32,"2.��ͥ��Ա���",0);
				OLED_ShowString_CH(0, 48,"3.��ͥ��Աɾ��",1);
			break;
			case 3:
				OLED_ShowString_CH(0, 0, "����Ա�˵�:",1);
				OLED_ShowString_CH(0, 16,"1.����Ա��Ϣ�޸�",1);
				OLED_ShowString_CH(0, 32,"2.��ͥ��Ա���",1);
				OLED_ShowString_CH(0, 48,"3.��ͥ��Աɾ��",0);
			break;
			case 4:
				OLED_ShowString_CH(0, 0, "����Ա�˵�:",1);
				OLED_ShowString_CH(0, 16,"4.ʱ���޸�",0);
			break;
			default:break;
		}
		
		key_number = getKeyNumber();	
		switch(key_number)
		{
			case PAGE_DOWN_KEY:
				
				if(menu2_select_status >= 4)
					menu2_select_status = 4;
				else
				{
					menu2_select_status++;
					OLED_Clear();
				};
				break;
			case PAGE_UP_KEY:
				
				if(menu2_select_status <= 1)
					menu2_select_status = 1;
				else
				{
					menu2_select_status--;
					OLED_Clear();
				};
				break;
			case PAGE_CONFIRM_KEY:
				if(menu2_select_status == 1)
				{
					OLED_Clear();
					menu_modify_display();
				}
				else if(menu2_select_status == 2)
				{
					OLED_Clear();
					member_info_add();
				}
				else if(menu2_select_status == 3)
				{
					member_info_delete();
				}
				else if(menu2_select_status == 4)
				{
					
				}
				break;//����˵�
			case CANCEL_KEY : time_show_refresh = 1; menu_select = 0; return 1;
				return 0;
			default: break;
		}
	}
	return 1;
	
}
extern u8 member_deteted_flag;


void vUiTask(void *p_arg)
{
	(void)p_arg;
	vTaskDelay( 300 * portTICK_MS);
	
	
	//�ָ���������
	//returnTodefault();
		
	//��ȡflash��Ϣ
	readRegInfoFromFlash();
	
	//δע��Ļ���ע��
	if(registered_flag == 0)
	{
		while(RegisterShow() !=1)
		{
			vTaskDelay( 10 * portTICK_MS);
		}
	}
	
	while(1)
	{
		if(menu_select == 0)
		{
			identify_run_flag = 1;
			if(time_show_refresh == 1)
			{
				TimeShow();
				time_show_refresh = 0;
			}
			
		}
		else if(menu_select == 1)
		{
			identify_run_flag = 0;
			menu_display();
		}
		vTaskDelay( 10 * portTICK_MS);
	}
	
}
u8 data1,data2,data3,data4;
extern u8 forbid_time_refresh;
void vKeyTask( void *p_arg )
{
	
	u8 count = 0;
	
	
	I2C_Configuration();
	
	while(1)
	{	
//		while(1)
//		{
//			
//			data1 = TSM_ReadOneByte(0x02);
//			vTaskDelay( 5* portTICK_MS);
//		}
		//data4 = getKeyNumber();
		
	
//	data1 = TSM_ReadOneByte(OUTPUT1);
//		data2 = TSM_ReadOneByte(OUTPUT2);
//		data3 = TSM_ReadOneByte(OUTPUT3);
		//getStarAndPound();
		if(identify_run_flag == 1 && menu_select == 0 && forbid_time_refresh == 0)
		{
			if(getStarAndPound() == 1)
			{
				menu_select = 1;
				OLED_Clear();
			}
		}
		vTaskDelay( 10 * portTICK_MS);
	}
}
extern u8 OLED_GRAM_LINE_SIZE[8];
extern u8 OLED_GRAM_LINE_INDEX[8];
int data_test;
u8 PA2 = 0;
u8 iuy = 0;
void vDisplayTask( void *p_arg )
{
	
	u8 count = 0;
	
	
	OLED_Init();
	
	
	while(1)
	{
		//PA2 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2);
		//while(PA2 == 1);
//		if(iuy)
//		{
//			data_test = get_key();
//		}
		//data_test = get_key();
		
		vTaskDelay( 10 * portTICK_MS);
		count++;
		if(count == 10)
		{
			OLED_Refresh_Gram();
			count = 0;
		}
	}
}


//#pragma clang diagnostic pop
