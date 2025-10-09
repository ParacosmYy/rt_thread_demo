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

int __gs_ffs(int value)
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

void show_version(void)
{
		
		printf("--------------------\r\n");
		printf("|      welcome     |\r\n");
		printf("|     GS-thread    |\r\n");
		printf("|    version  is     |\r\n");
		printf("|        V1.0          |\r\n");
		printf("--------------------\r\n");
}
