
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TYPE_EMPTY, INT, FLOAT, CHAR, STRING } DataType;

typedef union {
    int i;
    float f;
    char c;
    char *s;
} Data;

typedef struct {
    Data data;
    DataType type;
} Container;

// Initialize container
void init(Container *c) {
    c->type = TYPE_EMPTY;
}

// Clear container
void clear(Container *c) {
    if (!c) return;
    if (c->type == STRING)
        free(c->data.s);
    c->type = TYPE_EMPTY;
}

// Set values
void setInt(Container *c, int n) {
    clear(c);
    c->data.i = n;
    c->type = INT;
}

void setFloat(Container *c, float n) {
    clear(c);
    c->data.f = n;
    c->type = FLOAT;
}

void setChar(Container *c, char n) {
    clear(c);
    c->data.c = n;
    c->type = CHAR;
}

int setString(Container *c, const char *s) {
    if (!c || !s) return 0;

    char *p = malloc(strlen(s) + 1);
    if (!p) return 0;

    strcpy(p, s);
    clear(c);
    c->data.s = p;
    c->type = STRING;
    return 1;
}

// Type-safe getters
int getInt(const Container *c, int *n) {
    if (!c || !n || c->type != INT) return 0;
    *n = c->data.i;
    return 1;
}

int getFloat(const Container *c, float *n) {
    if (!c || !n || c->type != FLOAT) return 0;
    *n = c->data.f;
    return 1;
}

int getChar(const Container *c, char *n) {
    if (!c || !n || c->type != CHAR) return 0;
    *n = c->data.c;
    return 1;
}

int getString(const Container *c, const char **s) {
    if (!c || !s || c->type != STRING) return 0;
    *s = c->data.s;
    return 1;
}

// Get type name
const char *typeName(DataType t) {
    const char *names[] = {"EMPTY", "INT", "FLOAT", "CHAR", "STRING"};
    return t <= STRING ? names[t] : "INVALID";
}

// Display container
void print(const Container *c) {
    printf("%s: ", typeName(c->type));

    switch (c->type) {
        case INT:    printf("%d\n", c->data.i); break;
        case FLOAT:  printf("%.2f\n", c->data.f); break;
        case CHAR:   printf("%c\n", c->data.c); break;
        case STRING: printf("%s\n", c->data.s); break;
        default:     printf("Empty\n");
    }
}

// Deep copy
int copy(Container *dest, const Container *src) {
    if (!dest || !src) return 0;
    if (dest == src) return 1;

    char *p = NULL;

    if (src->type == STRING) {
        p = malloc(strlen(src->data.s) + 1);
        if (!p) return 0;
        strcpy(p, src->data.s);
    }

    clear(dest);
    dest->type = src->type;

    if (src->type == STRING)
        dest->data.s = p;
    else
        dest->data = src->data;

    return 1;
}

int main(void) {
    Container c[6];

    for (int i = 0; i < 6; i++)
        init(&c[i]);

    // Store six different values
    setInt(&c[0], 10);
    setFloat(&c[1], 3.14f);
    setChar(&c[2], 'A');
    setString(&c[3], "Hello");
    setInt(&c[4], 0);
    setString(&c[5], "Data");

    // Display all records
    for (int i = 0; i < 6; i++) {
        printf("%d. ", i + 1);
        print(&c[i]);
    }

    // Deep copy
    copy(&c[0], &c[5]);
    clear(&c[5]);

    printf("\nCopied value: ");
    print(&c[0]);

    // Type checking
    int n;
    if (!getInt(&c[0], &n))
        printf("Wrong type getter!\n");

    // Type transitions
    setString(&c[0], "First");
    setInt(&c[0], 100);
    setString(&c[0], "Second");
    setFloat(&c[0], 5.5f);

    printf("\nAfter changing types:\n");
    print(&c[0]);

    // Empty container
    clear(&c[0]);
    print(&c[0]);

    // Repeated cleanup is safe
    clear(&c[0]);

    // Clear all containers
    for (int i = 0; i < 6; i++)
        clear(&c[i]);

    return 0;
}
