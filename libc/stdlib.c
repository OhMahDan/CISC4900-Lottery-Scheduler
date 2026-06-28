#include "syscall.h"

void yield(void) {
    asm volatile ("int $0x80" : : "a"(SYS_YIELD));
}

/* intToChar: converts int to char
 * Input: num, result (array that holds int result as chars)
 * Output: returns nothing, but char array should be updated
 * temp = temp char array to store int to char conversion
   Note: temp stores the int in reverse
 * i = index for temp
 * j = index for result
 * neg = negative flag
 */
void intToChar(int num, char *result){
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
  while(i >= 0) {
    result[j++] = temp[i--];
  }
  result[j] = '\0';
}