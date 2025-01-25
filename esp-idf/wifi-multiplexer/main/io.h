#ifndef IO_H
#define IO_H

typedef enum FILE_TYPE {
    JPEG                  ,
    TXT                   ,
    PNG                   ,
    MP4                   ,
    PDF                   ,

}FILE_TYPE;

typedef struct {
  FILE_TYPE        extension     ;
  char*            name          ;
  char*            path          ;
  unsigned long    created_on ;
  unsigned long    updated_on ;
}FileMetaData;


void writeFile( FileMetaData* file_details )  ;
void readFile( char* path ) ;
void init_sd_card()         ;

#endif
