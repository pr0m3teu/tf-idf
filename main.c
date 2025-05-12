#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define M 150
#define SPECIAL_CHARS " \n\t\b,.*&#!()[]{}<>~@$%^_+;-.=?\\:\"'"

typedef struct Term_Freq Term_Freq;
struct Term_Freq{ 
    size_t freq;
    char* term;
   Term_Freq *next;
}; 

int hash(char* term);
void append_term(Term_Freq** ht, char* term);
void print_ht(Term_Freq** ht);
void free_ht(Term_Freq** ht);

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
            Term_Freq* ht[M] = {0};
    
            char* s = malloc(sizeof(char) * (strlen(dir_name) + strlen(entry->d_name))); 
            strcpy(s, dir_name);
            strncat(s, entry->d_name, strlen(entry->d_name));
            
            FILE *f = fopen(s, "rb");
            if (f == NULL) {
                fprintf(stderr, "ERROR could not open file %s!\n", s);
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
                char *p = strtok(buff, SPECIAL_CHARS);
                while (p != NULL)
                {
                    append_term(ht, p);
                    p = strtok(NULL, SPECIAL_CHARS);
                }
            }
            for(int i = 0; i < M; ++i)
            {
                if(ht[i] != NULL) {
                    printf("YES\n");
                }
            }
            
            fclose(f);
            
            free(buff);
            free(s);

            print_ht(ht);
            free_ht(ht);
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


void append_term(Term_Freq** ht, char* term)
{
    int key = hash(term);
    if (ht[key] == NULL) {
        ht[key] = malloc(sizeof(Term_Freq));
        ht[key]->freq = 1;
        ht[key]->term = malloc(sizeof(char) * strlen(term));
        strcpy(ht[key]->term, term);
        ht[key]->next = NULL;
    }
    else {
        Term_Freq *curr = ht[key]; 
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

        curr->next = malloc(sizeof(Term_Freq));
        curr->next->freq = 1;
        curr->next->term = malloc(sizeof(char) * strlen(term));
        strcpy(curr->next->term, term);
        curr->next->next = NULL;
    }
}

void free_ht(Term_Freq** ht)
{
    for (int i = 0; i < M; ++i)
    {
        if(ht[i] != NULL)
        {
            Term_Freq *t, *curr;
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

void print_ht(Term_Freq** ht)
{
    for(int i = 0; i < M; ++i)
    {
        printf("%d: ", i);
        Term_Freq *curr = ht[i];
        while(curr != NULL)
        {
            printf("%s:%zu, ", curr->term, curr->freq);
            curr = curr->next;
        }
        printf("\n");
    }
}
