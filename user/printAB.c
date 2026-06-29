extern void putchar(char c);

static void print(const char *s) {
    while (*s) putchar(*s++);
}

void printA() {
    while (1) {
        print("A");
    }
}

void printB() {
    while (1) {
        print("B");
    }
}
