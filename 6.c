
#include <stdio.h>
#include <string.h>
#include <stddef.h>

typedef union {
    int i;
    float f;
    char str[20];
} UnionData;

typedef struct {
    int i;
    float f;
    char str[20];
} StructData;

typedef enum {
    VALUE_INT, VALUE_FLOAT, VALUE_TEXT
} ValueType;

typedef struct {
    UnionData data;
    ValueType type;
} TaggedValue;

// Set integer
void setInt(TaggedValue *r, int n) {
    r->data.i = n;
    r->type = VALUE_INT;
}

// Set float
void setFloat(TaggedValue *r, float n) {
    r->data.f = n;
    r->type = VALUE_FLOAT;
}

// Set string
int setText(TaggedValue *r, const char *s) {
    if (!r || !s || strlen(s) >= 20)
        return 0;

    strcpy(r->data.str, s);
    r->type = VALUE_TEXT;
    return 1;
}

// Display active member
void display(TaggedValue r) {
    switch (r.type) {
        case VALUE_INT:
            printf("Integer: %d\n", r.data.i);
            break;
        case VALUE_FLOAT:
            printf("Float: %.2f\n", r.data.f);
            break;
        case VALUE_TEXT:
            printf("String: %s\n", r.data.str);
            break;
        default:
            printf("Invalid tag\n");
    }
}

int main(void) {
    UnionData u;
    StructData s;

    // Basic demonstration
    s.i = 10;
    s.f = 220.5f;
    strcpy(s.str, "Struct Programming");

    printf("Structure: %d %.2f %s\n", s.i, s.f, s.str);

    u.i = 10;
    printf("Union Integer: %d\n", u.i);

    strcpy(u.str, "Union Programming");
    printf("Union String: %s\n", u.str);

    u.f = 220.5f;
    printf("Union Float: %.2f\n", u.f);

    // Size and alignment
    printf("\nSizes:\n");
    printf("Union: %zu\n", sizeof(u));
    printf("Structure: %zu\n", sizeof(s));
    printf("Tagged Union: %zu\n", sizeof(TaggedValue));

    printf("Alignments: %zu %zu\n",
           _Alignof(UnionData), _Alignof(StructData));

    // Addresses
    printf("\nUnion Addresses:\n");
    printf("%p %p %p\n",
           (void *)&u.i, (void *)&u.f, (void *)&u.str);

    printf("\nStructure Addresses:\n");
    printf("%p %p %p\n",
           (void *)&s.i, (void *)&s.f, (void *)&s.str);

    // Structure offsets
    printf("\nOffsets:\n");
    printf("%zu %zu %zu\n",
           offsetof(StructData, i),
           offsetof(StructData, f),
           offsetof(StructData, str));

    // Memory comparison
    printf("\nMemory for 100 records:\n");
    printf("Union: %zu bytes\n", 100 * sizeof(UnionData));
    printf("Structure: %zu bytes\n", 100 * sizeof(StructData));

    // Five tagged records
    TaggedValue r[5];

    setInt(&r[0], 10);
    setFloat(&r[1], 220.5f);
    setText(&r[2], "Union Programming");
    setInt(&r[3], 500);
    setText(&r[4], "C Language");

    printf("\nTagged Records:\n");
    for (int i = 0; i < 5; i++)
        display(r[i]);

    // Edge cases
    printf("\nEdge Tests:\n");

    printf("19 characters: %s\n",
        setText(&r[0], "1234567890123456789")
        ? "Accepted" : "Rejected");

    printf("20 characters: %s\n",
        setText(&r[0], "12345678901234567890")
        ? "Accepted" : "Rejected");

    // Change data type
    setFloat(&r[0], 12.5f);
    display(r[0]);

    // Invalid tag
    r[0].type = (ValueType)999;
    display(r[0]);

    printf("\nObservation:\n");
    printf("Union members share memory.\n");
    printf("Structure members have separate memory.\n");
    printf("Padding may affect memory size.\n");
    printf("Enum identifies the active union member.\n");

    return 0;
}
