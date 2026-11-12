
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum { ADD, DELETE, VIEW, UPDATE, COMPLETE, FIND, HELP, EXIT, UNKNOWN } Command;
typedef enum { PENDING, DONE } Status;

typedef struct {
    int id;
    char desc[100];
    Status status;
} Task;

Task *t = NULL;
int n = 0, cap = 0;

int find(int id) {
    for (int i = 0; i < n; i++)
        if (t[i].id == id) return i;
    return -1;
}

int readLine(char *s, int size) {
    if (!fgets(s, size, stdin)) return 0;
    char *p = strchr(s, '\n');
    if (p) *p = '\0';
    else {
        int c = getchar();
        if (c != '\n' && c != EOF) {
            while ((c = getchar()) != '\n' && c != EOF);
            return -1;
        }
    }
    return 1;
}

int readID(int *id) {
    char s[30], extra;
    long x;

    printf("Enter ID: ");
    if (readLine(s, sizeof(s)) != 1 ||
        sscanf(s, "%ld %c", &x, &extra) != 1 ||
        x <= 0 || x > 2147483647L) {
        printf("Invalid ID!\n");
        return 0;
    }
    *id = (int)x;
    return 1;
}

void show(int i) {
    printf("%d | %s | %s\n", t[i].id, t[i].desc,
           t[i].status == DONE ? "DONE" : "PENDING");
}

void add(void) {
    int id;
    char s[100];

    if (!readID(&id)) return;
    if (find(id) != -1) {
        printf("Duplicate ID!\n");
        return;
    }

    printf("Description: ");
    if (readLine(s, sizeof(s)) != 1 || !s[0]) {
        printf("Invalid description!\n");
        return;
    }

    if (n == cap) {
        int c = cap ? cap * 2 : 4;
        Task *p = realloc(t, c * sizeof(Task));
        if (!p) {
            printf("Memory error!\n");
            return;
        }
        t = p;
        cap = c;
    }

    t[n++] = (Task){id, "", PENDING};
    strcpy(t[n-1].desc, s);
    printf("Task %d added!\n", id);
}

void modify(Command cmd) {
    int id;
    if (!readID(&id)) return;

    int i = find(id);
    if (i == -1) {
        printf("Task not found!\n");
        return;
    }

    if (cmd == DELETE) {
        for (int j = i; j < n-1; j++) t[j] = t[j+1];
        n--;
        printf("Task %d deleted!\n", id);
    }
    else if (cmd == COMPLETE) {
        t[i].status = DONE;
        printf("Task %d completed!\n", id);
    }
    else {
        char s[100];
        printf("New description: ");
        if (readLine(s, sizeof(s)) != 1 || !s[0]) {
            printf("Invalid description!\n");
            return;
        }
        strcpy(t[i].desc, s);
        printf("Task %d updated!\n", id);
    }
}

int main(void) {
    char s[30];
    Command cmd;

    while (1) {
        printf("\nADD DELETE VIEW UPDATE COMPLETE FIND HELP EXIT\nCommand: ");

        int r = readLine(s, sizeof(s));
        if (r == 0) break;
        if (r == -1) {
            printf("Command too long!\n");
            continue;
        }

        for (int i = 0; s[i]; i++)
            s[i] = (char)toupper((unsigned char)s[i]);

        if (!strcmp(s,"ADD")) cmd = ADD;
        else if (!strcmp(s,"DELETE")) cmd = DELETE;
        else if (!strcmp(s,"VIEW")) cmd = VIEW;
        else if (!strcmp(s,"UPDATE")) cmd = UPDATE;
        else if (!strcmp(s,"COMPLETE")) cmd = COMPLETE;
        else if (!strcmp(s,"FIND")) cmd = FIND;
        else if (!strcmp(s,"HELP")) cmd = HELP;
        else if (!strcmp(s,"EXIT")) cmd = EXIT;
        else cmd = UNKNOWN;

        switch (cmd) {
            case ADD: add(); break;
            case DELETE:
            case UPDATE:
            case COMPLETE: modify(cmd); break;

            case VIEW:
                if (!n) printf("No tasks!\n");
                for (int i = 0; i < n; i++) show(i);
                break;

            case FIND: {
                int id;
                if (readID(&id)) {
                    int i = find(id);
                    if (i == -1) printf("Not found!\n");
                    else show(i);
                }
                break;
            }

            case HELP:
                printf("ADD DELETE VIEW UPDATE COMPLETE FIND HELP EXIT\n");
                break;

            case EXIT:
                free(t);
                printf("Exiting!\n");
                return 0;

            default:
                printf("Unknown command!\n");
        }
    }

    free(t);
    return 0;
}
