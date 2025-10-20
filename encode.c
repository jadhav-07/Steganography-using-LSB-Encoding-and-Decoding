// Name : Jitesh Narayan Jadhav
// Batch: 25012B
// Date : 02/09/2025
// Description : This code performs steganography encoding by hiding data from a secret file

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#define MAGIC_STRING "#*"



/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
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

uint get_file_size(FILE *fptr)
{
    // Find the size of the secret file data and return it
    fseek(fptr,0,SEEK_END);
    uint size =ftell(fptr);
    rewind(fptr);
    return size;
    
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //  Step 1 : Check the argv[2] is having the .bmp extension or not
    // true - > Store into the structure member step 2 , false - > return e_failure
    // Step 2 : Check the argv[3] is having the .txt extension or not
    // true - > Store into the structure member step 3 , false - > return e_failure
    // Step 3 : Check argv[4] != NULL or not true - > Step 4, false -> Step 5
    // Step 4 : Check the argv[4] is having the .bmp extension or not
    // true - > Store into the structure member step 6 , false - > return e_failure
    // Step 5 : Store the default.bmp into the structure member - > Step 6
    // Step 6 : return e_success;

    if(strstr(argv[2], ".bmp") != NULL)       // Check source image (must be .bmp)
    {
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        fprintf(stderr, "ERROR : Source file must be .bmp\n");
        return e_failure;
    }
    // Check secret file extension (.txt, .c, .sh allowed)
    if(strstr(argv[3], ".txt") != NULL)
    {
        encInfo -> secret_fname = argv[3];
    }
    else if(strstr(argv[3], ".c") != NULL)
    {
        encInfo -> secret_fname = argv[3];
    }
    else if(strstr(argv[3], ".sh") != NULL)
    {
        encInfo -> secret_fname = argv[3];
    }
    else
    {
        fprintf(stderr, "ERROR : Secret file must be .txt/ .c / .sh\n");
        return e_failure;
    }

    // check Stego image filename
    if(argv[4] != NULL)
    {
        if(strstr(argv[4], ".bmp") != NULL)
        {
            encInfo -> stego_image_fname = argv[4];   
        }
        else
        {
            fprintf(stderr, "ERROR : Stego image must be .bmp\n");
            return e_failure;
        }
    }
    else
    {
        encInfo -> stego_image_fname = "default.bmp";  // default file name
    }
    
    return e_success;
}
// Open source, secret, and stego image files
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
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

Status check_capacity(EncodeInfo *encInfo)
{
    // Step 1 :image_capacity= get the size from  get_image_size() of Bmp function
    //  Step 2 : Find the size of secret file data by calling this function get_file_size();
    //  Step 3 : image_ capacity > 54 + 16 + 32 + 32 + 32 + (sizeofdata * 8)
    //  true - > e_success false -> e_failure
    uint image_capacity = get_image_size_for_bmp(encInfo-> fptr_src_image);

    encInfo -> size_secret_file = get_file_size(encInfo-> fptr_secret);
    
    uint required_capacity = 54 + (strlen(MAGIC_STRING) * 8) + 32 + (strlen(encInfo->extn_secret_file) * 8) + 32 + (encInfo->size_secret_file * 8);

    // Required space: header + magic + extension size + extension + secret size + data

    printf("Image Capacity : %u\n",image_capacity);
    printf("Required Capacity : %u\n",required_capacity);

    if(image_capacity > required_capacity)
    {
        return e_success;
    }
    else
    {
        fprintf(stderr, "ERROR : Image does not have enough capacity to hold secret data\n");
        return e_failure;
    }
}
 
// Copy first 54 bytes 
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // Step1 : rewind fptr_src_image
    // Step 2: char imageBuffer[54];
    // Step 3 : Read the 54 bytes from src image store into imageBuffer;
    // Step 4 : write the 54 bytes to the dest_image;
    // return e_success;

    rewind(fptr_src_image);      // reset pointer to start

    char imageBuffer[54];       // buffer to hold header

     // Read 54-byte BMP header
    fread(imageBuffer, 1, 54, fptr_src_image);
    // Write header into stego image
    fwrite(imageBuffer, 1, 54, fptr_dest_image);

    return e_success;

}

