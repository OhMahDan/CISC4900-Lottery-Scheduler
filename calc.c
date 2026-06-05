/* Calculator program
 * Input format: <num> <op> <num>
   Spaces will be delimiters
 * Supports: positive integers as input, +, -, *, /
 * Error when dividing by zero
 */
// REMOVE AFTER MAKING SYSCALL WRAPPERS
#include <stdio.h>

void prerror(int error);
int charToInt();
int eval(int left, char op, int right, int *error);
void intToChar(int num, char *result);

/* main function: calculator
 * op = operator
 * resultChar = result of expression as char array
 * left = left side of operator
 * right = right side of operator
 * result = result of expression as an int
 * error = error flag
 */
int main(){
  char op;
  char resultChar[12];
  int left, right, result, error = 0;
  while(1){
    // extract left num, op, and right num
    left = charToInt();
    op = getchar();
    getchar();
    right = charToInt();
    // evaluate
    result = eval(left, op, right, &error);
    if(error == 1)
      continue;
    // convert int to char
    intToChar(result, resultChar);
    //print
    for(int i = 0; resultChar[i] != '\0'; i++)
      putchar(resultChar[i]);
    putchar('\n');
  }
  return 0;
}

/* Print error depending on the error type
 * 1 = buffer overflow
 * 2 = division by zero
 * 3 = invalid operator
 * any other = error
 * msg = error message
 * i = index to keep track of *msg
 */
void prerror(int error){
  char *msg;
  int i = 0;
  switch(error){
  case 1:
    msg = "Error: < 100 characters";
    break;
  case 2:
    msg = "Error: Division by 0";
    break;
  case 3:
    msg = "Error: Invalid operator";
    break;
  default:
    msg = "Error";
  }
  while(msg[i] != '\0')
    putchar(msg[i++]);
  putchar('\n');
}

/* charToInt: Converts char to int
 * Input: nothing
 * Output: int value of char
 * Doesn't check for invalid inputs (e.g. a + b)
 */
int charToInt(){
  int total = 0;
  char c = getchar();
  while(c != ' ' && c != '\n'){
    total = total * 10 + (c - '0');
    c = getchar();
  }
  return total;
}

/* eval: Evaluate expression
 * Input: left num, operator, right num, error flag
 * Output: result as int
 * Error: division by zero. Calls perror with error code and set
   error flag to 1
 * Error: invalid operator. Calls perror with error code and set
   error flag to 1
 */
int eval(int left, char op, int right, int *error){
  *error = 0;
  switch(op){
  case '+':
    return left + right;
  case '-':
    return left - right;
  case '*':
    return left * right;
  case '/':
    if(right == 0){
      prerror(2);
      *error = 1;
    }
    else
      return left / right;
    break;
  default:
    prerror(3);
    *error = 1;
  }
  return 0;
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
  while(i >= 0)
    result[j++] = temp[i--];
    result[j] = '\0';
}
