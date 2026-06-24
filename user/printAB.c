extern void putchar(char c);
extern void yield(void);

static void print(const char *s) {
    while (*s) putchar(*s++);
}

void printA() {
    while (1) {
        print("A");
        yield();
    }
}

void printB() {
    while (1) {
        print("B");
        yield();
    }
}