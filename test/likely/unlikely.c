#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

void foo();
void bar();

int main(char *argv[], int argc)
{
   int a = 3;
   int b;
   if (unlikely (argc == 2)) {
      foo();
      a++;
   }
   else {
      bar();
      b = a;
   }
}