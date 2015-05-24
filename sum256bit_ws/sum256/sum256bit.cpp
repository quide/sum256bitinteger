// gcc -Wall -g sum256bit.cpp -lm -o sum256bit

#include <stdio.h>
#include <cstring>
#include <math.h>
#include <inttypes.h>


void help(char** argv)
{
	printf("Usage: %s int_A int_B\n", argv[0]);
	printf(" where int_A and int_B are hexadecimals with maximum 256 bits (64 digits)\n");
}


//#include <assert.h>

#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
/* These can't be used after statements in c89. */
#ifdef __COUNTER__
  #define STATIC_ASSERT(e,m) \
    ;enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }
#else
  /* This can't be used twice on the same line so ensure if using in headers
   * that the headers are not included twice (by wrapping in #ifndef...#endif)
   * Note it doesn't cause an issue when used on same line of separate modules
   * compiled with gcc -combine -fwhole-program.  */
  #define STATIC_ASSERT(e,m) \
    ;enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }
#endif

// ASCII table converted to decimal integer
// 'x' - is an out of scope big value, 120
const unsigned char CHAR_TO_INT_GLOBAL[] =
{
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 'x', 'x', 'x', 'x', 'x', 'x',
  'x',  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
   25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 'x', 'x', 'x', 'x', 'x',
  'x',  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
   25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 'x', 'x', 'x', 'x', 'x',
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x',
  'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'
};

#include <stdlib.h>

#define NUMBER_OF_PARTS 4		// 4*64 bits = 256 bits
#define MAX_BYTES_PER_PART 8 	// 64 bits = 8 bytes

struct integer256
{
	const unsigned int MAX_HEX_INPUT_SIZE_PER_PART;
	const unsigned int MAX_HEX_INPUT_SIZE;
	uint64_t  parts[NUMBER_OF_PARTS];
	bool overflow;
	bool invalid;
    
    integer256() :
		MAX_HEX_INPUT_SIZE_PER_PART(MAX_BYTES_PER_PART*2),
		MAX_HEX_INPUT_SIZE(NUMBER_OF_PARTS*MAX_HEX_INPUT_SIZE_PER_PART),
		overflow(false),
		invalid(false)
    {
        for(int i = 0;  i < NUMBER_OF_PARTS;  ++i)	// auto initialize to 0
        {	 parts[i] = 0;	 }
    }
	
	int init_from_string(char* varc1, unsigned int base)
	{
		size_t str1_size = strlen (varc1);
		size_t i;
		int c;
		uint64_t temp ;
		uint64_t dig;
		const unsigned char *digit_value;
		digit_value = CHAR_TO_INT_GLOBAL;
		
		int j=0;
		while( str1_size > 0 ) // check if there is still something to be read for the next integer part
		{
			uint64_t dig1 = 0;	
			for( i = 0;  i < str1_size && i < MAX_HEX_INPUT_SIZE_PER_PART;  ++i )	// Go thru all inserted letters until the max allowed per part (just read one 64 bit part at a time)
			{
				c = (unsigned char) *varc1++;				// Read next digit
				dig = digit_value[c];						// convert from string (that may represent eg. hexadecimal) to decimal
				if( dig >= base )							// (protect from invalid digits)
				{
					this->invalid = true;
					return -1;	
				}
				temp = dig * pow(base, i);		// (this intermediate step avoids unkown +1 error that occured for base=16, i=14, dig=15 calcule added to dig1=0xFFFFFFFFFFFFFF !!!!!)
				dig1 += temp;					// add this decimal to our 64 bit integer (a part) with its proper importance
			}
			str1_size = str1_size - i; 	// Reduce what was read from readed size
			this->parts[j] = dig1;		// Add this part to our 256 bit integer
			++j;
		}
		
		return 0;
	}
	
	integer256 operator+(const integer256& b)
	{
		integer256 result;
		uint64_t carrier = 0;
		
		if(b.invalid || this->invalid)
		{	result.invalid = true;	}
		else
		{
			for(int i = 0;  i < NUMBER_OF_PARTS;  ++i)
			{
				result.parts[i] = this->parts[i] + b.parts[i] + carrier;	// do the actual sum
				
				if(b.parts[i] > UINTMAX_MAX - carrier ||
					b.parts[i] + carrier > UINTMAX_MAX - this->parts[i]) 	// Overflow case
				{	carrier = 1;	}	// add +1 in the next part sum
				else
				{	carrier = 0;	}
			}
			
			if( carrier == 1 )	// Unhandlable Overflow
			{	result.overflow = true;	}
		}
		
		return result;
	}
	
	char* print()	// concatenate parts and form a string
	{
		char* result = (char*)calloc(MAX_HEX_INPUT_SIZE+1, sizeof(char));

		char* result_part[NUMBER_OF_PARTS];
		
		for(int i = NUMBER_OF_PARTS - 1;   i >= 0 ;   --i)
		{
            result_part[i] = (char*)calloc(MAX_HEX_INPUT_SIZE_PER_PART+1, sizeof(char));
			sprintf(result_part[i], "%016llx", this->parts[i]);		// TODO: remove hardcoded for hexadecimal (create "16" at string "%016llx" from MAX_HEX_INPUT_SIZE_PER_PART)
			strcat(result, result_part[i]);
		}
		
		if( this->overflow == true )
		{	strcat(result, " (overflown)");	}
		
		if( this->invalid == true )
		{	strcat(result, " Invalid value!");	}
		
		return result;
	}
	
	void validate_64_bit_vars()		// Just for system independence validation (TODO)
	{
		uintmax_t teste = 0 ;
		STATIC_ASSERT(sizeof(teste)*8==64, "Compilador não suporta variáveis de 64 bits!");				// Generate compile time error if we don't have 64 bit vars
		//_Static_assert(sizeof(teste)*8==64, "Compilador não suporta variáveis de 64 bits!");
		//static_assert(sizeof(teste)*8==64, "Compilador não suporta variáveis de 64 bits!");
	}
};

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
	//validate_64_bit_vars();

	if(argc != 3)
	{
		help(argv);
		return -1;
	}
	
	char* varc1	= argv[1];
	size_t str1_size = strlen (varc1);	
	char* varc2	= argv[2];
	size_t str2_size = strlen (varc2);
	
	integer256 full_dig1;

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


	full_dig1.init_from_string(varc1, 16);
	
	integer256 full_dig2;
	full_dig2.init_from_string(varc2, 16);
	
	integer256  result = full_dig1 + full_dig2;
	
	//printf("result: %llx\n", result);
	printf( "result: %s\n", result.print() );

	return 0;
}
