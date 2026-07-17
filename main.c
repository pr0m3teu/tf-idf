#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include "tfidf.h"

#define BUF_SIZE 1024

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: No Folder provided!\n");
        fprintf(stderr, "USAGE: ./main <FOLDER>\n");
        exit(1);
    }

    const char* dir_name = argv[1];
    DIR* dr = opendir(dir_name);

    if (dr == NULL)
    {
        fprintf(stderr, "ERROR: could not open folder!\n");
        exit(1);
    }

    da docs = {0};
    da_init(&docs);

    printf("Began indexing folder: %s\n", dir_name);
    if (index_documents(dr, &docs, dir_name) < 0)
    {
        exit(1);
    }
    printf("Finished!\n");
    
    double max_score = 0;
    Doc relevant_docs[10] = {0};
    int k = 0;

    char buf[BUF_SIZE];

    bool ok = true;
    while (ok)
    {
        printf("Search term > ");
        fgets(buf, BUF_SIZE, stdin);
        
        buf[strlen(buf) - 1] = 0;
        
        printf("> Searching for: \"%s\"\n", buf);
        for (size_t i = 0; i < docs.size; ++i)
        {
            double score = tf_idf(&docs, *docs.items[i], buf);
            if (score > max_score && k < 10)
            {
                relevant_docs[k++] = *docs.items[i];
                max_score = score;
            }
        }
 
        
        printf("> Found %d relevant files:\n", k);
        for (int i = 0; i < k; ++i)
        {
            printf("Doc %d: %s\n", i, relevant_docs[i].doc_name);
        }

        printf("Continue (yes: y, no: n): ");
        fgets(buf, BUF_SIZE, stdin);

        buf[strlen(buf) - 1] = 0;
        if (strcmp(buf, "y") != 0) ok = false;
        k = 0;
        max_score = 0;

    }
       
    closedir(dr);
   
    for (size_t i = 0; i < docs.size; ++i)
    {
        free_doc(docs.items[i]);
    }

    free(docs.items);
    return 0;
}



