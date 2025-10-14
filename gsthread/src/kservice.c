#include <gsthread.h>
#include <gshw.h>

char *gs_strncpy(char *des ,const char *src , gs_ubase_t n)
{ 
      if(n != 0)
      {
        char *d = des;
        const char *s = src;
        do
        {
           if( (*d++ = *s++)== 0) 
           {
             while(--n != 0)
           
                *d++ = 0;
                 break;
             
           }
            
        }while (--n != 0);            
      }
      return des;
    
}

int __gs_ffs(int value) //��32λ�е�һ������1��λ������0��ʼ��¼
{
     if(value == 0)
     {
        return 0;
     }
    else
    {
        int position = 0 ;
        while((value & 1) == 0)
        {
            value >>= 1;
            position++ ;
        }
        return position;
    }
}

int gs_strlen(char *src)
{
    int len ;
    while(*src ++ != '\0')
    {
        len++;
    }
    return len;
}

//���1��������2 �򷵻�1 ���򷵻�0
int gs_strcmp(char * src1 , char * src2)
{
    int len1;
    while(src1[len1] != '\0')
    {
        len1 ++;
    }
    
    while(*src2 != 0)
    {
        int i = 0;
        while(src1[i] == src2[i] && src1[i]!='\0')
        {
            i++;
        }
            if(src1[i] == '\0')
            {
                return 1;
            }
            src2++;
    }
    return 0;
}

void * gs_memmove(void *dest , const void* src , gs_ubase_t n) //�ȱȽ�d��s��λ�ã������ظ�������ֵ�������ظ�������ֵ
{
    char *d = (char *)dest ;
    char *s = (char *)src ;
    if(s<d && s+n>d)
    {
        d = d + n;
        s = s + n;
        
        while(n--)
        {
            d--;
            s--;
            *d = *s;
            
        }
    }
    else
    {
        while(n--)
        {
            *d = *s;
            d++;
            s++;
        }
    }
    return dest;
}

void show_version(void)
{
		printf("\r\n");
		printf("--------------------\r\n");
		printf("|      welcome     |\r\n");
		printf("|     GS-thread    |\r\n");
		printf("|    version  is     |\r\n");
		printf("|        V1.0          |\r\n");
		printf("--------------------\r\n");
}
