#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct DecodeInfo
{

    /* Secret File Info */
    char *fname;
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long extn_size;
    long size_secret_file;
    char *secret_extn;
    /* Is the out put file name passed or not */
    int flag; 
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    /* Magic string details */
    long magic_string_len;
    char *magic_string;
    
} DecodeInfo;


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);


/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);



/* Store Magic String */
Status decode_magic_string(long magic_string_len, DecodeInfo *decInfo);
/* Decode secret file extn size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);


/* Decode a byte into LSB of image data array */
Status decode_lsb_to_byte(char *ch, char *buff);


#endif
