#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

#include "tfidf.h"

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

    index_documents(dr, &docs, dir_name);
    
    closedir(dr);
    for (size_t i = 0; i < docs.size; ++i)
    {
        free_doc(docs.items[i]);
    }

    free(docs.items);
    return 0;
}



