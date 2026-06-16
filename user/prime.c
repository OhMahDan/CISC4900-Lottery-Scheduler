#include "../drivers/vga.h"

// Copied itoa from calc.c for easy VGA printing.
void itoa(int num, char *result){
  char temp[12];
  int i = 0, j = 0, neg = 0;
  // check if num is 0
  if(num == 0){
    result[0] = '0';
    result[1] = '\0';
    return;
  }
  // check if num is negative and set flag if it is
  if(num < 0){
    neg = 1;
    num = num * -1;
  }
  // extract each digit in num and convert to char
  while(num > 0){
    temp[i++] = (num % 10) + '0';
    num = num / 10;
  }
  temp[i--] = '\0';
  // add '-' if negative
  if(neg)
    result[j++] = '-';
  // copy and reverse temp to result 
  while(i >= 0)
    result[j++] = temp[i--];
    result[j] = '\0';
}

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

void calculate_primes() {
    int count = 0;
    int num = 2;
    char buffer[32];

    terminal_writestring("Starting Prime Generator...\n");

    // Find the first 50 primes for a quick test
    while (count < 50) {
        int is_prime = 1;
        for (int i = 2; i * i <= num; i++) {
            if (num % i == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime) {
            itoa(num, buffer);
            terminal_writestring(buffer);
            terminal_writestring(" ");
            count++;
        }
        num++;
    }
    terminal_writestring("\nDone.\n");
}