#include <stdio.h>
#include <sys\stat.h>

#include "file_processing.h"
#include "my_assert.h"


// long get_file_size(FILE * fp)
// {
//     struct stat buffer = {};

//     if (fstat(fileno(fp), &buffer))
//         return (long) NULL;

//     return buffer.st_size;
// }


FILE * file_open(const char * file_name, const char * mode)
{
    FILE * fp = NULL;

    if ((fp = fopen(file_name, mode)) == NULL)
    {
        printf("Error. Can't open %s\n", file_name);
    }

    return fp;
}


char * read_file(char * buffer, const size_t buffer_size, FILE * fp)
{
    if (!(fread(buffer, sizeof(char), buffer_size, fp)))
    {
        printf("Error. Can't read the file.\n");

        return (char *) NULL;
    }

    return buffer;
}


size_t write_file(char * buffer, const size_t buffer_size, FILE * fp)
{
    size_t val = 0;

    if (!(val = fwrite(buffer, buffer_size, 1, fp)))
    {
        printf("Error. Can't write the file.\n");

        return (size_t) NULL;
    }

    return val;
}
