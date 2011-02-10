#ifdef	LINUX
#include	<stdio.h>
#include	<stdlib.h>
#include    <string.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<openssl/sha.h>
#include	"include/file.h"

char *
SHA_File(const char *filename, char *buf)
{
	SHA_CTX c;
	struct stat finfo;
	unsigned char *buffer;
	int fd;
	int i;
	int j;
	char asciisum[41];
    char hashdigit[3];
    
	if(lstat(filename, &finfo) == -1)
		return NULL;

	fd = open(filename, O_RDONLY);
	if(fd < 0)
		return NULL;
	buffer = mmap(
	    NULL, finfo.st_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0
	);
	if(buffer == NULL)
		return NULL;
	SHA1_Init(&c);
	SHA1_Update(&c, buffer, finfo.st_size);
	SHA1_Final((unsigned char *)buf, &c);
	munmap(buffer, finfo.st_size);
	close(fd);
	memset(asciisum, 0, 41);
	for(i = 0, j = 0; j < 20; i += 2, j++)
	{
	    snprintf(hashdigit, 2, "%.2x", (unsigned char)buf[j]);
	    strncat(asciisum, hashdigit, 2);
	}
	snprintf(buf, 40, "%s", asciisum);
	return buf;
}
#endif
