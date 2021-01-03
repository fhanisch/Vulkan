#ifndef DEFS_H
#define DEFS_H

#ifdef LOG
static char buf[1048];
#define PRINT(...) \
	sprintf(buf, __VA_ARGS__); \
	fwrite(buf, strlen(buf), 1, logfile);
#else
#define PRINT(...) \
	printf(__VA_ARGS__);
#endif

#endif // !DEFS_H