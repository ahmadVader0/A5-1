#include <cstdio>
int main() {
    int zeros = 0, ones = 0;
    for (int a = 0; a <= 1; a++)
    for (int b = 0; b <= 1; b++)
    for (int c = 0; c <= 1; c++)
    for (int d = 0; d <= 1; d++) {
        int out = a ^ b ^ (c & d);
        if (out) ones++; else zeros++;
    }
    printf("f = a^b^(c&d): Zeros: %d, Ones: %d\n", zeros, ones);

    zeros = 0; ones = 0;
    for (int a = 0; a <= 1; a++)
    for (int b = 0; b <= 1; b++)
    for (int c = 0; c <= 1; c++)
    for (int d = 0; d <= 1; d++) {
        int out = (a & b) ^ (c & d) ^ a ^ d;
        if (out) ones++; else zeros++;
    }
    printf("f = (a&b)^(c&d)^a^d: Zeros: %d, Ones: %d\n", zeros, ones);
    return 0;
}
