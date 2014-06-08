#ifndef _STDIO_H_
#define _STDIO_H_

#include <stddef.h>
#include <stdarg.h>
//#include <sys/types.h>
//#include <sys/types.h>

/* Stream structure. */
struct __file;
typedef struct __file FILE;
typedef long fpos_t;

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define BUFSIZ 256
#define EOF (-1)

#define FOPEN_MAX 37
#define FILENAME_MAX 37
#define L_tmpnam 37

#ifndef SEEK_CUR
#define SEEK_CUR 0
#endif
#ifndef SEEK_END
#define SEEK_END 1
#endif
#ifndef SEEK_SET
#define SEEK_SET 2
#endif

#define TMP_MAX 37

extern FILE *stderr;
extern FILE *stdin;
extern FILE *stdout;

/* 7.19.4 Operations on files */
int remove(const char *);
int rename(const char *, const char *);
FILE *tmpfile(void);
char *tmpnam(char *);
int fclose(FILE *);
FILE *fdopen(int , const char *);
int fflush(FILE *);
int fileno(FILE *);
FILE *fopen(const char *, const char *);
FILE *freopen(const char *, const char *, FILE *);
void setbuf(FILE *, char *);
int setvbuf(FILE *, char *, int, size_t);

/* 7.19.6 Format i/o functions */
int fprintf(FILE *, const char *, ...);
int fscanf(FILE *, const char *, ...);
int printf(const char *, ...);
int scanf(const char *, ...);
int snprintf(char *, size_t, const char *, ...);
int sprintf(char *, const char *, ...);
int sscanf(const char *, const char *, ...);
int vfprintf(FILE *, const char *, va_list);
int vfscanf(FILE *, const char *, va_list);
int vprintf(const char *, va_list);
int vscanf(const char *, va_list);
int vsnprintf(char *, size_t, const char *, va_list);
int vasprintf(char **ret, const char *format, va_list ap);
int vsprintf(char *, const char *format, va_list arg);
int vsscanf(const char *s, const char *format, va_list arg);

/* 7.19.7 Character i/o functions */
int fgetc(FILE *);
char *fgets(char *, int, FILE *);
int fputc(int, FILE *);
int fputs(const char *, FILE *);

/*
 * getc is specified to be the same as fgetc, so it makes the most sense to
 * implement as a macro here 
 */
#define getc fgetc /*int getc(FILE *); */ /* lint -function(getc, fgetc) */

int getchar(void);
char *gets(char *);

/*
 * putc is specified to be the same as fputc, so it makes the most sense to
 * implement as a macro here 
 */
#define putc fputc /*int fetc(int, FILE *); */ /* lint -function(putc, fputc) */

int putchar(int);
int puts(const char *);
int ungetc(int, FILE *);

/* 7.19.8 Direct I/O functions */
size_t fread(void *, size_t, size_t, FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);

/* 7.19.9 File positioning functions */
int fgetpos(FILE *, fpos_t *);
int fseek(FILE *, long int, int);
//int fseeko(FILE *stream, off_t offset, int whence);
int fsetpos(FILE *, const fpos_t *);
long int ftell(FILE *);
//off_t ftello(FILE *stream);
void rewind(FILE *);

/* 7.19.10 Error-handling functions */
void clearerr(FILE *);
int feof(FILE *);
int ferror(FILE *);
void perror(const char *);

/* Include POSIX type defs if in POSIX environment */
#ifdef __USE_POSIX
#include <posix/stdio.h>
#endif

#endif /* _STDIO_H_ */
