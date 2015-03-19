// copy and run application  Robert Chapman  Aug 27, 2012

/*
	this function copies application to its run location and then runs it
	Expects a header at location:
	[ version# | Start | Dest | size | entry | checksum ]
 */

#include "bktypes.h"
#include "fletcher.h"
#include "copyRun.h"
#include "flash_errors.h"
#include "printers.h"

// external
void disableComponents(void);
void runStartup(void);
Long copyApp(Long image); // copy application
Long runApp(Long image);

Long checkHeader(Long image) // check image header
{
	imageHead_t *ih = (imageHead_t *)image;

	if (rangeCheck((Byte *)ih, (Byte *)(ih) + HEADER_SIZE) != FLASH_OK)
		return ERR_OUTOFRANGE;

	if (fletcher32((Byte *)ih, HEADER_SIZE - sizeof(ih->headerChecksum))
		 != ih->headerChecksum)
		return ERR_CHECKSUM;

	return CHECK_OK;
}

Long checkImage(Long image) // check image and header
{
	imageHead_t *ih = (imageHead_t *)image;
	Byte *src, *dst;
	Long size;
	Long result = checkHeader(image);
	
	if (result != CHECK_OK)
		return result;
	
	src = (Byte *)ih->start;
	dst = (Byte *)ih->dest;
	size = ih->size;
	
	if (rangeCheck(src, src+size) != FLASH_OK)
		return ERR_OUTOFRANGE;

	if (fletcher32(src, size) != ih->checksum)
		return ERR_CHECKSUM;
	
	return CHECK_OK;
}

Long copyApp(Long image) // copy application
{
	imageHead_t *ih = (imageHead_t *)image;
	Byte *src, *dst;
	Long size;
	Long result = checkImage(image);
	
	if (result != CHECK_OK)
		return result;

	src = (Byte *)ih->start;
	dst = (Byte *)ih->dest;
	size = ih->size;

	if (dst != src) // only copy if different
		while (size--)
		{
			*dst = *src;
			if (*dst++ != *src++) // make sure copy worked
				return ERR_COPY;
		}

	return CHECK_OK;
}

Long runApp(Long image)
{
	imageHead_t *ih = (imageHead_t *)image;
	vector function = (vector)ih->entry;
	
	disableComponents();
	function(); // should not return
	runStartup();  // restart image otherwise
	return CHECK_OK;
}

char *copyRunErrors[] = {"CHECK_OK ", "ERR_OUTOFRANGE ", "ERR_CHECKSUM ", "ERR_COPY "};

static void printError(Long err);
static void printError(Long err)
{
	print("\ncopy and run error: ");
	printDec(err);
	print(", "); 
	print(copyRunErrors[err]);
}

Long copyRun(Long image)
{
	Long result;
	
	result = copyApp(image);
	if (result == CHECK_OK)
		result = runApp(image);
	
	if (result)
		printError(result);

	return result;
}

// cli
#include "printers.h"
#include "botkernl.h"

void copyRun_cmd(void);
void runApp_cmd(void);
void copyApp_cmd(void);
void checkHeader_cmd(void);
void checkImage_cmd(void);
void dumpHeader_cmd(void);

void copyRun_cmd(void)
{
	copyRun(*sp++);
}

void copyApp_cmd(void)
{
	sp[0] = copyApp(sp[0]);
}

void runApp_cmd(void)
{
	sp[0] = runApp(sp[0]);
}

void checkHeader_cmd(void)
{
	printDec(checkHeader(*sp++));
}

void checkImage_cmd(void)
{
	printDec(checkImage(*sp++));
}

void dumpHeader_cmd(void)
{
	imageHead_t *ih = (imageHead_t *)*sp++;
	print("\nversion: "), printDec0(ih->mmb.mmb.major), print("."), printDec0(ih->mmb.mmb.minor), print("."), printDec0(ih->mmb.mmb.build);
	print("\nstart: "), printHex(ih->start);
	print("\ndest: "), printHex(ih->dest);
	print("\nsize: "), printDec(ih->size);
	print("\nentry: "), printHex(ih->entry);
	print("\nchecksum: "), printHex(ih->checksum);
	print("\nheadersize: "), printDec(ih->headerSize);
	print("\nrelease date: "), print(ih->releaseDate);
	print("\nappName: "), print(ih->appName);
	print("\nheader checksum: "), printHex(ih->headerChecksum);
}

void fletcher32_cmd(void);
void fletcher32_cmd(void)
{
	Long len = *sp++;
	Byte *address = (Byte *)*sp;
	
	*sp = fletcher32(address, len);
}

void fwdb_cmd(void);
void fwdb_cmd(void)
{
	lit(&fwdb);
}