// Encode magic string into image 

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // char imageBuffer[8];
    // Step1 : Find the size of magicstring
    // Step 2 :Generate the loop upto the size
    // Step 3 : Read 8 bytes from src_image store into the imageBuffer
    // Step 4 : Call the encode_byte_to_lsb(magic_string[i] , imageBuffer);
    // Step 5 : Write the imagebuffer into the dest_image
    // Step 6 : Repeat the process upto the size

    // return e_success;
    char imageBuffer[8];
    int len = strlen(magic_string);


    for(int i=0; i<len; i++)
    {
        fread(imageBuffer, 1, 8, encInfo -> fptr_src_image);
        
        encode_byte_to_lsb(magic_string[i] , imageBuffer);         // encode one character     

        fwrite(imageBuffer, 1, 8, encInfo -> fptr_stego_image);    // write modified bytes
    }
    return e_success;

}

// Encode extension size into image
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    // Char image_Buffer[32];
    // Step 1 : Read a 32 bytes from the src_image store into the image_buffer
    // Step 2 : Call encode_size_to_lsb(size,image_Buffer);
    // Step 3 : write the image_buffer into the dest_image;

    // retuen e_succes;
    char image_Buffer[32];
    int extn_size = strlen(encInfo -> extn_secret_file);

    if(fread(image_Buffer, 1, 32, encInfo-> fptr_src_image) != 32)
    {
        fprintf(stderr, "ERROR: Failed to read from source image while encoding extn size\n");
        return e_failure;
    }

    encode_size_to_lsb(extn_size,image_Buffer);

    if(fwrite(image_Buffer, 1, 32, encInfo-> fptr_stego_image) != 32)
    {
        fprintf(stderr, "ERROR: Failed to write stego image while extn size\n");
        return e_failure;
    }

    return e_success;
}

//Encode file extension into image 
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    // char imageBuffer[8];
    // Step1 : Find the size of extension(.txt)
    // Step 2 :Generate the loop upto the size
    // Step 3 : Read 8 bytes from src_image store into the imageBuffer
    // Step 4 : Call the encode_byte_to_lsb(magic_string[i] , imageBuffer);
    // Step 5 : Write the imagebuffer into the dest_image
    // Step 6 : Repeat the process upto the size

    // return e_success;
    char imageBuffer[8];                // Create a buffer to hold 8 bytes from the source image
    int len = strlen(file_extn);
    
    for(int i=0; i<len; i++)
    {
        fread(imageBuffer, 1, 8, encInfo -> fptr_src_image);   // Read 8 bytes from the source image into image_Buffer
        encode_byte_to_lsb(file_extn[i] , imageBuffer);
        fwrite(imageBuffer, 1, 8, encInfo -> fptr_stego_image);   // write bytes

    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // Char image_Buffer[32];
    // Step 1 : Read a 32 bytes from the src_image store into the image_buffer
    //  Step 2 : Call encode_size_to_lsb(size,image_Buffer);
    //  Step 3 : write the image_buffer into the dest_image;

    // retuen e_succes;
    char image_Buffer[32];                                // buffer to hold 32 bytes 
    fread(image_Buffer,1,32,encInfo->fptr_src_image);         // Read 32 bytes from the source image 

    encode_size_to_lsb(file_size,image_Buffer);        // Encode the size of the secret file into the LSBs of the 32-byte image

    fwrite(image_Buffer,1,32,encInfo->fptr_stego_image);      // Write the modified 32-byte buffer 

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // char secret_data[secret_file_size];
    // char imageBuffer[8];
    // Step 1 : rewind the secret_file_fptr;
    // Step 2 :Generate the loop upto the secret_file_size
    // Step 3 : Read 8 bytes from src_image store into the imageBuffer
    // Step 4 : Call the encode_byte_to_lsb(secret_data[i] , imageBuffer);
    // Step 5 : Write the imagebuffer into the dest_image
    // Step 6 : Repeat the process upto the size
    // return e_success
   
    char secret_data[encInfo -> size_secret_file];
    char imageBuffer[8];                                  // Create a buffer of 8 bytes from the image

    rewind(encInfo->fptr_secret);                         // Reset

    for(int i=0; i<encInfo ->size_secret_file; i++)             // Loop through each byte of the secret file
    {
        fread(&secret_data[i], 1, 1, encInfo ->fptr_secret);         // Read 1 byte from the secret file 
        fread(imageBuffer,1,8,encInfo -> fptr_src_image);           // Read 8 bytes from the source image into imageBuffer
        encode_byte_to_lsb(secret_data[i] , imageBuffer);            
        fwrite(imageBuffer,1,8,encInfo -> fptr_stego_image);             // Write the modified 8 bytes into the stego image
    }
    return e_success;                           // After encoding the whole secret file, return success
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    // Copy the remaining data from src to dest
    // return e_success
    int ch;
    while((ch = fgetc(fptr_src)) != EOF)         // Read characters one by one from the source file until End Of File (EOF)
    {
        fputc(ch, fptr_dest);                     // Write the read character into the destination file
    }
    return e_success;

}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Step 1 : Generate the loop from 0 to  7
    // Step 2 : Clear the lsb bit of Imagebuffer[i];
    // Step 3 : Get the bit from the data then replace the bit with Imagebuffer[i] of lsb;
    // Step 4 : Reapeat the process upto 7
    for(int i=0; i<8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);
    }
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    // Step 1 : Generate the loop from 0 to 31
    // Step 2 : Clear the lsb bit of Imagebuffer[i];
    // Step 3 : Get the bit from the size then replace the bit with Imagebuffer[i] of lsb;
    // Step 4 : Reapeat the process upto 31
    for(int i=0; i<32; i++)
    {
        imageBuffer[i] = imageBuffer[i] & 0xFE;                   // clear LSB
        imageBuffer[i] = imageBuffer[i] | ((size >> (31 -i)) & 1);    // set bit
    }
    return e_success;
    
}

