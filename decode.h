#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "common.h"
#include "types.h"

#define MAX_DECODE_FILE_SUFFIX 5   // Max length for file extension

// Holds all decoding related info
typedef struct _DecodeInfo
{
    char *stego_image_fname;       // Stego image file name
    FILE *fptr_stego_image;        // Stego image file pointer
    int size_secret_file;          // Secret file size
    
    int extn_size;                 // Extension size
    
    char *output_file_name;        // Output file name
    char *output_file_basename;    // Base name for output
    FILE *fptr_output;             // Output file pointer
    char *extn; // Buffer for extension
} DecodeInfo;

// Open output file
Status open_output_file(DecodeInfo *decInfo);

// Validate input args
Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo);

// Perform decoding
Status do_decoding(DecodeInfo *decInfo);

// Open stego image
Status open_image_file(DecodeInfo *decInfo);

// Skip BMP header
Status skip_bmp_header(FILE *fptr_stego_image);

// Decode magic string
Status decode_magic_string(DecodeInfo *decInfo);

// Decode extension size
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

// Decode extension
Status decode_secret_file_extn(DecodeInfo *decInfo);

// Decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo);

// Decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo);

// Decode 1 byte from 8 LSBs
void decode_byte_from_lsb(unsigned char *ch, const unsigned char *image_buffer);

// Decode integer from 32 LSBs
void decode_int_from_lsb(int *data, const unsigned char *image_buffer);

#endif
