#include <stdio.h>


typedef struct{
    char* firstName ;
    char* lastName  ;
}Names;

int main(){
    Names arr[1] = { { "Ayub " ,"Mohamed" } } ;
    printf("First Name : %s" , arr[0].firstName ) ;
}
