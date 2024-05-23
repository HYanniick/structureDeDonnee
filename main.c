#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define ALPHABET_SIZE 26
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    bool isEndOfWord;
    char phoneNumber[20];
    char email[50];
} TrieNode;

TrieNode *getNode(void) {
    TrieNode *pNode = (TrieNode *)malloc(sizeof(TrieNode));
    pNode->isEndOfWord = false;

    for (int i = 0; i < ALPHABET_SIZE; i++)
        pNode->children[i] = NULL;

    return pNode;
}

void insert(TrieNode *root, const char *name, const char *phoneNumber, const char *email) {
    int level;
    size_t length = strlen(name);
    int index;

    TrieNode *pCrawl = root;

    for (level = 0; level < length; level++) {
        index = CHAR_TO_INDEX(tolower(name[level])); // Ici je convertis le caractère en minuscule
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();

        pCrawl = pCrawl->children[index];
    }

    pCrawl->isEndOfWord = true;
    pCrawl->phoneNumber[0] = '0'; // Ici je rajoute un 0 devant le numéro de téléphone puisque le numéro dans le fichier csv ne contient pas de 0
    strcpy(pCrawl->phoneNumber + 1, phoneNumber); // Et je copie le numéro de téléphone à partir de la deuxième case du tableau
    strcpy(pCrawl->email, email);
}

void delete(TrieNode *root, const char *name) {
    int level;
    size_t length = strlen(name);
    int index;

    TrieNode *pCrawl = root;

    for (level = 0; level < length; level++) {
        index = CHAR_TO_INDEX(tolower(name[level]));

        if (!pCrawl->children[index]) {
            printf("Name not found for : %s\n", name);
            return;
        }

        pCrawl = pCrawl->children[index];
    }

    if (pCrawl != NULL && pCrawl->isEndOfWord) {
        pCrawl->isEndOfWord = false;
        printf("Name deleted: %s\n\n", name);
    } else {
        printf("Name not found for : %s\n\n", name);
    }
}

bool search(TrieNode *root, const char *name) {
    int level;
    size_t length = strlen(name);
    int index;
    TrieNode *pCrawl = root;

    for (level = 0; level < length; level++) {
        index = CHAR_TO_INDEX(tolower(name[level]));

        if (!pCrawl->children[index]) {
            printf("Name not found for : %s\n", name);
            return false;
        }

        pCrawl = pCrawl->children[index];
    }

    if (pCrawl != NULL && pCrawl->isEndOfWord) {
        printf("Searching name: %s\n", name);
        printf("Result:\n");
        printf("        Phone Number: %s\n", pCrawl->phoneNumber);
        printf("        Email: %s\n\n", pCrawl->email);
        return true;
    }

    printf("Name not found for : %s\n", name);
    return false;
}

void display(TrieNode *root, char str[], int level) {
    if (root->isEndOfWord) {
        str[level] = '\0';
        printf("Name: %s\n", str);
        printf("Phone Number: %s\n", root->phoneNumber);
        printf("Email: %s\n\n", root->email);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) {
            str[level] = i + 'a';
            display(root->children[i], str, level + 1);
        }
    }
}

void load_from_file(TrieNode *root, const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", fileName);
        return;
    }

    char name[50];
    char phoneNumber[20];
    char email[50];
    bool isFirstLine = true;

    while (fscanf(file, "%[^;];%[^;];%s\n", name, phoneNumber, email) != EOF) {
        if (isFirstLine) {
            if ((unsigned char)name[0] == 0xEF && (unsigned char)name[1] == 0xBB && (unsigned char)name[2] == 0xBF) {
                memmove(name, name + 3, strlen(name + 3) + 1);
            }
            isFirstLine = false;
        }

        insert(root, name, phoneNumber, email);
    }

    fclose(file);
}

void freeTrie(TrieNode *root) {
    if (root == NULL) return;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freeTrie(root->children[i]);
    }

    free(root);
}

int main(void) {
    TrieNode *root = getNode();
    char str[50];

    load_from_file(root, "..\\annuaire.csv");
    search(root, "Mehdi");
    search(root, "Steven");
    insert(root, "Dylan", "0612345678", "dylan@gmail.com");
    search(root, "Dylan");
    display(root, str, 0);
    delete(root, "Steven");
    search(root, "Steven");

    freeTrie(root);

    return 0;
}