/*
* CSCI3280 Introduction to Multimedia Systems *
* --- Declaration --- *
* I declare that the assignment here submitted is original except for source
* material explicitly acknowledged. I also acknowledge that I am aware of
* University policy and regulations on honesty in academic work, and of the
* disciplinary guidelines and procedures applicable to breaches of such policy
* and regulations, as contained in the website
* http://www.cuhk.edu.hk/policy/academichonesty/ *
* Assignment 3
* Name : ZHAO Mengyuan
* Student ID : 1155141582
* Email Addr : 1155141582@link.cuhk.edu.hk
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define CODE_SIZE 12
#define TRUE 1
#define FALSE 0

typedef struct hf
{
	char *entry = NULL;
} node;

/* function prototypes */
unsigned int read_code(FILE *, unsigned int);
void write_code(FILE *, unsigned int, unsigned int);
void writefileheader(FILE *, char **, int);
void readfileheader(FILE *, char **, int *);
void compress(FILE **, FILE *, int);
void decompress(FILE *, FILE **, int);
node *buildDict();

int main(int argc, char **argv)
{
	int printusage = 0;
	int no_of_file;
	char **input_file_names;
	char *output_file_names;
	FILE *lzw_file;

	if (argc >= 3)
	{
		if (strcmp(argv[1], "-c") == 0)
		{
			/* compression */
			lzw_file = fopen(argv[2], "wb");

			/* write the file header */
			input_file_names = argv + 3;
			no_of_file = argc - 3;
			writefileheader(lzw_file, input_file_names, no_of_file);
			/* ADD CODES HERE */
			FILE **input = (FILE **)malloc(sizeof(FILE *) * no_of_file); //for multiple input files
			for (int i = 0; i < no_of_file; i++)
				input[i] = fopen(argv[i + 3], "r");
			compress(input, lzw_file, no_of_file);
            printf("Success! Compressed:");
			for(int i = 0; i < no_of_file; i++)
			    printf(" %s", argv[i + 3]);
			printf("\n");
			fclose(lzw_file);
		}
		else if (strcmp(argv[1], "-d") == 0)
		{
			/* decompress */
			lzw_file = fopen(argv[2], "rb");

			/* read the file header */
			no_of_file = 0;
			readfileheader(lzw_file, &output_file_names, &no_of_file);
			/* ADD CODES HERE */
			int len = strlen(output_file_names);
			// for multiple input files
			FILE **output = (FILE **)malloc(sizeof(FILE *) * no_of_file);
			int no = 0;
			for (int i = 0; i < no_of_file; i++)
			{
				char *filename = (char *)malloc(sizeof(char) * len);
				for (int j = 0;; j++)
				{
					if (output_file_names[no] == '\n')
					{
						output[i] = fopen(filename, "w");
						free(filename);
						no++;
						break;
					}
					else
					{
						filename[j] = output_file_names[no];
						no++;
					}
				}
			}
			decompress(lzw_file, output, no_of_file);
			printf("Success! Decompressed: %s", argv[2]);
			fclose(lzw_file);
			free(output_file_names);
		}
		else
			printusage = 1;
	}
	else
		printusage = 1;

	if (printusage)
		printf("Usage: %s -<c/d> <lzw filename> <list of files>\n", argv[0]);

	return 0;
}

/*****************************************************************
 *
 * writefileheader() -  write the lzw file header to support multiple files
 *
 ****************************************************************/
void writefileheader(FILE *lzw_file, char **input_file_names, int no_of_files)
{
	int i;
	/* write the file header */
	for (i = 0; i < no_of_files; i++)
	{
		fprintf(lzw_file, "%s\n", input_file_names[i]);
	}
	fputc('\n', lzw_file);
}

/*****************************************************************
 *
 * readfileheader() - read the fileheader from the lzw file
 *
 ****************************************************************/
void readfileheader(FILE *lzw_file, char **output_filenames, int *no_of_files)
{
	int noofchar;
	char c, lastc;

	noofchar = 0;
	lastc = 0;
	*no_of_files = 0;
	/* find where is the end of double newline */
	while ((c = fgetc(lzw_file)) != EOF)
	{
		noofchar++;
		if (c == '\n')
		{
			if (lastc == c)
				/* found double newline */
				break;
			(*no_of_files)++;
		}
		lastc = c;
	}

	if (c == EOF)
	{
		/* problem .... file may have corrupted*/
		*no_of_files = 0;
		return;
	}
	/* allocate memeory for the filenames */
	*output_filenames = (char *)malloc(sizeof(char) * noofchar);
	/* roll back to start */
	fseek(lzw_file, 0, SEEK_SET);

	fread((*output_filenames), 1, (size_t)noofchar, lzw_file);

	return;
}

/*****************************************************************
 *
 * read_code() - reads a specific-size code from the code file
 *
 ****************************************************************/
unsigned int read_code(FILE *input, unsigned int code_size)
{
	unsigned int return_value;
	static int input_bit_count = 0;
	static unsigned long input_bit_buffer = 0L;

	/* The code file is treated as an input bit-stream. Each     */
	/*   character read is stored in input_bit_buffer, which     */
	/*   is 32-bit wide.                                         */

	/* input_bit_count stores the no. of bits left in the buffer */

	while (input_bit_count <= 24)
	{
		input_bit_buffer |= (unsigned long)getc(input) << (24 - input_bit_count);
		input_bit_count += 8;
	}

	return_value = input_bit_buffer >> (32 - code_size);
	input_bit_buffer <<= code_size;
	input_bit_count -= code_size;

	return (return_value);
}

