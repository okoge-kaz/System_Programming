#include <stdio.h>

int add(int a, int b) {
    return a + b;
}
int multiply(int a, int b) {
    return a * b;
}

int main(void) {
    // 関数ポインターの宣言
    int (*func_ptr)(int, int);
    int a, b;
    printf("Enter two numbers: ");
    scanf("%d %d", &a, &b);

    printf("Enter 1 for addition, 2 for multiplication: ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        func_ptr = add;
    } else {
        func_ptr = multiply;
    }
    printf("%d\n", func_ptr(a, b));
    return 0;
}