// Structures - 2 (Extended): Rectangle Inventory and Geometry Analysis
// Limits: 1-1000 rectangles; each dimension must be finite, in [1e-6, 1e9]
// (the upper bound keeps width * height safely inside float range).
// Square test: |width - height| <= 1e-4 * max(width, height)  (relative tolerance).
// Sorted view is built on a COPY, so the stored order never changes.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define MAXN 1000
#define EPS  1e-4f
#define SHOW(r) printf("ID %-6d Width %-10.2f Height %-10.2f Area %-12.2f %s\n", (r)->id, (r)->width, \
                       (r)->height, (r)->area, square(r) ? "Square" : "Non-square")

typedef struct { int id; float width, height, area; } Rectangle;
Rectangle *d; size_t n, cap;                  /* records, count, capacity */

void calculate_area(Rectangle *r) { r->area = r->width * r->height; }

/* malloc/realloc for k rectangles, with overflow check (p == NULL acts as malloc) */
void *alloc(void *p, size_t k) { return k > SIZE_MAX / sizeof(Rectangle) ? NULL : realloc(p, k * sizeof(Rectangle)); }

/* Reads a line; returns 1 if it was too long. On end of input: free memory and exit. */
int line(const char *p, char *b, int sz) {
    int c, bad = 0;
    printf("%s", p); fflush(stdout);
    if (!fgets(b, sz, stdin)) { free(d); puts("\nAll memory released."); exit(0); }
    size_t l = strcspn(b, "\n");
    if (b[l]) b[l] = 0; else while ((c = getchar()) != '\n' && c != EOF) bad = 1;
    return bad;
}

/* Reads a number in [lo, hi] (NaN and infinity fail); nonzero (after a message) if invalid. */
int num(const char *p, double lo, double hi, int whole, double *o) {
    char b[99], *e;
    int bad = line(p, b, sizeof b);
    double v = strtod(b, &e);
    while (*e == ' ') e++;
    if (bad || e == b || *e || !(v >= lo && v <= hi) || (whole && v != (long long)v))
        return printf("Error: enter %s from %g to %g.\n", whole ? "a whole number" : "a number", lo, hi);
    *o = v;
    return 0;
}

int square(const Rectangle *r) {
    float x = r->width - r->height, m = r->width > r->height ? r->width : r->height;
    return (x < 0 ? -x : x) <= EPS * m;
}

int find(int id) {
    for (size_t k = 0; k < n; k++) if (d[k].id == id) return (int)k;
    return -1;
}

/* Reads width and height into r and computes the area; nonzero if invalid. */
int dims(Rectangle *r) {
    double v;
    if (num("Width: ", 1e-6, 1e9, 0, &v)) return 1;
    r->width = (float)v;
    if (num("Height: ", 1e-6, 1e9, 0, &v)) return 1;
    r->height = (float)v;
    calculate_area(r);
    return 0;
}

/* Reads a new rectangle and appends it, growing the array if full. */
int add(void) {
    Rectangle r, *t; double v;
    if (num("Rectangle ID: ", 1, 2147483647, 1, &v)) return 1;
    r.id = (int)v;
    if (find(r.id) >= 0) return puts("Error: duplicate ID.");
    if (dims(&r)) return 1;
    if (n == cap) {                                         /* full: grow with realloc */
        size_t m = cap * 2 > MAXN ? MAXN : cap * 2;
        if (n >= MAXN || !(t = alloc(d, m))) return puts("Error: cannot grow; nothing added.");
        d = t; cap = m;
        printf("(Array full: capacity grown to %zu.)\n", m);
    }
    d[n++] = r;
    puts("Success: rectangle added.");
    return 0;
}

int cmp(const void *a, const void *b) {                     /* area ascending, then ID ascending */
    const Rectangle *x = a, *y = b;
    if (x->area != y->area) return x->area > y->area ? 1 : -1;
    return (x->id > y->id) - (x->id < y->id);
}

int main(void) {
    double v; char b[16]; Rectangle r, *t; size_t k;

    while (!cap) {
        if (num("Number of rectangles (1-1000): ", 1, MAXN, 1, &v)) continue;
        if ((d = alloc(NULL, (size_t)v))) cap = (size_t)v; else puts("Error: cannot allocate that much memory.");
    }
    printf("Success: space allocated for %zu rectangle(s).\n", cap);
    for (size_t want = cap; n < want; ) { printf("Rectangle %zu of %zu\n", n + 1, want); add(); }

    for (;;) {
        puts("\n1 Add  2 Display  3 Search  4 Update  5 Delete  6 Area stats  7 Sorted by area  8 Filter by area  0 Exit");
        line("Choice: ", b, sizeof b);
        if (!b[0] || b[1] || !strchr("012345678", b[0])) { puts("Error: enter 0-8."); continue; }
        int c = b[0] - '0', i = -1;
        if (!c) break;
        if (!n && strchr("2345678", b[0])) { puts("The collection is empty."); continue; }

        if (strchr("345", b[0])) {                          /* needs an existing ID */
            if (num("Rectangle ID: ", 1, 2147483647, 1, &v)) continue;
            if ((i = find((int)v)) < 0) { puts("Error: rectangle ID not found."); continue; }
        }

        switch (c) {
        case 1: add(); break;
        case 2: case 7:
            t = d;
            if (c == 7) {
                if (!(t = alloc(NULL, n))) { puts("Error: out of memory."); break; }
                memcpy(t, d, n * sizeof *d);
                qsort(t, n, sizeof *t, cmp);
            }
            for (k = 0; k < n; k++) SHOW(&t[k]);
            if (c == 7) free(t);
            break;
        case 3: SHOW(&d[i]); break;
        case 4:                                             /* edit a copy so fields stay consistent */
            r = d[i];
            if (!dims(&r)) { d[i] = r; puts("Success: rectangle updated."); }
            break;
        case 5:
            memmove(&d[i], &d[i + 1], (n - i - 1) * sizeof *d);
            n--;
            puts("Success: rectangle deleted.");
            break;
        case 6: {
            Rectangle *hi = d, *lo = d; double sum = 0;
            for (k = 0; k < n; k++) {
                if (d[k].area > hi->area) hi = &d[k];
                if (d[k].area < lo->area) lo = &d[k];
                sum += d[k].area;
            }
            printf("Largest:  "); SHOW(hi);
            printf("Smallest: "); SHOW(lo);
            printf("Total area: %.2f   Average area: %.2f\n", sum, sum / n);
            break;
        }
        case 8:
            if (num("Show rectangles with area greater than: ", 0, 1e18, 0, &v)) break;
            for (i = 0, k = 0; k < n; k++) if (d[k].area > v) { SHOW(&d[k]); i++; }
            if (!i) puts("No rectangle has an area above that threshold.");
            break;
        }
    }
    free(d);
    puts("All memory released. Goodbye!");
    return 0;
}
