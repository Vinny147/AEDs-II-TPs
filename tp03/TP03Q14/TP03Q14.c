#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#define _CRT_SECURE_NO_WARNINGS

struct Keywords {
    char** member;
    int length;
};

typedef struct {
    char name[100];
    char oTitle[100];
    char releaseDate[20];
    int runtime;
    char genre[200];
    char oLang[25];
    char status[25];
    float budget;
    struct Keywords keywords;
} Movie;

float moneyToFloat(char* str) {
    float result = 0.00f;
    if (strcmp("-", str) != 0) {
        char* aux = malloc(strlen(str) + 1);
        int count = 0;
        for (int i = 0; i < strlen(str); i++)
            if (str[i] != '$' && str[i] != ',')
                aux[count++] = str[i];
        aux[count] = 0;
        result = atof(aux);
        free(aux);
    }
    return result;
}

char* removeDataName(char* str, char* dataName) {
    int count = 0;
    char* newStr = malloc(strlen(str) + 1);
    for (int i = strlen(dataName); i < strlen(str); i++)
        newStr[count++] = str[i];
    newStr[count] = 0;
    return newStr;
}

int stringToMin(char* str) {
    int result = 0, n = strlen(str);
    if (n < 4) {
        if (str[1] == 'h') result += 60 * (str[0] - '0');
        else {
            char* aux = malloc(sizeof(str));
            strcpy(aux, str);
            strncat(str, aux, n - 2);
            result += atoi(str);
        }
    }
    else {
        result += 60 * (str[0] - '0');
        char* aux = malloc(n - 4);
        for (int i = 3, j = 0; i < n - 1; i++)
            aux[j++] = str[i];
        result += atoi(aux);
    }
    return result;
}

char* removeHtmlEntities(char* str) {
    char* newStr = malloc(strlen(str) + 1);
    bool isHtmlEntities = false;
    int count = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '&') isHtmlEntities = true;
        else if (str[i] == ';') isHtmlEntities = false;
        if (!isHtmlEntities && str[i] != '&' && str[i] != ';')
            newStr[count++] = str[i];
    }
    newStr[count] = 0;
    return newStr;
}

char* trimSpace(char* str) {
    char* aux;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    aux = str + strlen(str) - 1;
    while (aux > str && isspace((unsigned char)*aux)) aux--;
    aux[1] = '\0';
    return str;
}

char* removeTags(char* str) {
    char* newStr = malloc(strlen(str) + 1);
    bool isTag = false;
    int count = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '<') isTag = true;
        else if (str[i] == '>') isTag = false;
        if (!isTag && str[i] != '<' && str[i] != '>' && str[i] != '\n')
            newStr[count++] = str[i];
    }
    newStr[count] = 0;
    return newStr;
}

struct Keywords emptyKeywords() {
    struct Keywords keywords;
    keywords.member = malloc(sizeof(char*));
    keywords.member[0] = malloc(2);
    strcpy(keywords.member[0], "");
    keywords.length = 0;
    return keywords;
}

Movie emptyMovie() {
    Movie m;
    strcpy(m.name, "");
    strcpy(m.oTitle, "");
    strcpy(m.releaseDate, "");
    m.runtime = 0;
    strcpy(m.genre, "");
    strcpy(m.oLang, "");
    strcpy(m.status, "");
    m.budget = 0.00f;
    return m;
}

struct Keywords parseKeywords(FILE* f, char* line) {
    for (int i = 0; i < 2; i++) fgets(line, 1000, f);
    struct Keywords keywords = emptyKeywords();
    if (strcmp(trimSpace(removeTags(line)), "Nenhuma palavra-chave foi adicionada.") != 0) {
        char** aux = malloc(sizeof(char*) * 30);
        for (int i = 0; i < 30; i++) aux[i] = malloc(100);
        while (strstr(line, "</ul>") == 0) {
            if (strstr(line, "<li>")) {
                strcpy(aux[keywords.length], removeTags(trimSpace(line)));
                if (strcmp(aux[keywords.length], "") != 0) keywords.length++;
            }
            fgets(line, 1000, f);
        }
        keywords.member = malloc(sizeof(char*) * keywords.length);
        for (int i = 0; i < keywords.length; i++) {
            keywords.member[i] = malloc(strlen(aux[i]) + 1);
            strcpy(keywords.member[i], aux[i]);
        }
        free(aux);
    }
    return keywords;
}

