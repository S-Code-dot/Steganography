#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
// Encode to LSB..
Status encode_byte_to_lsb(char data,char *image_buffer)
{
    char temp=0;   
    for(int i=0;i<8;i++)
    {
	image_buffer[i] = image_buffer[i] & (~1);  // Clear lsb of data[i]..
	temp = data & 1;                           // Getting the LSB of the size of the..
	image_buffer[i] = image_buffer[i] | temp;  // Modifing the data..
	data = data >> 1;                          // Modifing the data by doing right shift.. 
    } 
}
// Copy remaining data of source image file..
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char data[1];
    while( fread(data,1,1,fptr_src) > 0)
	fwrite(data,1,1,fptr_dest);
     return e_success;    
}

// Encode secret file data..
Status encode_secret_file_data(EncodeInfo *encInfo) 
{
    long int offset = ftell(encInfo->fptr_src_image);      // Checking the present offset of source file..
    char data[8];char secret_buff[encInfo->size_secret_file];  // Two character array declaire..
    fseek(encInfo->fptr_secret,0,SEEK_SET);                    // Setting file pointer in the start in secret file..
    fread(secret_buff,1,encInfo->size_secret_file,encInfo->fptr_secret);  // Reading the total secret file data..
    for(int i = 0;i < encInfo->size_secret_file;i++)     // Loop to get secret file character data one by one..
    {
	fread(data,1,8,encInfo->fptr_src_image);         // Reading 8 byte of data from source file..
	encode_byte_to_lsb(secret_buff[i],data);         // Calling the function to modify the source file data.. 
	fwrite(data,1,8,encInfo->fptr_stego_image);      // Writing the modified data to the output file..
    }
    return e_success;
}

