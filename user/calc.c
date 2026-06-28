/* Calculator program
 * Input format: <num> <op> <num>
   Spaces will be delimiters
 * Supports: positive integers as input, +, -, *, /
 * Error when dividing by zero
 */
#include "stdio.h"

void prerror(int error);
int eval(int left, char op, int right, int *error);
static void skipSpaces(const char *buf, int *pos);
static int parseInt(const char *buf, int *pos);

/* main function: calculator
 * line = input buffer
 * op = operator
 * resultChar = result of expression as char array
 * left = left side of operator
 * right = right side of operator
 * result = result of expression as an int
 * error = error flag
 */
int calc_main(){
  char line[64];
  char resultChar[12];
  int left, right, result, error;
  char op;
 
  char *prompt = "Calc booted. Enter equation (e.g. 1+2):\n";
  for(int i = 0; prompt[i] != '\0'; i++) {
    putchar(prompt[i]);
  }
 
  while(1){

    // Read the full line, THEN parse it.
    readline(line, sizeof(line));
 
    int pos = 0;
    error = 0;
 
    // Left operand
    skipSpaces(line, &pos);
    left = parseInt(line, &pos);
 
    // Operator (single char; parseInt stopped right on it)
    skipSpaces(line, &pos);
    op = line[pos];
    if(op != '\0') pos++;
 
    // Right operand
    skipSpaces(line, &pos);
    right = parseInt(line, &pos);
 
    // Evaluate
    result = eval(left, op, right, &error);
    if(error == 1) {
      continue;   // Eval already printed the error message
    }
 
    // Convert and print
    intToChar(result, resultChar);
    for(int i = 0; resultChar[i] != '\0'; i++) {
      putchar(resultChar[i]);
    }
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

static void skipSpaces(const char *buf, int *pos){
  while(buf[*pos] == ' ') (*pos)++;
}

static int parseInt(const char *buf, int *pos){
  int total = 0;
  while(buf[*pos] >= '0' && buf[*pos] <= '9'){
    total = total * 10 + (buf[*pos] - '0');
    (*pos)++;
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


