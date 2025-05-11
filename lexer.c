#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define LALLOCS_IMPLEMENTATION
#include "lallocs.h"

#include "lexer.h"

#define DEFAULT_SIZE 256
#define SPECIAL_CHARS "*&#!();-.=?\\:\"'"
#define WHITE_SPACE " \n\t"

Position* new_pos(List* ptrs, const char* const file_name, int line, int col)
{
    Position* pos = lmalloc(ptrs, sizeof(Position));
    if (pos == NULL) return NULL;

    pos->file_name = lmalloc(ptrs, (strlen(file_name) + 1) * sizeof(char));
    if (pos->file_name == NULL) return NULL;

    strncpy(pos->file_name, file_name, strlen(file_name) + 1);

    pos->line = line;
    pos->col = col;

    return pos;
}


void free_pos(List*ptrs, Position* pos)
{
    if (pos != NULL)
    {
        lfree(ptrs, pos->file_name);
        pos->file_name = NULL;
    }
}

void add_pos(List* ptrs, Lexeme* l, Position pos)
{

    l->pos = lmalloc(ptrs, sizeof(Position));

    if (l->pos == NULL)
    {
        fprintf(stderr, "ERROR: Could not copy position\n");
        exit(1);
    }

    
   // if(!memcpy(l->pos, pos, sizeof(Position)))
   // {
   //     fprintf(stderr, "ERROR: Could not copy position\n");
   //     exit(1);
   // }
    
   l->pos->file_name = pos.file_name;
   l->pos->line = pos.line;
   l->pos->col = pos.col;
}

Lexeme* new_lexeme(List* ptrs, char* val, Position pos)
{
    Lexeme* lexeme = lmalloc(ptrs, sizeof(Lexeme));
    if (lexeme == NULL) 
    {
        fprintf(stderr, "ERROR: Could not allocate memory");
        exit(1);
    }

    lexeme->value = lmalloc(ptrs, strlen(val) + 1);
    if (lexeme->value == NULL) return NULL;

    strncpy(lexeme->value, val, strlen(val) + 1);
    add_pos(ptrs, lexeme, pos);

    return lexeme;
}

void print_lexeme(Lexeme lexeme)
{
    if (lexeme.value == NULL)
    {
        printf("ERROR: Lexeme seems to be empty!\n");
        return;
    }
    printf("/%s:%d:%d : ", lexeme.pos->file_name, lexeme.pos->line, lexeme.pos->col);
    printf("%s\n", lexeme.value);

}

void free_lexeme(List* ptrs, Lexeme* lexeme)
{
    if (lexeme != NULL)
    {

        free_pos(ptrs, lexeme->pos);
        lfree(ptrs, lexeme->pos);
        lexeme->pos = NULL;
        lfree(ptrs, lexeme->value);
        lexeme->value = NULL;
    }
}

void new_darray(List* ptrs, Lexemes* arr)
{
    if (arr == NULL) return;

    arr->size = DEFAULT_SIZE;
    arr->count = 0;
    arr->items = lmalloc(ptrs, DEFAULT_SIZE * sizeof(Lexeme));

    if (arr->items == NULL)
    {
        fprintf(stderr, "ERROR: Could not allocate array\n");
        exit(1);
    }
}

int da_append(List *ptrs, Lexemes *arr, Lexeme item)
{
    if (arr->count < arr->size)
    {
        arr->items[arr->count] = item;
    }
    else
    {
        arr->size += DEFAULT_SIZE;
        arr->items = lrealloc(ptrs, arr->items, (arr->size)*sizeof(Lexeme));
        if(arr->items == NULL) return -1;
        arr->items[arr->count] = item;
    }
    arr->count++;
    return 0;
}

void free_lexemes(List* ptrs, Lexemes *arr)
{

    for (int i = 0; i < arr->count; i++) {
        free_lexeme(ptrs, &arr->items[i]);
    }
    lfree(ptrs, arr->items);

    printf("Freed da_array.\n");
}

void lex_file(List *ptrs, Lexemes* arr, FILE* file, const char* file_name)
{

    // Find EOF
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file);

    // Go back to file start
    fseek(file, 0L, SEEK_SET);

    // Allcate buffer for file contents
    char *line_contents = lmalloc(ptrs, file_size);
    if (line_contents == NULL){
        fprintf(stderr, "ERROR: Could not allocate memory for reading file.\n");
        exit(1); 
    }

    if (arr == NULL){
        fprintf(stderr, "ERROR: Could not allocate memory for lexemes.\n");
        exit(1);
    }
    new_darray(ptrs, arr);


    // Getting file contents
    int line_number = 0;
    while (fgets(line_contents, file_size, file))
    {
        line_number++;
        int col_number = 0;

        char* buffer = lmalloc(ptrs, strlen(line_contents) + 1);
        if (buffer == NULL) {
            fprintf(stderr, "ERROR: Could not allocate memory for buffer! \n");
            exit(1);
        }
        strcpy(buffer, "\0");

        for (size_t i = 0; i < strlen(line_contents); i++) {
            col_number++;
            const char ch = line_contents[i];

            if (strchr(SPECIAL_CHARS, ch) != NULL || strchr(WHITE_SPACE, ch) != NULL) {
                // Add current word to array
                if (strlen(buffer) > 0) {
                    //strncpy(buffer+strlen(buffer), "\0", 1);
                    strncat(buffer, "\0", 1);
                    //Position *pos = new_pos(file_name, line_number, col_number);

                    Lexeme *l = new_lexeme(ptrs, buffer, (Position) { (char*) file_name, line_number, col_number});
                    da_append(ptrs, arr, *l);
                    //free_pos(pos);
                    free_lexeme(ptrs, l);
                }
                
                // Append char
                strncpy(buffer, &ch, 1);
                strncpy(buffer+1, "\0", 1);

                //Position *pos = new_pos(file_name, line_number, col_number);
                Lexeme *l = new_lexeme(ptrs, buffer, (Position) { (char*) file_name, line_number, col_number});
                da_append(ptrs, arr, *l);
                strncpy(buffer, "\0", 1);
                free_lexeme(ptrs, l);
                //free_pos(pos);
                continue;
            }

            else {
                strncat(buffer, &ch, 1);
                col_number--;
                continue;
            }
        }

        lfree(ptrs, buffer);
        buffer = NULL;
    }
    lfree(ptrs, line_contents);
    line_contents = NULL;
}
