#include "string.h"
#include "modes.h"

int strlen(const char *pText)
{
	int len = 0;
	for(; *pText != '\0'; pText++, len++)
		;
	return len;
}

/*******************************************************************************
 * Function Name  : itoa
 * Description    : Convert int to char
 * Input          : int number (signed/unsigned)
 * Return         : pointer to text string
 *******************************************************************************/
void itoa_(int n, char s[])
{
	int i, sign;

	if((sign = n) < 0)
		n = -n;

	i = 0;

	do
	{
		s[i++] = n % 10 + '0';
	} while((n /= 10) > 0);

	if(sign < 0) s[i++] = '-';

	s[i] = '\0';
	reverse(s);
}

#define PRECISION 5
void ftoa_(float num, char str[])
{
	/*
	float num = 2.55f;
	int m = log10(num);
	int digit;
	float tolerance = .0001f;

	while (num > 0 + precision)
	{
		float weight = pow(10.0f, m);
		digit = floor(num / weight);
		num -= (digit*weight);
		*(fstr++)= '0' + digit;
		if (m == 0)
			*(fstr++) = '.';
		m--;
	}
	*(fstr) = '\0';
	*(fstr) */

	uint8_t zeroFlag = 0;

	if(num < 0)
	{
		num = -num;
		zeroFlag = 1;
	}

	int whole_part = num;
	int digit = 0, reminder = 0;
	int log_value = log10_(num), index = log_value;
	long wt = 0;

	// Extract the whole part from float num
	for(int i = 1; i < log_value + 2; i++)
	{
		wt = pow_(10.0, i);
		reminder = whole_part % wt;
		digit = (reminder - digit) / (wt / 10);

		// Store digit in string
		str[index--] = digit + 48; // ASCII value of digit  = digit + 48
		if(index == -1)
			break;
	}

	index = log_value + 1;
	str[index] = '.';
	if(zeroFlag) str[++index] = '-';

	float fraction_part = num - whole_part;
	float tmp1 = fraction_part, tmp = 0;

	// Extract the fraction part from  num
	for(int i = 1; i < PRECISION; i++)
	{
		wt = 10;
		tmp = tmp1 * wt;
		digit = tmp;

		// Store digit in string
		str[++index] = digit + 48; // ASCII value of digit  = digit + 48
		tmp1 = tmp - digit;
	}
	str[++index] = '\0';
}

/*
char * dtoa(char *s, double n) {

		int digit, m, m1;
		char *c = s;
		int neg = (n < 0);
		if (neg)
			n = -n;
		// calculate magnitude
		m = log10_(n);
		int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
		if (neg)
			*(c++) = '-';
		// set up for scientific notation
		if (useExp) {
			if (m < 0)
			   m -= 1.0;
			n = n / pow_(10.0, m);
			m1 = m;
			m = 0;
		}
		if (m < 1.0) {
			m = 0;
		}
		// convert the number
		while (n > PRECISION || m >= 0) {
			double weight = pow_(10.0, m);
			if (weight > 0 && !isinf(weight)) {
				digit = floor(n / weight);
				n -= (digit * weight);
				*(c++) = '0' + digit;
			}
			if (m == 0 && n > 0)
				*(c++) = '.';
			m--;
		}
		if (useExp) {
			// convert the exponent
			int i, j;
			*(c++) = 'e';
			if (m1 > 0) {
				*(c++) = '+';
			} else {
				*(c++) = '-';
				m1 = -m1;
			}
			m = 0;
			while (m1 > 0) {
				*(c++) = '0' + m1 % 10;
				m1 /= 10;
				m++;
			}
			c -= m;
			for (i = 0, j = m-1; i<j; i++, j--) {
				// swap without temporary
				c[i] ^= c[j];
				c[j] ^= c[i];
				c[i] ^= c[j];
			}
			c += m;
		}
		*(c) = '\0';
	return s;
}

float floor_(float n)
{
	return n - (n % 1);
}
*/
float log10_(int v)
{
	return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8
							   : (v >= 10000000)	? 7
							   : (v >= 1000000)		? 6
							   : (v >= 100000)		? 5
							   : (v >= 10000)		? 4
							   : (v >= 1000)		? 3
							   : (v >= 100)			? 2
							   : (v >= 10)			? 1u
													: 0u;
}

/*******************************************************************************
 * Function Name  : reverse
 * Description    : Reverses string
 * Input          : pointer to string
 *******************************************************************************/
void reverse(char s[])
{
	int c, i, j;
	for(i = 0, j = strlen(s) - 1; i < j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/*******************************************************************************
 * Function Name  : strcat
 * Description    : add
 * Input          : pointer to string
 *******************************************************************************/
void strcat_(char first[], char second[])
{
	int i = 0, j = 0;

	while(first[i] != '\0')
		i++;
	while((first[i++] = second[j++]) != '\0')
		;
}
