#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include "tfidf.h"


// ------------------------------------------- DA
void da_append(da* arr, Doc* doc)
{
    if (arr->capacity <= arr->size)
    {
       arr->items = realloc(arr->items, sizeof(Doc*) * arr->capacity * 1.5); 
       arr->capacity *= 1.5;
    }

    arr->items[arr->size] = doc;
    arr->size++;
}


void da_init(da* arr)
{
   arr->capacity = 100;
   arr->size = 0;
   arr->items = malloc(sizeof(Doc) * arr->capacity);
}

// ------------------------------------------- DA


int hash(const char* term)
{
    int sum = 0; 
    for (size_t i = 0; i < strlen(term); ++i)
    {
        sum += (unsigned char) term[i];
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
        curr->term = malloc(sizeof(char) * (strlen(term) + 1));
        strncpy(curr->term, term, strlen(term) + 1);
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
        curr->next->term = malloc(sizeof(char) * (strlen(term) + 1));
        strncpy(curr->next->term, term, strlen(term) + 1);
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
                t->term = NULL;
                free(t);
            }
            free(curr->term);
            curr->term = NULL;
            free(curr);
            curr = NULL;
        }
    }
}


void print_ht(Term_Freq_List** ht)
{
    for(int i = 0; i < M; ++i)
    {
        printf("Entry %d: ", i);
        Term_Freq_List *curr = ht[i];
        while(curr != NULL)
        {
            printf("%s:%zu, ", curr->term, curr->freq);
            curr = curr->next;
        }
        printf("\n");
    }
    printf("\n");
}


void free_doc(Doc* doc)
{
    free_ht(doc->term_freqs);
    free(doc->term_freqs);
    doc->term_freqs = NULL;

    free(doc->doc_name);
    doc->doc_name = NULL;
    free(doc);
}


bool search_ht(Term_Freq_List** ht, const char* item)
{
    int key = hash(item);
    Term_Freq_List* entry= ht[key];

    if (entry == NULL) return false;

    while(entry != NULL)
    {
        if (strcmp(entry->term, item) == 0) return true;
        entry = entry->next;
    }
    return false;
}


int get_freq(Term_Freq_List** ht, const char* term)
{
    const int key = hash(term);
    Term_Freq_List *curr = ht[key];

    if (curr == NULL) return -1;

    do {
        if (strcmp(curr->term, term) == 0) return curr->freq;
        curr = curr->next;
    } while (curr != NULL);

    return -1;
}

    
int tf(Doc doc, const char* term)
{
    int freq = get_freq(doc.term_freqs, term);
    return freq;
}


double idf(Doc *docs_list, int docs_count, const char* term)
{
   if (docs_count < 0) return -1;

   int nt = 0; 
   for (int i = 0; i < docs_count; ++i)
   {
        if (search_ht(docs_list[i].term_freqs, term)) nt += 1;
   }

   return (double)(docs_count + 1) / (nt + 1);
}

int index_documents(DIR *dr, da* docs, const char* dir_name)
{   
    size_t docs_len = 0;

    struct dirent* entry;
    while((entry = readdir(dr)) != NULL)
    {
        if (entry->d_type == DT_REG) {
            printf("\nDoc Entry: %s\n", entry->d_name);

            Doc *doc = malloc(sizeof(Doc)); 
            doc->term_freqs = malloc(sizeof(Term_Freq_List*) * M);
            for (int i = 0; i < M; ++i)
            {
                doc->term_freqs[i] = NULL;
            }
            doc->terms_count = 0;
            char* doc_name = malloc(sizeof(char) * (strlen(dir_name) + strlen(entry->d_name) + 2)); 
            if (doc_name == NULL)
            {
                fprintf(stderr, "ERROR: Could not allocate memory!\n");
                exit(1);
            }

            strcpy(doc_name, dir_name);
            strcat(doc_name, "/");
            strncat(doc_name, entry->d_name, strlen(entry->d_name));
            
            doc->doc_name = doc_name;
            printf("[INFO]: Successfully initialized doc: %s\n", doc->doc_name);

            FILE *f = fopen(doc_name, "rb");
            if (f == NULL) {
                fprintf(stderr, "ERROR could not open file %s!\n", doc_name);
                exit(1);
            }
        
            // Find file size
            fseek(f, 0L, SEEK_END);
            const long f_size = ftell(f);
            fseek(f, 0L, SEEK_SET);

            char *buff = malloc(sizeof(char) * (f_size + 1));
            if (buff == NULL) {
                fprintf(stderr, "ERROR: Could not allocate memory for buffer!\n");
                exit(1);
            }
            

            printf("[INFO]: Successfully opened file: %s\n", doc->doc_name);
            while(fgets(buff, f_size, f))
            {
                char *word = strtok(buff, SPECIAL_CHARS);
                while (word != NULL)
                {
                    char *t = word;
                    for(; *t; ++t) *t = tolower(*t);

                    append_term(doc->term_freqs, word);
                    doc->terms_count++;
                    word = strtok(NULL, SPECIAL_CHARS);
                }
            }
            
            fclose(f);
            printf("[INFO]: Parsed file\n"); 
            free(buff);
            buff = NULL;

            da_append(docs, doc);
            docs_len++;
            printf("[INFO]: Added file: %s to docs list \n", doc->doc_name); 

        }
    }

    for(size_t i = 0; i < docs_len; ++i) 
    {   
        printf("docs[%ld]->doc_name %s, terms_count %ld\n", i, docs->items[i]->doc_name, docs->items[i]->terms_count);
        print_ht(docs->items[i]->term_freqs);
    }
    return docs_len;
}


