#ifndef TFIDF_H_
#define TFIDF_H_

#define M 50
#define SPECIAL_CHARS " \r\n\t\b,.*&#!()[]{}<>~@$%^_+;-.=?\\:\"'"

#include <stdbool.h>

typedef struct Term_Freq_List Term_Freq_List;
struct Term_Freq_List { 
    size_t freq;
    char* term;
    Term_Freq_List *next;
}; 


typedef struct {
    size_t            terms_count;
    char*             doc_name;
    Term_Freq_List**  term_freqs;
} Doc;

// ------------------------- DA
typedef struct {
    size_t size;
    size_t capacity;
    Doc** items;
} da;

void da_append(da* arr, Doc* doc);
void da_init(da* arr);


// ------------------------- DA
int hash(const char* term);
void append_term(Term_Freq_List** ht, char* term);
void print_ht(Term_Freq_List** ht);
void free_ht(Term_Freq_List** ht);

bool search_ht(Term_Freq_List** ht, const char* item);
int get_freq(Term_Freq_List** ht, const char* term);

void free_doc(Doc* doc);
int tf(Doc doc, const char* term);
double idf(da* docs, const char* term);
double tf_idf(da* docs, Doc doc, const char* term);

int index_documents(DIR *dr, da* docs, const char* dir_name);


#endif // TFIDF_H_
