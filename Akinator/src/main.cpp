#include <stdlib.h>

#include "akinator.h"
#include "cmd_input.h"
#include "flags.h"
#include "file_processing.h"
#include "my_assert.h"

int main(int argc, char * argv[])
{
    if (!check_cmd_input(argc, argv))
    {
        return 1;
    }

    AError_t aktor_errors = 0;
    char * buffer = NULL;

    long buffer_size = 0;
    printf("Tree suka\n");
    if (!(buffer_size = text_file_to_buffer(SOURCE_FILE_NAME, &buffer)))
    {
        aktor_errors |= AKINATOR_ERRORS_CANT_CONVERT_TEXT_FILE;

        return aktor_errors;
    }
    printf("Che za huyna?\n");

    Tree akinator_tree = {};

    op_new_tree(&akinator_tree, "");

    printf("%ld\n", buffer_size);             //
    MY_ASSERT(buffer);                          //
    for (long i = 0; i < buffer_size; i++)      //
    {                                           //
        printf("%c", buffer[i]);                //
    }                                           //

    printf("%p\n", buffer);

    if (aktor_errors = create_akinator_tree(&akinator_tree, buffer))
    {
        tree_dump(&akinator_tree);
        // return aktor_errors;
    }

    tree_dump(&akinator_tree);

    free(buffer);
    op_delete_tree(&akinator_tree);

    return 0;
}
