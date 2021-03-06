// Version

#include <string.h>
#include <stdlib.h>

#include "build_version.h"
#include "version.h"
#include "valueTransfer.h"
#include "printers.h"

version_t version = {VERSION_ID, BUILD_NAME, __DATE__" "__TIME__};

char * getBuildDate(void)
{
	return &version.buildDate[0];
}

char * getBuildName(void)
{
	return &version.buildName[0];
}

Long majorVersion(void) // use last two digits of year
{
	return extractMajorVersion(&version);
}

Long minorVersion(void) // month 1-12
{
	return extractMinorVersion(&version);
}
	
Long buildVersion(void) // return ((day-1)*24 + hour)*60 + minute
{
	return extractBuildVersion(&version);
}

// major, minor, build extraction	
Long extractMajorVersion(version_t *version)
{
	char year[3];
	
	memcpy(year, &version->buildDate[9], 2);
	return strtoul(year, (char **)NULL, 10);
}

Long extractMinorVersion(version_t *version)
{
	if (0 == memcmp(&version->buildDate[0], "Jan", 3)) return 1;
	if (0 == memcmp(&version->buildDate[0], "Feb", 3)) return 2;
	if (0 == memcmp(&version->buildDate[0], "Mar", 3)) return 3;
	if (0 == memcmp(&version->buildDate[0], "Apr", 3)) return 4;
	if (0 == memcmp(&version->buildDate[0], "May", 3)) return 5;
	if (0 == memcmp(&version->buildDate[0], "Jun", 3)) return 6;
	if (0 == memcmp(&version->buildDate[0], "Jul", 3)) return 7;
	if (0 == memcmp(&version->buildDate[0], "Aug", 3)) return 8;
	if (0 == memcmp(&version->buildDate[0], "Sep", 3)) return 9;
	if (0 == memcmp(&version->buildDate[0], "Oct", 3)) return 10;
	if (0 == memcmp(&version->buildDate[0], "Nov", 3)) return 11;
	if (0 == memcmp(&version->buildDate[0], "Dec", 3)) return 12;
	return 0;
}

Long extractBuildVersion(version_t *version)
{
	char day[3] = {0, 0, 0};
	char hour[3] = {0, 0, 0};
	char minute[3] = {0, 0, 0};
	Long build;
	
	memcpy(&day[0], &version->buildDate[4], 2);
	memcpy(&hour[0], &version->buildDate[12], 2);
	memcpy(&minute[0], &version->buildDate[15], 2);

	build = strtoul(day, (char **)NULL, 10);
	build = (build - 1)*24 + strtoul(hour, (char **)NULL, 10);
	return build * 60 + strtoul(minute, (char **)NULL, 10);
}

Long extractVersion(Long address) // address might not be long aligned
{
	version_t *version = (version_t *)address;
	Long mmb = 0;
	
	if (bytesToLong((Byte *)&version->headerTag) == VERSION_ID)
	{
		mmb = extractMajorVersion(version);
		mmb = (mmb<<8) + extractMinorVersion(version);
		mmb = (mmb<<16) + extractBuildVersion(version);
	}
	return mmb;
}

Long getVersion(void)
{
	Long mmb = 0;
	
	mmb = extractMajorVersion(&version);
	mmb = (mmb<<8) + extractMinorVersion(&version);
	mmb = (mmb<<16) + extractBuildVersion(&version);

	return mmb;
}

void showVersion(void)
{
	Short build = (Short)buildVersion();

	print("\n");
	print(BUILD_NAME), print(" ");
	printDec0(majorVersion()), print("."), printDec0(minorVersion());
	print("."), printnHex(0, build);
	print(" "), print(getBuildDate());
}

// Setting prompt using Build name
#define MAX_PROMPT 20
#define PROMPT_COLON ": "

void setPrompt(char * );

void setBuildPrompt(void)
{
	char prompt[MAX_PROMPT+1] = {0};

	strncat(prompt, getBuildName(), MAX_PROMPT - sizeof(PROMPT_COLON));
	strcat(prompt, PROMPT_COLON);
	setPrompt(prompt);
}