Movie readMovie(char fileName[100]) {
    Movie m = emptyMovie();
    char* filePath;
    filePath = malloc(sizeof(char) * 1000);
    strcpy(filePath, "tmp/filmes/");
    strcat(filePath, fileName);
    if (strchr(filePath, 13)) {
        filePath[strlen(filePath) - 1] = 0;
    }
    FILE* f = fopen(filePath, "r");
    if (f == NULL) {
        char* aux = malloc(1000);
        strcpy(aux, "/");
        strcat(aux, filePath);
        f = fopen(aux, "r");
    }
    char* line = malloc(sizeof(char) * 1000);
    while (fgets(line, 1000, f) != NULL) {
        if (strstr(line, "h2 class")) {
            fgets(line, 1000, f);
            strcpy(m.name, trimSpace(removeHtmlEntities(removeTags(line))));
            strcpy(m.oTitle, m.name);
        } else if (strstr(line, "\"release\"")) {
            fgets(line, 1000, f);
            strncpy(m.releaseDate, trimSpace(line), 10);
            m.releaseDate[10] = 0;
        } else if (strstr(line, "\"genres\"")) {
            for (int i = 0; i < 2; i++) fgets(line, 1000, f);
            strcpy(m.genre, trimSpace(removeHtmlEntities(removeTags(line))));
        } else if (strstr(line, "\"runtime\"")) {
            for (int i = 0; i < 2; i++) fgets(line, 1000, f);
            m.runtime = stringToMin(trimSpace(line));
        } else if (strstr(line, "Título original")) {
            strcpy(m.oTitle, trimSpace(removeDataName(trimSpace(removeTags(line)), "Título original")));
        } else if (strstr(line, "Situação")) {
            strcpy(m.status, trimSpace(removeDataName(trimSpace(removeTags(line)), "Situação")));
        } else if (strstr(line, "Idioma original")) {
            strcpy(m.oLang, trimSpace(removeDataName(trimSpace(removeTags(line)), "Idioma original")));
        } else if (strstr(line, "Orçamento")) {
            m.budget = moneyToFloat(trimSpace(removeDataName(trimSpace(removeTags(line)), "Orçamento")));
        } else if (strstr(line, "Palavras-chave")) {
            m.keywords = parseKeywords(f, line);
        }
    }
    fclose(f);
    free(filePath);
    free(line);
    return m;
}

void printMovie(Movie* m) {
    printf("%s %s %s %d %s %s %s %.0e [", m->name, m->oTitle, m->releaseDate, m->runtime, m->genre, m->oLang, m->status, m->budget);
    for (int i = 0; i < m->keywords.length - 1; i++) {
        printf("%s, ", m->keywords.member[i]);
    }
    if (m->keywords.length > 0) printf("%s]\n", m->keywords.member[m->keywords.length - 1]);
    else printf("]\n");
}

int isFIM(char s[1000]) {
    return (strlen(s) <= 4 && s[0] == 'F' && s[1] == 'I' && s[2] == 'M');
}

// Author: Pedro Henrique Lopes Costa
char* substring(char* string, int position, int length) {
    char* p;
    int c;
    p = malloc(length + 1);
    if (p == NULL) {
        printf("Unable to allocate memory.\n");
        exit(1);
    }
    for (c = 0; c < length; c++) {
        *(p + c) = *(string + position - 1);
        string++;
    }
    *(p + c) = '\0';
    return p;
}

/*===============================================================================================================================*/
//Generic Linked Stack Implementation

typedef struct Cell {
    void* item;
    struct Cell* next;
} Cell;

Cell* newCell(size_t itemSize) {
    Cell* temp = malloc(sizeof(Cell));
    temp->item = malloc(itemSize);
    temp->next = NULL;
    return temp;
}

typedef struct LinkedStack {
    struct Cell* top;
    int size;
    long numComp, numMov;
} LinkedStack;

LinkedStack* newLinkedStack(size_t itemTypeSize) {
    LinkedStack* temp = malloc(sizeof(LinkedStack));
    temp->top = newCell(itemTypeSize);
    temp->size = 0;
    temp->numComp = temp->numMov = 0;
    return temp;
}

void lStack_push(LinkedStack* ls, void* src, size_t srcSize) {
    if (ls->size == 0) {
        ls->top->item = src;
    } else {
        Cell* tmp = newCell(srcSize);
        memcpy(tmp->item, src, srcSize);
        tmp->next = ls->top;
        ls->top = tmp;
    }
    ls->size++;
}

void* lStack_pop(LinkedStack* ls) {
    void* tmp = NULL;
    if (ls != NULL && ls->size > 0) {
        tmp = ls->top->item;
        ls->top = ls->top->next;
        ls->size--;
    }
    return tmp;
}

// Custom print function casting item data to Movie Class
void mlStack_printMovie(Cell* c, int i, int size) {
    if (c != NULL) {
        mlStack_printMovie(c->next, ++i, size);
        printf("[%d] ", ((i - size) * -1));
        printMovie((Movie*)(c->item));
    }
}

/*===============================================================================================================================*/
// Driver Code

int main() {
    //Read first movies input
    char** s = malloc(sizeof(char*) * 100);
    int count = 0;
    for (int i = 0; i < 200; i++) {
        s[i] = malloc(sizeof(char) * 1000);
        fgets(s[i], 1000, stdin);
        s[i][strlen(s[i]) - 1] = 0;
        if (isFIM(s[i])) break;
        count++;
    }
    LinkedStack* mlStack = newLinkedStack(sizeof(Movie));
    Movie* m = malloc(sizeof(Movie));
    for (int i = 0; i < count; i++) {
        *m = readMovie(s[i]);
        lStack_push(mlStack, m, sizeof(Movie));
    }
    free(s);
    memset(m, 0, sizeof(Movie));
    //Read next instroctions
    char* aux = malloc(sizeof(char) * 100);
    count = atoi(fgets(aux, 100, stdin));
    for (int i = 0; i < count; i++) {
        fgets(aux, 100, stdin);
        aux[strlen(aux) - 1] = 0;
        if (strstr(aux, "I ")) {
            *m = readMovie(substring(aux, 3, strlen(aux)));
            lStack_push(mlStack, m, sizeof(Movie));
        }
        else {
            printf("(R) %s\n", ((Movie*)lStack_pop(mlStack))->name);
        }
    }
    free(aux);
    free(m);
    mlStack_printMovie(mlStack->top, 0, mlStack->size);

    return 0;
}
