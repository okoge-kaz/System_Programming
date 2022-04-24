#include <stdio.h>

#if defined(__GNU_LIBRARY__)

#define TYPE "glibc"
#include <stdio_ext.h>
int is_unbuffered(FILE *fp) { return __fbufsize(fp) == 0; }
int is_linebuffered(FILE *fp) { return __flbf(fp) != 0; }
int buffer_size(FILE *fp) { return __fbufsize(fp); }

#elif defined(_IO_UNBUFFERED)

#define TYPE "IO_UNBUFFERED"
int is_unbuffered(FILE *fp) { return (fp->_flags & _IO_UNBUFFERED) != 0; }
int is_linebuffered(FILE *fp) { return (fp->_flags & _IO_LINE_BUF) != 0; }
int buffer_size(FILE *fp) { return fp->_IO_buf_end - fp->_IO_buf_base; }

#elif defined(__SNBF)

#define TYPE "SNBF"
int is_unbuffered(FILE *fp) { return (fp->_flags & __SNBF) != 0; }
int is_linebuffered(FILE *fp) { return (fp->_flags & __SLBF) != 0; }
int buffer_size(FILE *fp) { return fp->_bf._size; }

#elif defined(_IONBF)

#ifdef _LP64
  #define TYPE "IONBF_LP64"
  #define _flag __pad[4]
  #define _ptr __pad[1]
  #define _base __pad[2]
#else
  #define TYPE "IONBF"
#endif

int is_stdio_unbuffered(FILE *fp) { return (fp->_flag & _IONBF) != 0; }
int is_linebuffered(FILE *fp) { return (fp->_flag & _IOLBF) != 0; }
int buffer_size(FILE *fp) {
#ifdef _LP64
    return fp->_base - fp->_ptr;
#else
    return BUFSIZ; // expectation
#endif
}

#else

#error unknown stdio implementation!

#endif

void check(FILE *fp) {
    printf("buffer size: %d\n", buffer_size(fp));
    printf("line buffered: %s\n", is_linebuffered(fp) ? "yes" : "no");
    printf("unbuffered: %s\n", is_unbuffered(fp) ? "yes" : "no");
}

int main(int argc, char **argv) {
    char buf[256];
    fread(buf, 1, 1, stdin);
    printf("type: %s\n", TYPE);
    printf("[stdin]\n");
    check(stdin);
    printf("[stdout]\n");
    check(stdout);
    printf("[stderr]\n");
    check(stderr);
}
