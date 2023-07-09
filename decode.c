#include <stdio.h>
#include "types.h"
#include <string.h>
#include "decode.h"
#include "common.h"
/* Function Definitions */
// Decode Secret file data..
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buff[8];
    char ch;
    int flag = 0;
    for(int i=0;i<decInfo->size_secret_file;i++)
    {

	fread(buff,1,8,decInfo->fptr_stego_image);
	decode_lsb_to_byte(&ch,buff);
	fputc(ch,decInfo->fptr_secret);
	flag = 1;
    }
    if(flag == 1)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }


} 
// Decode Secret file Size..
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buff[8];
    char ch;
    int flag = 0;
    for(int i = 0;i < sizeof(long int);i++)
    {
	fread(buff,1,8,decInfo->fptr_stego_image);
	decode_lsb_to_byte(&ch,buff);
	decInfo->size_secret_file = decInfo->size_secret_file + ch;
	flag = 1;
    }
    printf("The size of secret file size: %lu\n",decInfo->size_secret_file);
    if(flag == 1)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}
//Decode Secret file extension..
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buff[8];
    char ch;
    int flag = 0;
    char extn[decInfo->extn_size];
    for(int i = 0;i < decInfo->extn_size;i++)
    {
	fread(buff,1,8,decInfo->fptr_stego_image);
	decode_lsb_to_byte(&ch,buff);
	extn[i] = ch;
	flag = 1;
    }
    extn[decInfo->extn_size] = '\0';
    decInfo->secret_extn = extn;
    if(decInfo->flag == 0)
    {
	printf("Info:adding name\n");
	printf("secret fname : %s\n",decInfo->secret_fname);
	printf("The secret file extension is: %s\n",decInfo->secret_extn);
	decInfo->fname = strcat(decInfo->secret_fname,decInfo->secret_extn);
	printf("printing fname\n");
	printf("name added: %s\n",decInfo->secret_fname);
	decInfo->fptr_secret = fopen(decInfo->fname,"w+");
	printf("Info:Final out put File opened\n");
    }
    
    if(flag == 1)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}
// Decode Secret file extension size..
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buff[8];
    char ch=0;
    int flag =0;
    for(int i = 0;i < sizeof(long int);i++)
    {
	fread(buff,1,8,decInfo->fptr_stego_image);
	decode_lsb_to_byte(&ch,buff);
	decInfo->extn_size = decInfo->extn_size + ch;
	flag = 1;
    }
    printf("The secret file extension size is: %lu\n",decInfo->extn_size);
    if(flag == 1)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}



// Decoding magic string length and magic string..
Status decode_magic_string(long magic_string_len, DecodeInfo *decInfo)
{
    // Decoding the magic string length..   
    char img_buff[8];char ch1;   
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    for(int i=0;i<sizeof(int);i++)
    {
	fread(img_buff,1,8,decInfo->fptr_stego_image);
	decode_lsb_to_byte(&ch1,img_buff);
	decInfo->magic_string_len = decInfo->magic_string_len+ ch1;
    }
    printf("The length of the magic string is: %lu\n",decInfo->magic_string_len);

    // Decode magic string..
    char ch2;
    char magic_string_char[decInfo->magic_string_len + 1];
    for(int i=0;i<decInfo->magic_string_len;i++)
    {
	fread(img_buff,1,8,decInfo->fptr_stego_image);
	decode_lsb_to_byte(&ch2,img_buff);
	magic_string_char[i] = ch2;
    }
    magic_string_char[decInfo->magic_string_len] = '\0';
    decInfo->magic_string = magic_string_char;
    if(strcmp(decInfo->magic_string,MAGIC_STRING) == 0)
    {
	printf("Magic String is : %s\n",decInfo->magic_string);

	return e_success;
    }
    else
	return e_failure;
}
// Opening Files..
Status open_files_decode(DecodeInfo *decInfo)
{
    // Secret file
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    // Do Error handling
    if (decInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


// Decode lsb bit in byte..
Status decode_lsb_to_byte(char * ch, char *buff)
{
    char temp=0;
    for(int i=0;i<sizeof(ch);i++)
    {
	temp = temp | ((buff[i] & 1) << i);
    }
    *ch = temp;
}
// Reading and Validating the command line argument..
Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo)
{
    if(strcmp(strstr(argv[2],"."),".bmp") == 0 && argc == 3)
    {
	printf("No out put file given\n");
	decInfo->stego_image_fname = argv[2]; 
	decInfo->secret_fname ="res";
	decInfo->flag = 0;
	return e_success;
    }
    else if(strcmp(strstr(argv[2],"."),".bmp") == 0 && argv[3] != NULL)
    {
	printf("Argument 3 given\n");
	if(strstr(argv[3],".txt") != NULL)
	{   
	    printf("Output file given\n");
	    decInfo->stego_image_fname = argv[2];
	    decInfo->secret_fname = argv[3];
	    decInfo->flag = 1;
	    return e_success;
	}
	else
	{
	    printf("Output file given without extension\n");
	    decInfo->stego_image_fname = argv[2]; // Pointing to the encoded image file..
	    decInfo->secret_fname = argv[3];      // Pointing to the 4th argument given..
	    decInfo->flag = 0;
	    return e_success;
	}
    }
    else
    {
	return e_failure;

    }	
}
// Do decoding..
Status do_decoding(DecodeInfo *decInfo)
{

    if(open_files_decode(decInfo) == e_success)
    {
	printf("Open file success without opening output file\n");
	if(decode_magic_string(decInfo->magic_string_len,decInfo) == e_success)
	{
	    printf("Info:The file is encoded\n");
	    if( decode_secret_file_extn_size(decInfo) == e_success)
	    { 
		printf("Info:Secret file extension size decoded successfully\n");  
		if(decode_secret_file_extn(decInfo) == e_success)
		{
		    printf("Info:Secret file extension decoded successfully\n");
		    if(decode_secret_file_size(decInfo) == e_success)
		    {
			printf("Info:Secret file Size decoded successfully\n");
			if(decode_secret_file_data(decInfo) == e_success)
			{
			    printf("Info:Secret file data decoded successfully\n");
			    printf("Info:Your secret data is in %s\n",decInfo->secret_fname);
			}
			else
			{
			    printf("Info:Secret data decoding not successfull\n");
			}

		    }
		    else
		    {
			printf("Secret file size decoded not success\n");

		    }
		}
		else
		{
		    printf("Info:Secret file size extension decoded not success\n");
		}	    
	    }	   
	}
	else
	{
	    printf("The file is not encoded\n");
	}
    }
    else
    {
	printf("Open file Not Success\n");
    }	
}
