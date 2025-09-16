#include <gsthread.h>
#include <gshw.h>

char *gs_strncpy(char *des , char *src , gs_uint8_t n)
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
             {
                *d++ = 0;
                 break;
             }
           }
            
        }while (--n != 0);            
      }
      return des;
    
}

