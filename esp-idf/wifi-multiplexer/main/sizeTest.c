#include <stdio.h>
#include <string.h>


size_t getSize( char* json )
{
    return sizeof(json);
}




int main()
{
    char* json = "Hello world" ;
    printf("%d" , sizeof(json)) ;
    return 0;
}