Status do_encoding(EncodeInfo *encInfo)
{
    // Step 1 : Call the open_files(encInfo) check the return value e_success or not
    //   true - > step 2 false -> return e_failure
    // Step 2 : Call the Check_capacity(encinfo) check the return value e_success or not;
    //   true - > step 3 false -> return e_failure
    // Step 3 : Call copybmpHeader()check the return value e_success or not;
    // true - > step 4 false -> return e_failure;
    // Step 4 : Call the encode_magic_string()check the return value e_success or not;
    //   true - > step 5 false -> return e_failure
    // Step 5 : Extract .txt from the secretfile_name and store into structure varaibale then find the size
    //  Step 6: Call the encode secret_file_extension_size() check the return value e_success or not;
    //   true - > step 7 false -> return e_failure
    //  Step 7: Call the encode secret_file_extension() check the return value e_success or not;
    //   true - > step 8 false -> return e_failure
    //  Step 8: Call the encode_secret_file_size() check the return value e_success or not;
    //   true - > step 9 false -> return e_failure
    //  Step 9: Call the encode_secret_file_data() check the return value e_success or not;
    //   true - > step 10 false -> return e_failure
    //  //  Step 10: Call the copy_remaining_data() check the return value e_success or not;
    //   true - > step 11 false -> return e_failure
    // Step 11 : e_success;
    

    if (open_files(encInfo) != e_success)
    {
        printf("Not OPening.\n");
        return e_failure;
    }  
    else
    {
        printf("File is opening successfully!\n");
    }

    if (check_capacity(encInfo) != e_success)
    {
        printf("Capacity check unsuccessful.\n");
        return e_failure;
    }
    else
    {
        printf("Check Capacity Success.\n");
    }

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf("Did not Copied header\n");
        return e_failure;
    }
    else
    {
        printf("Copied Header Success\n");
    }

    if (encode_magic_string(MAGIC_STRING, encInfo) != e_success)
    {
        printf("Unsuccessful encoding magic string\n");
         return e_failure;
    }
    else
    {
        printf("Successful encoding magic string\n");
    }

    char *ext = strstr(encInfo->secret_fname, ".");
    if (ext == NULL)
    {
        fprintf(stderr, "ERROR: Secret file has no extension\n");
        return e_failure;
    }

    strncpy(encInfo->extn_secret_file, ext, sizeof(encInfo->extn_secret_file) - 1);
    encInfo->extn_secret_file[sizeof(encInfo->extn_secret_file) - 1] = '\0';

    int size = strlen(encInfo->extn_secret_file);

    if (encode_secret_file_extn_size(size, encInfo) != e_success)
    {
        printf("Unsuccessful encode secret file extn size\n");
        return e_failure;
    }
    else
    {
        printf("Successful encode secret file extn size\n");
    }

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) != e_success)
    {
        printf("Unsuccessful encode secret file extn\n");
        return e_failure;
    }
    else
    {
        printf("Successful encode secret file extn\n");
        
    }

    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) != e_success)
    {
        printf("Unsuccessful encode secret file size\n");
        return e_failure;
    }
    else
    {
        printf("Successful encode secret file size\n");

    }

    if (encode_secret_file_data(encInfo) != e_success)
    {
        printf("Unsuccessful encode secret file data\n");
        return e_failure;
    }
    else
    {
        printf("Successful encode secret file data\n");
    }

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf("Unsuccessful copy remaining img data\n");
        return e_failure;
    }
    else
    {
        printf("Successful copy remaining img data\n");
    }

    return e_success;

}
