#include <stdio.h>
#include <dirent.h>


int main(int argc, char** argv)
{
    struct direct entry;
    DIR* dr = opendir("../");

    if (dr == NULL)
    {

    }

    return 0;
}
