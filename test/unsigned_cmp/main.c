#include <stdio.h>
#include <stdint.h>

#define assert(cond) {\
    if(cond) printf(#cond": yes\n");\
    else printf(#cond": no\n");}


#define UL 0xFFFFFFFFFFFFFFFFUL
#define U  0xFFFFFFFFFFFFFFFFU
#define L  0xFFFFFFFFFFFFFFFFL
#define SU  0xFFFFFFFFFFFFFFFFL
#define S  0xFFFFFFFFFFFFFFFF

int main() {
    uint64_t a = 0xFFFFFFFFFFFFFFFF;
    int64_t b = 0x0;
    int64_t c = 0xFFFFFFFFFFFFFFFF;

    assert(a < b);
    assert(c < b);
    assert(UL < b);
    assert(L < b);
    assert(U < b);
    assert(SU < b);
    assert(S < b);

    return 0;
}