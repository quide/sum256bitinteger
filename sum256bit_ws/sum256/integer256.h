#ifndef INTEGER256_H
#define INTEGER256_H

#include <cstring>
#include <math.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

namespace ceiia
{

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

#define NUMBER_OF_PARTS 4		// 4*64 bits = 256 bits
#define MAX_BYTES_PER_PART 8 	// 64 bits = 8 bytes

struct integer256
{
private:
	const unsigned int MAX_HEX_INPUT_SIZE_PER_PART;
	uint64_t  parts[NUMBER_OF_PARTS];
	bool overflow;
public:
	const unsigned int MAX_HEX_INPUT_SIZE;
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
	
	int init_from_string(char* varc, unsigned int base)
	{
		size_t str_size = strlen (varc);
		if(str_size > MAX_HEX_INPUT_SIZE)
		{
			printf("hexadecimal integer %s must be 256 bits max (64 digits). It has %d digits instead!\n", varc, str_size);
			this->invalid = true;
			this->overflow = true;
			return -1;
		}
	
		size_t i;
		int c;
		uint64_t temp ;
		uint64_t dig;
		const unsigned char *digit_value;
		digit_value = CHAR_TO_INT_GLOBAL;
		
		varc += (str_size - 1); // Go from the right to the left of the input string
		
		int j=0;
		while( str_size > 0 ) // check if there is still something to be read for the next integer part
		{
			uint64_t dig_temp = 0;	
			for( i = 0;  i < str_size && i < MAX_HEX_INPUT_SIZE_PER_PART;  ++i )	// Go thru all inserted letters until the max allowed per part (just read one 64 bit part at a time)
			{
				c = (unsigned char) *varc--;				// Read next digit
				dig = digit_value[c];						// convert from string (that may represent eg. hexadecimal) to decimal
				if( dig >= base )							// (protect from invalid digits)
				{
					this->invalid = true;
					return -1;	
				}
				temp = dig * pow(base, i);		// (this intermediate step avoids unkown +1 error that occured for base=16, i=14, dig=15 calcule added to dig_temp=0xFFFFFFFFFFFFFF !!!!!)
				// TODO: measure time for faster alternative, something like: 
				// dig_temp |= dig << i (hexadecimal specific nevertheless)
				dig_temp += temp;					// add this decimal to our 64 bit integer (a part) with its proper importance
			}
			str_size = str_size - i; 	// Reduce what was read from readed size
			this->parts[j] = dig_temp;		// Add this part to our 256 bit integer
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
	
	integer256& operator=(const integer256& b)
	{
		for(int i = 0;  i < NUMBER_OF_PARTS;  ++i)
        {	 this->parts[i] = b.parts[i];	 }
		this->invalid = b.invalid;
		this->overflow = b.overflow;
		return *this;
	}
	
	char* print()	// concatenate parts and form a string
	{
		char* result = (char*)calloc(MAX_HEX_INPUT_SIZE+1, sizeof(char));

		char* result_part[NUMBER_OF_PARTS];
		char* conversion_string = (char*)malloc(7+1);
		sprintf(conversion_string, "%%0%dllx", MAX_HEX_INPUT_SIZE_PER_PART);
		
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

}

#endif // INTEGER256_H
