#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stdint.h>
#include <stddef.h>

/*
 * ISOC99 7.20 General Utilities 
 */

/*
 * 7.20.2 div types 
 */
typedef struct {
    int quot, rem;
} div_t;

typedef struct {
    long quot, rem;
} ldiv_t;

typedef struct {
    long long quot, rem;
} lldiv_t;

typedef struct {
    intmax_t quot, rem;
} imaxdiv_t;

static inline div_t
div(int x, int y) {
    div_t res;

    res.quot = x / y;
    res.rem = x % y;

    return res;
}

static inline ldiv_t
ldiv(long x, long y) {
    ldiv_t res;

    res.quot = x / y;
    res.rem = x % y;

    return res;
}

static inline lldiv_t
lldiv(long long x, long long y) {
    lldiv_t res;

    res.quot = x / y;
    res.rem = x % y;

    return res;
}

static inline imaxdiv_t
imaxdiv(intmax_t x, intmax_t y) {
    imaxdiv_t res;

    res.quot = x / y;
    res.rem = x % y;

    return res;
}

/* 7.20.3 EXIT_ macros */
#define EXIT_FAILURE    1
#define EXIT_SUCCESS    0

#define RAND_MAX        INT_MAX
#define MB_CUR_MAX      1

/* 7.20.1 Numeric conversion functions */

#ifndef CONFIG_WITHOUT_FLOATING
/* 7.20.1-3 The strtod, strtof and strtold functions */
double strtod(const char *s, char **endp);
float strtof(const char *s, char **endp);
#ifndef CONFIG_WITHOUT_LONG_DOUBLE
long double strtold(const char *s, char **endp);
#endif
#endif

/* 7.20.1-4 The strtol, stroll, stroul, strtoull functions */
long strtol(const char *s, char **endp, int base);
long long strtoll(const char *s, char **endp, int base);
intmax_t strtoimax(const char *s, char **endp, int base);
unsigned long strtoul(const char *s, char **endp, int base);
unsigned long long strtoull(const char *s, char **endp, int base);
uintmax_t strtoumax(const char *s, char **endp, int base);

/* 7.20.1-1 atof function */
#ifndef CONFIG_WITHOUT_FLOATING
static inline double
atof(const char *nptr)
{
    return strtod(nptr, (char **)NULL);
}
#endif

/* 7.20.1-2 The atoi, atol and atoll functions */
#include <stdio.h>
static inline int
atoi(const char *nptr)
{
    return (int)strtol(nptr, (char **)NULL, 10);
}

static inline long
atol(const char *nptr)
{
    return strtol(nptr, (char **)NULL, 10);
}

static inline long long
atoll(const char *nptr)
{
    return strtoll(nptr, (char **)NULL, 10);
}

/* 7.20.2 Pseudo-random sequence generation functions */

int rand(void);
int rand_r(unsigned *seed);
void srand(unsigned int seed);

/* 7.20.3 Memory management functions */

void *malloc(size_t);
void free(void *);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);

/* 7.20.4 Communcation with the environment */

void abort(void);
int atexit(void (*func) (void));
void exit(int status);
void _Exit(int status);
int system(const char *string);
char *getenv(const char *name);
int unsetenv(const char *name);
int setenv(const char *envname, const char *envval, int overwrite);

/* 7.20.5 Searching and sortin utilities */
void *bsearch(const void *key, const void *base, size_t nmemb, size_t,
              int (*compar) (const void *, const void *));
void qsort(void *base, size_t nmemb, size_t,
           int (*compar) (const void *, const void *));

/* 7.20.6 Integer arithmetic function */

/*
 * FIXME: (benjl) Gcc defines these, but if we aren't using gcc it probably
 * won't, but how do we know? Or maybe we should compile with -fnobuiltin? 
 */

int abs(int);
long labs(long);
long long llabs(long long);
intmax_t imaxabs(intmax_t);

/* Include POSIX symbols if in POSIX environment */
#ifdef __USE_POSIX
#include <posix/stdlib.h>
#endif

#if 0
static inline int
abs(int x)
{
    return x < 0 ? -x : x;
}

static inline long
labs(long x)
{
    return x < 0 ? -x : x;
}

static inline long long
llabs(long long x)
{
    return x < 0 ? -x : x;
}

static inline intmax_t
imaxabs(intmax_t x)
{
    return x < 0 ? -x : x;
}
#endif
/* 7.20.7 Multibyte/wide character conversion functions */
#if 0                           /* We don't have wide characters */
int mblen(const char *s, size_t n);
int mbtowc(wchar_t pwc, const char *s, size_t n);
int wctomb(char *s, wchat_t wc);
#endif

/* 7.20.8 Multibyte/wide string conversion functions */
#if 0                           /* We don't have wide characters */
size_t mbstowcs(wchar_t *pwcs, const char *s, size_t n);
size_t wcstombs(char *s, constwchat_t * pwcs, size_t n);
#endif

#endif /* _STDLIB_H_ */
