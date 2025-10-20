// Name : Jitesh Narayan Jadhav
// Batch: 25012B
// Date : 02/09/2025
// Description :  Decode secret data from stego BMP file and save to output file

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#define MAGIC_STRING "#*"

Status open_output_file(DecodeInfo *decInfo) // opening output file
{
    decInfo->fptr_output = fopen(decInfo -> output_file_name, "wb");        // open file in write-binary mode
    if(decInfo -> fptr_output == NULL)       // if file didn't open, return failure
    {
        fprintf(stderr, "ERROR : Unable to open file %s \n",decInfo ->output_file_name);
        return e_failure;
    }
    return e_success;

}
  // function to check command line inputs for decoding
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    if(argc < 3 || strstr(argv[2], ".bmp") == NULL)       // check if .bmp file is provided in arguments
    {
        fprintf(stderr, "ERROR : missing input file or invalid input stego file\n");
        return e_failure;
    }
    else
    decInfo ->stego_image_fname = argv[2];      // store input image file name
    
    if(argc > 3)      // check if user gave output file name also
    { 
        decInfo ->output_file_basename = argv[3];
    }
    else
    {
        decInfo->output_file_basename = "decoded";                    // if not, keep default file name
    }

    return e_success;
}

              // Perform decoding
Status do_decoding(DecodeInfo *decInfo)
{
    if (open_image_file(decInfo) == e_failure) 
    {
        printf("FAIL: Did not open image file\n");
        return e_failure;
    }
    else
    {
        printf("opened image file\n");
    }
    if (skip_bmp_header(decInfo->fptr_stego_image) == e_failure)      //skipping bmp header
    {
        printf("FAIL: Unsuccessful skip bmp header\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }
    else
    {
        printf("Successful skip bmp header\n");
    }
    if (decode_magic_string(decInfo) == e_failure)                   // Decoding magic string
    {
        printf("FAIL: Unsuccessful to decoded magic string\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }
    else
    {
        printf("Successfully decoded magic string\n");
    }
    if (decode_secret_file_extn_size(decInfo) == e_failure)            // decoding secret file extension
    {
        printf("FAIL: Did not decoded secret file extn size\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }
    else
    {
        printf("Decoded secret file extn size\n");
    }
    if (decode_secret_file_extn(decInfo) == e_failure)              // Decoding secret file extn
    {
        printf("FAIL: Did not decoded secret file extn\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }
    else
    {
        printf("Successfully decoded secret file extn\n");
    }
    if (decode_secret_file_size(decInfo) == e_failure)              // decoding secret file size
    {
        printf("FAIL: Unsuccessful to decode secret file size\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }
    else
    {
        printf("Successfully decoded secret file size: %d\n",decInfo->size_secret_file);
    }
   
    if (open_output_file(decInfo) == e_failure)                       // opening output file
    {
        printf("FAIL: Did not opened output file\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }
    else
    {
        printf("Opened output file: %s\n",decInfo->output_file_name);
    }
    if (decode_secret_file_data(decInfo) == e_failure)              // decoding secret file data
    {
        printf("FAIL: Did not decoded secret file data\n");
        fclose(decInfo->fptr_output);
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }
    else
    {
        printf("Decoded secret file data\n");
    }
    fclose(decInfo->fptr_output);
    fclose(decInfo->fptr_stego_image);
    printf("INFO: Decoding completed successfully. Secret saved to %s\n",decInfo->output_file_name);
    return e_success;

}

// Open stego image
Status open_image_file(DecodeInfo *decInfo)
{
    decInfo -> fptr_stego_image = fopen(decInfo-> stego_image_fname, "rb");      // open file in read-binary mode
    if(decInfo -> fptr_stego_image == NULL)       // check if fopen failed
    {
        fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo-> stego_image_fname);    // if file not opened, print error
        return e_failure;
    }
    return e_success;
}

// Skip BMP header
Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image, 54, SEEK_SET);         // move file pointer after header
    return e_success;
}

// Decode magic string
Status decode_magic_string(DecodeInfo *decInfo)
{
    char expected[] = MAGIC_STRING;                 // expected magic string "#*"
    char ch;
    char imageBuffer[8];                            // buffer for 8 bytes (LSBs)

    for (int i = 0; i < strlen(expected); i++)
    {
        fread(imageBuffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb((unsigned char *)&ch, (unsigned char *)imageBuffer);    //decode 1 char

        printf("DEBUG: Decoded char = '%c' (0x%02X), Expected = '%c'\n", ch, (unsigned char)ch, expected[i]);
        if (ch != expected[i])         // check mismatch
        { 
            fprintf(stderr, "ERROR : Magic string do not match %d expected %c, got %c\n",
                    i, expected[i], ch);
            return e_failure;
        }
    }
    return e_success;                // magic string matched
}


// Decode extension size
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    int extn_size;
    unsigned char buffer[32];                // buffer for 32 bytes (LSBs)

    fread(buffer, 1, 32, decInfo ->fptr_stego_image);
    decode_int_from_lsb(&extn_size, buffer);               // extract integer from LSBs

    printf("Decoded secret file extn size %d\n",extn_size);

    decInfo->extn = malloc(extn_size + 1);             // allocate memory for extension
    if(decInfo->extn == NULL)
    {
        fprintf(stderr, "ERROR : Memory allocation failed for output file\n");
        return e_failure;
    }

    decInfo-> extn[extn_size] = '\0';         // null terminate string
    decInfo->extn_size = extn_size;           // store extension size

    return e_success;
}

// Decode extension
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char imageBuffer[8];                    // buffer for 8 bytes
    char ch;

    // Decode extension characters
    for (int i = 0; i < decInfo->extn_size; i++)
    {
        if (fread(imageBuffer, 1, 8, decInfo->fptr_stego_image) != 8)          // read 8 bytes
        {
            fprintf(stderr, "ERROR: Failed to read data for extension\n");
            return e_failure;
        }
        decode_byte_from_lsb((unsigned char *)&ch, (unsigned char *)imageBuffer);
        decInfo->extn[i] = ch;
    }
    decInfo->extn[decInfo->extn_size] = '\0';             // null terminate string

    printf("Decoded secret file extension: %s\n", decInfo->extn);
     
    if (strcmp(decInfo->extn, ".txt") != 0 &&
        strcmp(decInfo->extn, ".c")   != 0 &&
         strcmp(decInfo->extn, ".sh")  != 0)
   {
    fprintf(stderr, "ERROR: Unsupported file extension decoded: %s\n", decInfo->extn);
    return e_failure;
   }


    // --- Build final output file name ---
    const char *basename = decInfo->output_file_basename;
    const char *ext = decInfo->extn;

    size_t basename_len = strlen(basename);
    size_t ext_len = strlen(ext);

    // Check if basename already ends with the same extension
    int already_has_ext = 0;
    if (basename_len >= ext_len)
    {
        if (strcmp(basename + basename_len - ext_len, ext) == 0)        // compare last characters
        {
            already_has_ext = 1;     // extension already present
        }
    }

    size_t need = basename_len + (already_has_ext ? 0 : ext_len) + 2;      // total memory needed

    decInfo->output_file_name = malloc(need);
    if (!decInfo->output_file_name)
    {
        fprintf(stderr, "ERROR: Memory allocation failed for output file name\n");
        return e_failure;
    }

    strcpy(decInfo->output_file_name, basename);

    if (!already_has_ext)
    {
        if (ext[0] != '.') strcat(decInfo->output_file_name, ".");     // add dot if needed
        strcat(decInfo->output_file_name, ext);
    }

    printf("Final output file name: %s\n", decInfo->output_file_name);

    return e_success;
}

// Decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    unsigned char buffer[32];          // buffer to hold 32 bytes

    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)       // read 32 bytes from stego image
    {
        fprintf(stderr, "ERROR: Failed to read file size\n");
        return e_failure;
    }
    decode_int_from_lsb(&decInfo->size_secret_file, buffer);      // extract integer from LSBs

    return e_success;

}

// Decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];                  // buffer to hold 8 bytes
    char ch;

    for(int i=0; i<decInfo->size_secret_file; i++)
    {
        if(fread(buffer, 8, 1, decInfo->fptr_stego_image) != 1)     // read 8 bytes = 1 char
        {
            fprintf(stderr, "ERROR: Failed to read data for decoding\n");
            return e_failure;
        }

        decode_byte_from_lsb(&ch, buffer);    // decode 1 char from LSBs

        fputc(ch, decInfo->fptr_output);
    }
    return e_success;
}

// Decode 1 byte from 8 LSBs
void decode_byte_from_lsb(unsigned char *ch, const unsigned char *image_buffer)
{
    *ch =0;

    for(int i=0; i<8; i++)
    {
        *ch = (*ch << 1) | (image_buffer[i] & 1);      // shift left & add LSB
    }
}

// Decode integer from 32 LSBs
void decode_int_from_lsb(int *data,const unsigned char *image_buffer)
{
    *data =0;
    for(int i=0; i<32; i++)
    {
        *data = (*data << 1) | (image_buffer[i] & 1);        // shift left & add LSB
    }
}