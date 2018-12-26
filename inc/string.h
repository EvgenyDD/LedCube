/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STRING_H
#define STRING_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int strlen(char *);
void itoa_(int n, char s[]);
void ftoa_(float num, char str[]);
void reverse(char s[]);
void strcat_(char first[], char second[]);
float log10_(int v);


#endif //STRING_H
