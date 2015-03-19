// Printers header file for printer functions in a library  Robert Chapman III

void print(const char *message);
void printHex(unsigned int hex);
void printnHex(unsigned int n, unsigned int hex);
void printDec(unsigned int dec);
void printDec0(unsigned int dec);
void printBin(unsigned int bin);
void printChar(unsigned char ch);
void printHex2(unsigned int hex);
void flush(void);

#ifndef PRINTERS_H
#define PRINTERS_H
#define printHex4(hex) printHex2((unsigned int)(hex>>8)), printHex2((unsigned int)hex)
#endif