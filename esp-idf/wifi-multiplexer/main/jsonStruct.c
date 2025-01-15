#include <stdio.h>
#include <string.h>


typedef struct{
    char* firstName ;
    char* lastName  ;
}Names;



int testParse( char* str ){
    int idx = 0 ;
    while( str[idx] != '\0'){
        if ( str[idx] == ','  || str[idx] == '"') {
            printf("\n") ;
        }
        else {
            printf("%c" , str[idx]) ;
        }
        idx ++ ;
    }
    return 0;
}


int main(){
    Names arr[1] = { { "Ayub " ,"Mohamed" } } ;
    char* json = "{\
    \"filename\": \"string\",\
    \"filename\": \"string\",\
    \"filename\": \"string\",\
    \"filename\": \"string\",\
    \"filename\": \"string\",\
    }";
    testParse(json) ;
    return 0;
}
