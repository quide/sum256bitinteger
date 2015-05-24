// gcc -Wall -g sum256bit.cpp -lm -o sum256bit


#include "integer256.h"

void help(char** argv)
{
	printf("Usage: %s int_A int_B\n", argv[0]);
	printf(" where int_A and int_B are hexadecimals with maximum 256 bits (64 digits)\n");
}


void tests()
{
	uintmax_t teste = UINTMAX_MAX;
	printf("Max unsigned integer is %d bits with value %llx\n", sizeof(teste)*8, UINTMAX_MAX);
	printf("Min unsigned integer is %d bits with value %llx\n", sizeof(teste)*8, (long long unsigned int)(0));

	intmax_t teste2 = INTMAX_MAX;
	printf("Max integer is %d bits with value %llx\n", sizeof(teste2)*8, INTMAX_MAX);
	printf("Min integer is %d bits with value %llx\n", sizeof(teste2)*8, INTMAX_MIN);
	//uint64_t teste2 = 0;
	//int64_t teste4 = 0;
}

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		help(argv);
		return -1;
	}
	
	char* varc1	= argv[1];
	size_t str1_size = strlen (varc1);	
	char* varc2	= argv[2];
	size_t str2_size = strlen (varc2);
	
	ceiia::integer256 full_dig1;

	if(str1_size > full_dig1.MAX_HEX_INPUT_SIZE)
	{
		printf("First hexadecimal integer must be 256 bits max (64 digits). It has %d!\n", str1_size);
		help(argv); 
		return -1;
	}
	if(str2_size > full_dig1.MAX_HEX_INPUT_SIZE)
	{
		printf("Second hexadecimal integer must be 256 bits max (64 digits). It has %d!\n", str2_size);
		help(argv); 
		return -1;
	}

	unsigned int base = 16;
	full_dig1.init_from_string(varc1, base);
	if(full_dig1.invalid)
	{
		printf("First integer has invalid digits for base %d!\n", base);
		help(argv); 
		return -1;
	}
	
	ceiia::integer256 full_dig2;
	full_dig2.init_from_string(varc2, base);
	if(full_dig2.invalid)
	{
		printf("Second integer has invalid digits for base %d!\n", base);
		help(argv); 
		return -1;
	}
	
	ceiia::integer256  result = full_dig1 + full_dig2;
	
	//printf("result: %llx\n", result);
	printf( "result: %s\n", result.print() );

	return 0;
}
