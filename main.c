#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>


int main(int argc, char** argv)
{
    struct dirent* entry;
    DIR* dr = opendir("../");

    if (dr == NULL)
    {
        fprintf(stderr, "ERROR: could not open folder!\n");
        exit(1);
    }
    
    while((entry = readdir(dr)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    closedir(dr);

    return 0;
}
