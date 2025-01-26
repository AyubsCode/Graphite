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

typedef enum IO_ERROR{
    EXIT_SUCCESSFUL       , // Successful Event
    SPECIFICATION_FAILURE , // Failed to specify file type
    SIZE_BOUND            , // Exceeded file size
    METADATA_ERROR        , // Metadata error
}IO_ERROR;


// Typedef for array

typedef IO_ERROR ( *read_callback_arr )( FileMetaData* );


IO_ERROR jpeg_handler( FileMetaData* file_details  ) ;
IO_ERROR txt_handler(  FileMetaData* file_details  ) ;
IO_ERROR png_handler(  FileMetaData* file_details  ) ;
IO_ERROR mp4_handler(  FileMetaData* file_details  ) ;
IO_ERROR pdf_handler(  FileMetaData* file_details  ) ;

void writeFile( FileMetaData* file_details )  ;
void readFile(  FileMetaData* file_details )  ;
void init_sd_card()         ;

#endif
