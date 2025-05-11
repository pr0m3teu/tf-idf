#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "lexer.h"

#define M 50

typedef struct { 
    size_t freq;
    char* term;
} Term_Freq;

typedef struct Term_Freqs Term_Freqs;
struct Term_Freqs {
   Term_Freq tf;
   Term_Freqs *next;
};

int hash(char* term);
void append_term(Term_Freqs** ht, char* term);

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: No Folder provided!\n");
        fprintf(stderr, "USAGE: ./main [FOLDER]\n");
        exit(1);
    }

    struct dirent* entry;
    char* dir_name = argv[1];
    DIR* dr = opendir(dir_name);

    if (dr == NULL)
    {
        fprintf(stderr, "ERROR: could not open folder!\n");
        exit(1);
    }
    
    while((entry = readdir(dr)) != NULL)
    {
        if (entry->d_type == 0x08){
            printf("FOUND TEXT FILE %s\n", entry->d_name);
            List ptrs = {0};
            Lexemes terms = {0};

            char* s = strcat(dir_name, entry->d_name); 
            FILE *f = fopen(s, "rb");
            if (f == NULL) {
                fprintf(stderr, "ERROR could not open file %s!\n", s);
            }
            
            lex_file(&ptrs, &terms, f, (const char *)s);
            
            lfree_all(&ptrs);
            fclose(f);
        }
    }

    closedir(dr);
    return 0;
}

int hash(char* term)
{
    int sum = 0; 
    for (size_t i = 0; i < strlen(term); ++i)
    {
        sum += term[i];
    }
    return sum;
}


void append_term(Term_Freqs** ht, char* term)
{
    int key = hash(term);
    if (ht[key] == NULL)
    {
        ht[key] = malloc(sizeof(Term_Freqs));
        ht[key]->tf.freq = 1;
        strcpy(ht[key]->tf.term, term);
    }
}
