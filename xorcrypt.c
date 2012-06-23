#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FL_EXT ".xor"

typedef unsigned char byte;

/*
 * get_next_byte 
 * Non null in argument sets the array, length is required.
 * A null in argument gets the next byte from the array (end to beginning) 
 */
byte get_next_byte(byte* in, int length)
{
    static byte* bytes = NULL;
    static int count = 0;
    static int ocount;

    if(!in)
    {
	if(!count)
	    count = ocount;
	return bytes[count--];
    }
    else
    {
	bytes = in;
	count = ocount = length - 1;
	return (byte)0;
    }
}

/*
 * load_file
 * loads a file named fln into memory and returns its size into outsz 
 */
byte* load_file(const char* fln, int* outsz)
{
    int flsz = 0;
    FILE* fl = fopen(fln, "rb+");
    byte* result = NULL;

    if(fl)
    {
	fseek(fl, 0, SEEK_END);
	flsz = ftell(fl);
	result = malloc(sizeof(byte)*flsz);
	fseek(fl, 0, SEEK_SET);
	fread(result, sizeof(byte), flsz, fl);
	*outsz = flsz;
	fclose(fl);
    }

    return result;
}

/*
 * write_file
 * writes a file out to name, from the byte array src, of len length
 */
void write_file(const char* name, byte* src, int len)
{
    FILE* result = fopen(name, "wb+");
    int bytes_out = 0;

    bytes_out = fwrite(src, sizeof(byte), len, result);
    if(bytes_out != len)
    {
	fprintf(stderr, "Out bytes did not match length!\n");
    }

    fclose(result);
}

/*
 * xor_bytes
 * Takes the byte array arr of length alen and xors it with the pattern of length plen
 */
void xor_bytes(byte* arr, int alen, byte* pattern, int plen)
{
    byte cur;
    get_next_byte(pattern, plen);
    int i;

    for(i = 0; i < alen; i++)
    {
	cur = get_next_byte(NULL, 0);
	arr[i] ^= cur;
    }
}


/*
 * main
 * xors the input file (the second cli arg) with the pattern (the third cli arg) and determines the output file name
 * based on the cli flag (the first cli arg). ("-e" for output to ".xor", "-u" to strip the ".xor" extension, if any)
 */
int main(int argc, char **argv)
{
    int flsz;
    byte* filemem;
    char* fltmp;

    filemem = load_file(argv[2], &flsz);
    xor_bytes(filemem, flsz, argv[3], strlen(argv[3]));
    
    if(!strcmp(argv[1], "-e"))
    {	
	fltmp = malloc(sizeof(char)*(strlen(argv[2])+5));
	strcpy(fltmp, argv[2]);
	strcat(fltmp, FL_EXT);
	
	write_file(fltmp, filemem, flsz);

	free(fltmp);
    }
    else if(!strcmp(argv[1], "-u"))
    {
	char* p = &argv[2][strlen(argv[2])-4];
	if(!strcmp(p, FL_EXT))
	{
	    fltmp = malloc(sizeof(char)*(strlen(argv[2])+1));
	    strcpy(fltmp, argv[2]);
	    fltmp[strlen(fltmp)-4] = '\0';
	    write_file(fltmp, filemem, flsz);
	    free(fltmp);
	}
	else
	{
	    write_file("unencrypted_file", filemem, flsz);
	}
    }
    else
    {
	puts("Unknown argument, please try again.");
    }
    return 0;
}
