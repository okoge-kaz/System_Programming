#include <stdio.h>
int main() {
    char buf[4096], i = 0;
    while (fgets(buf, sizeof(buf), stdin) && (i++ < 3)) {
        printf("%s", buf);
    }
    return 0;
}