/*****************************************************************
 *
 * write_code() - write a code (of specific length) to the file 
 *
 ****************************************************************/
void write_code(FILE *output, unsigned int code, unsigned int code_size)
{
	static int output_bit_count = 0;
	static unsigned long output_bit_buffer = 0L;

	/* Each output code is first stored in output_bit_buffer,    */
	/*   which is 32-bit wide. Content in output_bit_buffer is   */
	/*   written to the output file in bytes.                    */

	/* output_bit_count stores the no. of bits left              */

	output_bit_buffer |= (unsigned long)code << (32 - code_size - output_bit_count);
	output_bit_count += code_size;

	while (output_bit_count >= 8)
	{
		putc(output_bit_buffer >> 24, output);
		output_bit_buffer <<= 8;
		output_bit_count -= 8;
	}

	/* only < 8 bits left in the buffer                          */
}

/*****************************************************************
 *
 * compress() - compress the source file and output the coded text
 *
 ****************************************************************/
void compress(FILE **input, FILE *output, int num)
{
	/* ADD CODES HERE */
	unsigned int x;
	int length = 1;
	char c;
	char *p = NULL;
	node *dict = buildDict();

	for (int i = 0; i < num; i++)
	{
		while ((c = fgetc(input[i])) != EOF)
		{
			char ch[2];
			ch[0] = c;
			ch[1] = '\0';
			if (p == NULL)
			{
				char a[2];
				a[0] = c;
				a[1] = '\0';
				p = a;
			}
			else
			{
				char *b = (char *)malloc(sizeof(char) * (length + 1));
				strcpy(b, p);
				strcat(b, ch);
				p = b;
			}
			if (strlen(p) == 1)
			{
				length++;
				x = c;
			}
			else
			{
				int len = strlen(p);
				
				//simple hash function
				int fx = 255;
				for (int j = 0; j < len; j++)
					fx = fx + p[j];
				if (fx > 4094)
					while (fx > 4094)
						fx -= 3839;
				
				if (dict[fx].entry != NULL)
				{
					if (strcmp(dict[fx].entry, p) == 0) //found
					{
						x = fx;
						length++;
						continue;
					}
					else
					{
						fx++;
						int flag = 0;
						while (fx < 4095 && dict[fx].entry != NULL)
						{
							if (strcmp(dict[fx].entry, p) == 0)
							{
								length++;
								x = fx;
								flag = 1;
								break;
							}
							fx++;
						}
						if (flag == 1) //found
							continue;
						
						//build a new dictionary
						if (fx > 4094)
						{
							dict = buildDict();
							fx = 255;
							for (int j = 0; j < len; j++)
								fx = fx + p[j];
							if (fx > 4094)
								while (fx > 4094)
									fx -= 3839;
						}
					}
				}
				dict[fx].entry = (char *)malloc(sizeof(char) * length);
				strcpy(dict[fx].entry, p);
				char a[2];
				a[0] = c;
				p = a;
				length = 1;
				write_code(output, x, CODE_SIZE);
				x = c;
				continue;
			}
		}
		write_code(output, x, CODE_SIZE);
		write_code(output, 4095, CODE_SIZE);
		p = NULL;
	}
	write_code(output, 0, CODE_SIZE);
}

/*****************************************************************
 *
 * decompress() - decompress a compressed file to the orig. file
 *
 ****************************************************************/
void decompress(FILE *input, FILE **output, int num)
{
	node *dict = buildDict();
	unsigned int pw, cw;
	char c;

	pw = read_code(input, CODE_SIZE);
	c = dict[pw].entry[0];
	fprintf(output[0], "%c", c);

	for (int i = 0; i < num; i++)
	{
		while ((cw = read_code(input, CODE_SIZE)) != 4095)
		{
			char* s;
			if (dict[cw].entry != NULL) //found
			{
				c = dict[cw].entry[0];
				int len = strlen(dict[cw].entry);
				s = dict[cw].entry;
			}
			else //not found
			{
				c = dict[pw].entry[0];
				int len = strlen(dict[pw].entry);
				s = (char *)malloc(sizeof(char) * (len + 2));
				strcpy(s, dict[pw].entry);
				s[len] = c;
				s[len + 1] = '\0';
			}
			fprintf(output[i], "%s", s);
			int len = strlen(dict[pw].entry);
			len++;
			char* p = (char *)malloc(sizeof(char) * (len + 1));
			strcpy(p, dict[pw].entry);
			p[len - 1] = c;
			p[len]= '\0';
			//same hash function
			int fx = 255;
			for (int j = 0; j < len; j++)
				fx = fx + p[j];
			if (fx > 4094)
				while (fx > 4094)
					fx -= 3839;
			while (dict[fx].entry != NULL && fx < 4095)
				fx++;
			if (fx == 4095) //build a new dictionary
			{
				dict = buildDict();
				int fx = 255;
				for (int j = 0; j < len; j++)
					fx = fx + p[j];
				if (fx > 4094)
					while (fx > 4094)
						fx -= 3839;
			}
			dict[fx].entry = (char *)malloc(sizeof(char) * (len + 1));
			strcpy(dict[fx].entry, p);
			pw = cw;
		}
	}
}

//build a dictionary with 0 - 255 predefined
node *buildDict()
{
	node *res = (node *)malloc(sizeof(node) * 4096);
	for (int i = 0; i < 256; i++)
	{
		char str[2];
		str[0] = i;
		str[1] = '\0';
		res[i].entry = (char *)malloc(sizeof(char) * 2);
		strcpy(res[i].entry, str);
	}
	return res;
}