// Encode Secret File Size..
Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
    char data[8];char size;                          // One character array and one character variable declared..
    for(int i=0;i<sizeof(long int);i++)               // Loop to get the source file data 8 byte at once..
    {
	fread(data,1,8,encInfo->fptr_src_image);       // Reading 8 byte from source file..
	size = file_size;                              // Here file_size is 8 byte data but taking only 1 byte..
	encode_byte_to_lsb(size,data);                 // Calling the function to modify the source file data..
	fwrite(data,1,8,encInfo->fptr_stego_image);    // Writting the modified data to the output file.. 
	file_size = file_size >> 8;                    // After taking the one byte taking next  byte of the size..
    }
    return e_success;

}
// Encoding the secret file extension..
Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)
{

    char data[8];   // Character array declared..
    long int offset = ftell(encInfo->fptr_src_image);   // Getting the current offset value of the source file..
    for(int i = 0;i<strlen(file_extn);i++) 
    {
	fread(data,1,8,encInfo->fptr_src_image);   // Reading 8 byte from the source file.. 
	encode_byte_to_lsb(file_extn[i],data);     // Calling the function to modify the source file..
	fwrite(data,1,8,encInfo->fptr_stego_image);  // Writting modified data to the output file..

    }
    return e_success;
}
// Store magic string length and magic string..
Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)
{
    char data[8];uint len = strlen(MAGIC_STRING);  // Getting the magic string length..
    char s_len=0;   
    // Encoding Magic string length..
    for(int i=0;i<sizeof(int);i++)
    {
	fread(data,1,8,encInfo->fptr_src_image);    // Reading 8 byte data from source file..
	s_len = len;                   // len is 4 byte taking 1 byte at once..            
	encode_byte_to_lsb(s_len,data);  // Calling the function to modify the source file..
	fwrite(data,1,8,encInfo->fptr_stego_image); // Writting the modified data to the output file..
	len = len >> 8;    // Removing 8 byte from lsb..

    }
    printf("Magic string length encoding success\n");

    // Encoding the Magic String..
    long int offset= ftell(encInfo->fptr_src_image);  // Getting the present offset of the source file..  
    printf("It is magic string encoding\n");
    for(int i = 0;i<strlen(MAGIC_STRING);i++)         // Loop running  to get the magic string character one by one..
    {
	fread(data,1,8,encInfo->fptr_src_image);    // Reading 8 byte from source file..
	encode_byte_to_lsb(magic_string[i],data);   // Calling the function to modify the source file..
	fwrite(data,1,8,encInfo->fptr_stego_image); // Writting the modified data to the output file..
    }  
    printf("Magic string encoding success\n");
    return e_success;
}
// Header bmp copy...
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest)
{
    char *header[54];         // Character array to store first 54 byte header data..
    fseek(fptr_src_image,0,SEEK_SET);  // Setting source file pointer to the first..
    fread(header,1,54,fptr_src_image); // Reading 54 byte from source file..
    fwrite(header,1,54,fptr_dest);   // Writting the 54 byte to the output file..

    if(ferror(fptr_dest))        // Condition to check the output file open or not..
    {
	printf("Header not Copied\n");  // If the ferror() true means not opened..
	return e_failure;   // returning enum value e_failure..
    }
    else
    {
	printf("Header Copied\n");
	return e_success;  // If opened then returning e_success..
    }
}
// Image secret file size..
uint get_file_size(FILE *fptr)  
{

    fseek(fptr,0,SEEK_END);         // Setting source file pointer to the end..
    uint secret_size = (ftell(fptr)) ;  // Calculating the secret file size..
    fseek(fptr,0,SEEK_SET);  // After getting setting the secret file pointer to the first..
    return secret_size;   // Returning the size..
}
// Check Capacity..
Status check_capacity(EncodeInfo *encInfo)
{
    int count=0,i=0;  
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);

    while(encInfo->secret_fname[i] != '\0')
    {
	if(encInfo->secret_fname[i] == '.')
	{
	    for(int  j=i;encInfo->secret_fname[j] != '\0';j++)
	    {
		count += 1;
		i = j;
	    }

	}
	i++;
    }
    uint len_extn = count;
    uint size_extn = count * 8;
    encInfo->secret_file_extn_size = len_extn;
    uint size_magic_string = (strlen(MAGIC_STRING)*8);
    uint magic_string_length = strlen(MAGIC_STRING);
    printf("Size of magic string = %u\n",size_magic_string);
    printf("Length of magic string = %u\n",magic_string_length);
    printf("Extension size: %u\n",size_extn);
    printf("Extension length: %u\n",size_extn);
    uint capacity = (encInfo->image_capacity -(54 + sizeof(int)*8 + encInfo->size_secret_file + sizeof(int)*8 + size_extn + sizeof(int)*8 + size_magic_string));
    if(capacity > 0)
	return e_success;
    else
	return e_failure;
}
// Function to read and validate the command line argument..
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
    if((strstr(argv[2],".bmp") != NULL) && (strstr(argv[3],".txt") != NULL))  // Checking bmp and txt file passed or not..
    {

	encInfo->src_image_fname = argv[2]; // Storing 3rd argument or source file in pointer..    
	encInfo->secret_fname = argv[3];  // Storing the secret file in pointer..
	if(argv[4] == NULL)      // Condition to check 5th argument or output file passed or not..
	{
	    encInfo->stego_image_fname="stego.bmp"; // If not passed the create one file name stego.bmp storing in pointer..
	    printf("No output file given it will be created by compiler\n");
	}
	else
	{
	    encInfo->stego_image_fname=argv[4];  // If output file passed the storing in a pointer..
	    printf("Out put file present\n");
	}
	return e_success;
    }
    else if(strstr(argv[2],".bmp") == NULL)  // If source file not passed..
    {
	printf(".bmp file not found\n");  // Print error message..
	return e_failure;

    }
    else
    {
	printf(".txt file not found\n");  // If secret file not passed..
	return e_failure;
    }
}
// Checking which operation user want encode or decode..
OperationType check_operation_type(char *argv[]) // Argument passed commandline..
{
    if (strcmp(argv[1],"-e") == 0)   // If the 2nd argument is "-e"..
    {

	return e_encode;  // Returning encode or user want encoding..
    }
    else if(strcmp(argv[1],"-d") == 0)  // Or if argument is "-d"..
    {
	return e_decode;  // Decoding operation need..

    }
    else   // If no argument passed then print the messages..
    {
	printf("Please Enter valid input\n");
	printf("Encoding:./a.out -e beautiful.bmp secret.txt\n");
	printf("Decoding:./a.out -d stego.bmp backup.bmp\n");

    }
}
// Encode secret file extension size..
Status encode_secret_file_extn_size(long int size,EncodeInfo *encInfo)
{
    char buff[8];

    for(int i=0;i<sizeof(long int);i++)
    {
	fread(buff,1,8,encInfo->fptr_src_image);
	encode_byte_to_lsb(size,buff);
	fwrite(buff,1,8,encInfo->fptr_stego_image);
	size = size >> 8; 
    }
    return e_success;
} 
// Get image size function..
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// Encoding Function..    
Status do_encoding(EncodeInfo *encInfo)
{   
    // Opening files..
    if(open_files(encInfo) == e_success)
    {
	printf("Info:Open files successfull\n");
	// Checking Capacity.. 
	if(check_capacity(encInfo) == e_success)
	{
		printf("Info:Capacity Checked Successfully\n");
	    // Copying bmp header bytes..
	    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
	    {
		printf("Info:bmp header copied\n");

		// Encoding magic string..
		if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
		{
                    printf("Magic string encoded successfully\n");
		    // Finding the secret file extension..
		    char *extn_secret = strstr(encInfo->secret_fname, ".");  
		    int i = 0;
		    while(extn_secret[i] != '\0')
		    {
			encInfo->extn_secret_file[i] = extn_secret[i] ;
			i++;
		    }
		    // Encode secret file extn size..
		    if(encode_secret_file_extn_size(encInfo->secret_file_extn_size,encInfo) == e_success)
		    {
			printf("INFO:Secret file extension size encoded successfull\n");


			// Encode secret file extn..
			if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success) // Function calling..
			{
			    printf("Secret file extension encoded successfully\n");
			    fseek(encInfo->fptr_secret,0,SEEK_END);
			    encInfo->size_secret_file = ftell(encInfo->fptr_secret);
			    printf("secret file size:%lu\n",encInfo->size_secret_file); 
			    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
			    {	
                                 printf("Info:Secret file size encoded Successfully\n");
				// Encoding secret file data..
				if(encode_secret_file_data(encInfo) == e_success)
				{
				   printf("Info:Secret file encoded successfully\n");

				    // Copying the remaining data of image file..
				    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
				    {
					printf("Info:Remaining data Copied Successfully\n");
				    }
			            else
				    {
					printf("Error:Remaining data not Copied\n");
				    }
				}
	                        else
				{
				   printf("Error:Secret file data not encoded \n");
				
				}
			    }
			    else
			    {
			      printf("Error:Secret file size not encoded");
			    }
			}
			else
			{
			  printf("Error:Secret file extension not encoded\n");
			
			}
		    }
		    else
		    {
		      printf("Error:Secret file extension size not encoded\n");
		    
		    }
		}
		else
		{
		  printf("Magic String not encoded\n");
		}
	    }
	    else
	    {
		printf("Error:bmp header not copied\n");
	      
	    }
	}
	else
	{
	 printf("Error:bmp file dont have enough capacity\n");
	
	}
    }
    else
    {
	printf("Error:Open files Unsuccess\n");
    }
}
