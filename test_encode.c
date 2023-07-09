#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
#include "decode.h"

// Main Function Definition..
int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
    if(argc >= 3)
    {
	int ret= check_operation_type(argv);
	if(ret == e_encode)
	{
	    printf("Info:You are doing encoding\n");
	    if(read_and_validate_encode_args(argv,&encInfo) == e_success)
	    {
		printf("Info:Read and Validation Successfull\n");
		do_encoding(&encInfo);
                img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
                printf("INFO: Image size = %u\n", img_size);
 
	    }

	}
	else if(ret == e_decode)
	{
	    printf("Info:You are doing decoding\n");
	    if(read_and_validate_decode_args(argc,argv,&decInfo) == e_success)
	    {
		printf("Info:Read and Validation Successfull\n");  
		do_decoding(&decInfo);
	    }
	    else
	    {
		printf("Error:Command line argument is wrong\n");
	    }

	}	   
    }
    else
    {
       printf("Error: Please pass arguments properly\n");
    
    }
    return 0;
}
