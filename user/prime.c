#include <stdio.h>

// Determine if n is prime. A prime must have TWO distinct divisors, itself and 1.
int is_prime(int n){

    // Simple cases.
    if(n <= 1) return 0;                                // 0 and 1 are not prime.
    if(n <= 3) return 1;                                // 2 and 3 are prime.
    if(n % 2 == 0 || n % 3 == 0) return 0;              // Anything divisible by 2 (even) or 3 above the number 4 will not be prime.

    // Also check other odd divisors, but we only need to check up to sqrt(n).
    for(int i = 5; i * i <= n; i += 6){
        if(n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

void main() {
    int n = 2;

    while(1) {
        if(is_prime(n)){
            putchar(n);
        }
    }
}