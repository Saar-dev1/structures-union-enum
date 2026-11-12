// Structures - 1 (Extended): Student Records and Academic Analytics System
// Sorting choice: the sorted view is built on a COPY, so stored order is unchanged.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define NAME   50                 /* 49 characters + '\0' */
#define MAXCAP 1000000UL

typedef struct { int roll; char name[NAME]; int age; float gpa; } Student;
typedef struct { Student *d; size_t n, cap; } List;   /* data, count, capacity */

static List L;

/* Reads a line: 0 ok, 1 too long. On end of input, frees memory and exits. */
static int line(const char *prompt, char *b, size_t sz) {
    printf("%s", prompt); fflush(stdout);
    if (!fgets(b, (int)sz, stdin)) { free(L.d); puts("\nAll memory released."); exit(0); }
    size_t l = strlen(b);
    if (l && b[l - 1] == '\n') { b[l - 1] = 0; return 0; }
    int c, bad = 0;
    while ((c = getchar()) != '\n' && c != EOF) bad = 1;
    return bad;
}

/* Reads a number in [lo, hi]; returns 0 ok, 1 invalid (message printed). */
static int num(const char *prompt, double lo, double hi, int integer, double *out) {
    char b[256], *end;
    int bad = line(prompt, b, sizeof b);
    double v = strtod(b, &end);
    while (*end == ' ' || *end == '\t') end++;
    if (bad || end == b || *end || !(v >= lo && v <= hi) || (integer && v != (long long)v)) {
        printf("Error: expected %s between %g and %g.\n", integer ? "a whole number" : "a number", lo, hi);
        return 1;
    }
    *out = v;
    return 0;
}

/* Reads name, age and GPA into s; returns 1 on invalid input. */
static int ask(Student *s) {
    char b[256]; double v;
    size_t l;
    int bad = line("Name (max 49 chars, spaces ok): ", b, sizeof b);
    l = strlen(b);
    if (bad || !l || l >= NAME || strspn(b, " \t") == l) {
        puts("Error: name must be 1-49 characters and not blank.");
        return 1;
    }
    strcpy(s->name, b);
    if (num("Age (16-100): ", 16, 100, 1, &v)) return 1;
    s->age = (int)v;
    if (num("GPA (0.00-4.00): ", 0, 4, 0, &v)) return 1;
    s->gpa = (float)v;
    return 0;
}

static int find(int roll) {
    for (size_t i = 0; i < L.n; i++) if (L.d[i].roll == roll) return (int)i;
    return -1;
}

static void show(const Student *s) {
    printf("Roll %-6d %-30s Age %-3d GPA %.2f\n", s->roll, s->name, s->age, s->gpa);
}

static int cmp(const void *a, const void *b) {     /* GPA desc, then roll asc */
    const Student *x = a, *y = b;
    if (x->gpa != y->gpa) return x->gpa < y->gpa ? 1 : -1;
    return (x->roll > y->roll) - (x->roll < y->roll);
}

int main(void) {
    double v;
    char b[16];

    for (;;) {                                      /* initial capacity */
        if (num("Initial capacity (1-1000000): ", 1, MAXCAP, 1, &v)) continue;
        if ((size_t)v <= SIZE_MAX / sizeof(Student) && (L.d = malloc((size_t)v * sizeof(Student)))) break;
        puts("Error: could not allocate that much memory.");
    }
    L.cap = (size_t)v;
    printf("Success: space allocated for %zu student(s).\n", L.cap);

    for (;;) {
        puts("\n1 Add  2 Display  3 Search  4 Update  5 Delete  6 GPA summary  7 Sorted  0 Exit");
        line("Choice: ", b, sizeof b);
        int c = atoi(b), i;
        Student s, *t;

        if (c == 0 && b[0] != '0') { puts("Error: enter a menu number."); continue; }
        if (c == 0) break;
        if (c < 0 || c > 7) { puts("Error: unknown option."); continue; }

        if (c == 1 || c == 3 || c == 4 || c == 5) {         /* operations needing a roll number */
            if (num("Roll number: ", 1, 2147483647, 1, &v)) continue;
            s.roll = (int)v;
            i = find(s.roll);
            if (c == 1 && i >= 0) { puts("Error: duplicate roll number."); continue; }
            if (c != 1 && i < 0)  { puts("Error: roll number not found."); continue; }
        }

        switch (c) {
        case 1:
            if (ask(&s)) break;
            if (L.n == L.cap) {                         /* grow with realloc */
                size_t nc = L.cap > MAXCAP / 2 ? MAXCAP : L.cap * 2;
                if (L.n >= MAXCAP || nc > SIZE_MAX / sizeof(Student) ||
                    !(t = realloc(L.d, nc * sizeof(Student)))) {
                    puts("Error: cannot grow the collection; nothing was added.");
                    break;
                }
                L.d = t; L.cap = nc;
                printf("(Array full: capacity grown to %zu.)\n", nc);
            }
            L.d[L.n++] = s;
            puts("Success: student added.");
            break;
        case 2:
            if (!L.n) puts("The collection is empty.");
            for (size_t k = 0; k < L.n; k++) show(&L.d[k]);
            break;
        case 3:
            show(&L.d[i]);
            break;
        case 4:
            if (ask(&s)) break;                         /* s.roll is kept */
            L.d[i] = s;
            puts("Success: student updated.");
            break;
        case 5:
            memmove(&L.d[i], &L.d[i + 1], (L.n - i - 1) * sizeof(Student));
            L.n--;
            puts("Success: student deleted.");
            break;
        case 6: {
            if (!L.n) { puts("The collection is empty."); break; }
            Student *hi = L.d, *lo = L.d;
            double sum = 0;
            for (size_t k = 0; k < L.n; k++) {
                if (L.d[k].gpa > hi->gpa) hi = &L.d[k];
                if (L.d[k].gpa < lo->gpa) lo = &L.d[k];
                sum += L.d[k].gpa;
            }
            printf("Highest: "); show(hi);
            printf("Lowest:  "); show(lo);
            printf("Average GPA: %.2f\n", sum / (double)L.n);
            break;
        }
        case 7:
            if (!L.n) { puts("The collection is empty."); break; }
            if (!(t = malloc(L.n * sizeof(Student)))) { puts("Error: out of memory."); break; }
            memcpy(t, L.d, L.n * sizeof(Student));
            qsort(t, L.n, sizeof(Student), cmp);
            for (size_t k = 0; k < L.n; k++) show(&t[k]);
            free(t);
            break;
        }
    }

    free(L.d);
    puts("All memory released. Goodbye!");
    return 0;
}
