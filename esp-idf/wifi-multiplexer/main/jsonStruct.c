#include <stdio.h>
#include <string.h>


#define CHARBUFFER 50

typedef struct{
    char* key   ;
    char* value ;
}Pair;


// Turn the values from a string to an associative array
// Specific Structs for each and every response
// Shape of the data remains the same
// DELETE   request
// UPLOAD   request
// UPDATE   request
// DOWNLOAD request
// SNAPSHOT request

/*

Sample Input

{
  "folderId": "string",
  "fileList": [
    {
      "fileId": "string",
      "filename": "string",
      "thumbnail_img": "string",
      "lastModifiedDateInUTC": "string",
      "creationDateInUTC": "string"
    }
  ]
}

*/




int testParse( char* str )
{
    int idx = 0 ;
    while( str[idx] != '\0'){
        if ( str[idx] == ','  || str[idx] == '"')
        {
            printf("\n") ;
        }
        else if( str[idx] == '{' || str[idx] == '}' )
        {
            printf("\n") ;
        }
        // Beginning of the string
        else
        {
            char str[CHARBUFFER] ;
            char* tok = strtok(str , ":") ;
            while( tok != NULL )
            {
                printf(" % s\n", tok );
                tok = strtok(NULL, " : ");
                idx ++ ;
            }
        }
        idx ++ ;
    }
    return 0;
}


int main(){
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


