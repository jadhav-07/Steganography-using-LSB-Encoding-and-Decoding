// Name : Jitesh Narayan Jadhav
// Batch: 25012B
// Date : 02/09/2025
// Desription : Here we are checking for arg count, calling function to call check operation type.
//      LSB Image Stragnography    //

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>
OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    // Step 1 : Check the argc count >= 4 or not -> true go to the step 2
    // false print the error message , return e_failure
    // Step 2 : Call the check_operation_type((argv[1]) == E_encode))
    // true -> Step 3
    // Step 3 : Declare the struct _EncodeInfo encInfo;
    // Step 4 :Call the validatefunction(argv,&encinfo) true -> step 5 false return e_failure ;
    // Step 5 Call do_encoding(&encinfo);

    if (argc >= 2 && argc <=5)
    {
        OperationType optype = check_operation_type(argv[1]);

        // ---------------- ENCODE ----------------
        if (optype == e_encode)
        {
            if (argc < 4) // encode needs 4 or more args
            {
                printf("\nMinimum arguments should be FOUR for encoding\n");
                return e_failure;
            }

            struct _EncodeInfo encInfo;   // proper struct name
            if (read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                if (do_encoding(&encInfo) == e_success)
                {
                    printf("\nEncoding completed successfully!\n");
                    return e_success;
                }
            }
            return e_failure;
        }

        // ---------------- DECODE ----------------
        else if (optype == e_decode)
        {
            if (argc < 3) // decode needs 3 or more args
            {
                printf("\nMinimum arguments should be THREE for decoding\n");
                return e_failure;
            }

            struct _DecodeInfo decInfo;   // proper struct name
            if (read_and_validate_decode_args(argc, argv, &decInfo) == e_success)
            {
                if (do_decoding(&decInfo) == e_success)
                {
                    printf("\nDecoding completed successfully!\n");
                    return e_success;
                }
            }
            return e_failure;
        }

        // ---------------- UNSUPPORTED ----------------
        else
        {
            printf("\nInvalid operation type! Use -e for encode OR -d for decode.\n");
            return e_failure;
        }
    }
    else
    {
        printf("\nEncoding: ./a.out -e <.bmp_file> <.text_file> [output file]\nDecoding: ./a.out -d <.bmp_file> [output file]\n");
        printf("\nMinimum arguments should between TWO to FIVE \n");
        return e_failure;
    }
}



OperationType check_operation_type(char *symbol)
{
    // Step 1 : Check -e or not - > true return e_encode false -> Step2
    // Step 2 : Check -d or not - > true return e_decode false -> Step3
    // Step 3 : return e_unsupported;
    if(strcmp(symbol,"-e") == 0)
    {
        return  e_encode;
    }
    else if(strcmp(symbol,"-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }

}