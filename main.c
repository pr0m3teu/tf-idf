#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#define M 250 
#define SPECIAL_CHARS " \n\t\b,.*&#!()[]{}<>~@$%^_+;-.=?\\:\"'"

typedef struct Term_Freq_List Term_Freq_List;
struct Term_Freq_List { 
    size_t freq;
    char* term;
    Term_Freq_List *next;
}; 

typedef struct {
    size_t freq;
    char* term;
} Term_Freq;

typedef struct {
    char*            doc_name;
    Term_Freq_List*  term_freqs[M];
} Doc;

int hash(char* term);
void append_term(Term_Freq_List** ht, char* term);
void print_ht(Term_Freq_List** ht);
void free_ht(Term_Freq_List** ht);

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
        if (entry->d_type == 0x08) {
            Doc document = {0}; 
            char* doc_name = malloc(sizeof(char) * (strlen(dir_name) + strlen(entry->d_name) + 1)); 
            if (doc_name == NULL)
            {
                fprintf(stderr, "ERROR: Could not allocate memory!\n");
                exit(1);
            }

            strcpy(doc_name, dir_name);
            strncat(doc_name, entry->d_name, strlen(entry->d_name));
            
            document.doc_name = doc_name;

            FILE *f = fopen(doc_name, "rb");
            if (f == NULL) {
                fprintf(stderr, "ERROR could not open file %s!\n", doc_name);
                exit(1);
            }
        
            // Find file size
            fseek(f, 0L, SEEK_END);
            long f_size = ftell(f);
            fseek(f, 0L, SEEK_SET);

            char *buff = malloc(sizeof(char) * f_size);
            if (buff == NULL) {
                fprintf(stderr, "ERROR: Could not allocate memory for buffer!\n");
                exit(1);
            }

            while(fgets(buff, f_size, f))
            {
                char *word = strtok(buff, SPECIAL_CHARS);
                while (word != NULL)
                {
                    char *t = word;
                    for(; *t; ++t) *t = tolower(*t);

                    append_term(document.term_freqs, word);
                    word = strtok(NULL, SPECIAL_CHARS);
                }
            }
            
            fclose(f);
            
            free(buff);

            print_ht(document.term_freqs);
            free_ht(document.term_freqs);
            free(document.doc_name);
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
    return sum % M;
}


void append_term(Term_Freq_List** ht, char* term)
{
    const int key = hash(term);
    Term_Freq_List *curr = ht[key]; 
    if (curr == NULL) {
        curr = malloc(sizeof(Term_Freq_List));
        curr->freq = 1;
        curr->term = malloc(sizeof(char) * strlen(term));
        strcpy(curr->term, term);
        curr->next = NULL;
        ht[key] = curr;
    }
    else {
        while(curr->next != NULL)
        {
            if (strcmp(curr->term, term) == 0) {
                curr->freq++;
                return;
            }
            curr = curr->next;

        }
        if (strcmp(curr->term, term) == 0) {
            curr->freq++;
            return;
        }

        curr->next = malloc(sizeof(Term_Freq_List));
        curr->next->freq = 1;
        curr->next->term = malloc(sizeof(char) * strlen(term));
        strcpy(curr->next->term, term);
        curr->next->next = NULL;
    }
}

void free_ht(Term_Freq_List** ht)
{
    for (int i = 0; i < M; ++i)
    {
        if(ht[i] != NULL)
        {
            Term_Freq_List *t, *curr;
            curr = ht[i];
            while(curr->next != NULL)
            {
                t = curr;
                curr = curr->next;
                free(t->term);
                free(t);
            }
            free(curr->term);
            free(curr);
        }
    }
}

void print_ht(Term_Freq_List** ht)
{
    for(int i = 0; i < M; ++i)
    {
        printf("Entry %d:\n", i);
        Term_Freq_List *curr = ht[i];
        while(curr != NULL)
        {
            printf("%s:%zu, ", curr->term, curr->freq);
            curr = curr->next;
        }
        printf("\n");
    }
}
