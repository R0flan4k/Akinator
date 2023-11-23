#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "stack.h"
#include "my_assert.h"
#include "cmd_input.h"

#define RED_COLOR "\x1b[31;1m"
#define DEFAULT_COLOR "\x1b[0m"

enum StackResizes {
    STACK_EXPAND,
    STACK_CONSTRICT
};

struct StackError {
    StackErrorsMasks mask;
    const char * output_error;
};

static int stack_resize(Stack * stk, StackResizes resize_mode);
static Elem_t * stack_data_to_raw(Stack * stk);
static Elem_t * stack_raw_to_data(void * data);
static Error_t stack_vtor(Stack * stk);
static StackError create_stack_error(StackErrorsMasks error_mask, const char * error_output_info);

const int STACK_EXPAND_COEFFICIENT = 2;
const int STACK_CONSTRICT_COEFFICIENT = 4;
const int STACK_START_CAPACITY = 16;
const int STACK_POISON = 0x70FEEFEE;
Elem_t * const STACK_POISON_PTR = NULL;
const Jagajaga_t STACK_JAGAJAGA_VALUE = 0xAB00B1EABCDEF;


Error_t stack_ctor(Stack * stk)
{
    MY_ASSERT(stk);

    Elem_t * tmp = NULL;
    Error_t errors = 0;

    if (stk->data != nullptr)
    {
        errors |= STACKERRORS_CANT_CONSTRUCT;

        return errors;
    }

    stk->size = 0;
    stk->capacity = STACK_START_CAPACITY;
    stk->left_jagajaga = STACK_JAGAJAGA_VALUE;
    stk->right_jagajaga = STACK_JAGAJAGA_VALUE;
    stk->hash = 0;


    if ((tmp = (Elem_t *) calloc(STACK_START_CAPACITY * sizeof(Elem_t) + 2 * sizeof(Jagajaga_t), sizeof(char))) == NULL)
    {
        stk->data = nullptr;

        errors |= STACKERRORS_CANT_ALLOCATE_MEMORY;

        return errors;
    }

    stk->data = stack_raw_to_data(tmp);

    *stack_get_data_left_jagajaga(stk) =  STACK_JAGAJAGA_VALUE;
    *stack_get_data_right_jagajaga(stk) = STACK_JAGAJAGA_VALUE;

    stk->hash = calculate_hash(stk, sizeof(Stack));
    stk->data_hash = calculate_hash(stk->data, stk->capacity * sizeof(Elem_t));
    errors = stack_vtor(stk);

    return errors;
}


Error_t stack_dtor(Stack * stk)
{
    MY_ASSERT(stk);

    Error_t errors = 0;

    if ((errors = stack_vtor(stk)))
    {
        return errors;
    }

    if (!stk->data && stk->capacity == STACK_POISON && stk->size == STACK_POISON)
    {
        errors |= STACKERRORS_CANT_DESTRUCT;

        return errors;
    }

    stk->capacity =       STACK_POISON;
    stk->size =           STACK_POISON;
    stk->hash =           STACK_POISON;
    stk->data_hash =      STACK_POISON;
    stk->left_jagajaga =  STACK_POISON;
    stk->right_jagajaga = STACK_POISON;

    free(stack_data_to_raw(stk));
    stk->data = nullptr;

    return errors;
}


Error_t stack_push(Stack * stk, const Elem_t value)
{
    MY_ASSERT(stk);

    stk->hash = stack_recalculate_hash(stk, sizeof(Stack));
    stk->data_hash = calculate_hash(stk->data, stk->capacity * sizeof(Elem_t));

    Error_t errors = stack_vtor(stk);

    if (errors)
    {
        show_dump(stk, &errors);

        return errors;
    }

    stk->data[stk->size] = value;//

    (stk->size)++;

    if (stk->size >= stk->capacity)
    {
        MY_ASSERT(stk->size == stk->capacity);

        if (stack_resize(stk, STACK_EXPAND))
        {
            errors |= STACKERRORS_CANT_ALLOCATE_MEMORY;

            return errors;
        }
    }


    stk->hash = stack_recalculate_hash(stk, sizeof(Stack));
    stk->data_hash = calculate_hash(stk->data, stk->capacity * sizeof(Elem_t));

    return errors;
}


Error_t stack_pop(Stack * stk, Elem_t * value)
{
    MY_ASSERT(stk);
    MY_ASSERT(value);

    Error_t errors = stack_vtor(stk);

    if (errors)
    {
        return errors;
    }

    if (stk->size > 0)
    {
        *value = stk->data[stk->size];
        stk->data[stk->size] = STACK_POISON;
        (stk->size)--;

        if (stk->size <= stk->capacity / STACK_CONSTRICT_COEFFICIENT && stk->capacity > STACK_START_CAPACITY)
        {
            MY_ASSERT(stk->size == stk->capacity / STACK_CONSTRICT_COEFFICIENT);

            if (stack_resize(stk, STACK_CONSTRICT))
            {
                errors |= STACKERRORS_CANT_CONSTRICT;

                return errors;
            }
        }

        stk->hash = stack_recalculate_hash(stk, sizeof(Stack));
        stk->data_hash = calculate_hash(stk->data, stk->capacity * sizeof(Elem_t));
    }
    else
    {
        errors |= STACKERRORS_EMPTY_STACK;
    }

    return errors;
}


static int stack_resize(Stack * stk, StackResizes resize_mode)
{
    MY_ASSERT(stk);

    Elem_t * pointer = NULL;

    if (resize_mode == STACK_EXPAND)
    {
        *stack_get_data_right_jagajaga(stk) = 0;

        if (!(pointer = (Elem_t *) realloc(stack_data_to_raw(stk),
                                           stk->capacity * sizeof(Elem_t) * STACK_EXPAND_COEFFICIENT + 2 * sizeof(Jagajaga_t))
          ))
        {
            free(stack_data_to_raw(stk));
            return 1;
        }
        stk->capacity *= STACK_EXPAND_COEFFICIENT;
    }
    else // resize_mode == STACK_CONSTRICT
    {
        if (!(pointer = (Elem_t *) realloc(stack_data_to_raw(stk), (stk->capacity * sizeof(Elem_t)) / STACK_EXPAND_COEFFICIENT + (2 * sizeof(Jagajaga_t)))))
        {
            free(stack_data_to_raw(stk));
            return 1;
        }
        stk->capacity /= STACK_EXPAND_COEFFICIENT;
    }

    stk->data = stack_raw_to_data(pointer);
    *stack_get_data_right_jagajaga(stk) = STACK_JAGAJAGA_VALUE;

    return 0;
}


Jagajaga_t * stack_get_data_left_jagajaga(const Stack * stk)
{
    MY_ASSERT(stk->data);

    return (Jagajaga_t *) stk->data - 1;
}


Jagajaga_t * stack_get_data_right_jagajaga(const Stack * stk)
{
    MY_ASSERT(stk->data);

    return (Jagajaga_t *) (stk->data + stk->capacity);
}


static Elem_t * stack_data_to_raw(Stack * stk)
{
    MY_ASSERT(stk);

    #ifndef NCANARYPROTECTION
        return (Elem_t *) ((Jagajaga_t *) stk->data - 1);
    #elif
        return (Elem_t *) stk->data;
    #endif // NCANARYPROTECTION
}


static Elem_t * stack_raw_to_data(void * data)
{
    MY_ASSERT(data);

    #ifndef NCANARYPROTECTION
        return (Elem_t *) ((Jagajaga_t *) data + 1);
    #elif
        return (Elem_t) data;
    #endif // NCANARYPROTECTION
}


static Error_t stack_vtor(Stack * stk)
{
    MY_ASSERT(stk);

    Error_t verificator = 0;
    Hash_t stack_tmp_hash = stk->hash;
    Hash_t data_tmp_hash =  stk->data_hash;

    if (stk->size < 0)              verificator |= STACKERRORS_INVALID_SIZE;
    if (stk->capacity < 0)          verificator |= STACKERRORS_INVALID_CAPACITY;
    if (stk->size >= stk->capacity) verificator |= STACKERRORS_INVALID_SIZECAPACITY;
    if (stk->left_jagajaga != STACK_JAGAJAGA_VALUE)            verificator |= STACKERRORS_SPOILED_LEFT_JAGAJAGA;
    if (stk->right_jagajaga != STACK_JAGAJAGA_VALUE)           verificator |= STACKERRORS_SPOILED_RIGHT_JAGAJAGA;
    if (stack_tmp_hash != stack_recalculate_hash(stk, sizeof(Stack))) verificator |= STACKERRORS_SPOILED_HASH_VALUE;

    if (!stk->data)
    {
        verificator |= STACKERRORS_INVALID_DATA;
        return verificator;
    }
    if (*stack_get_data_left_jagajaga(stk) != STACK_JAGAJAGA_VALUE)  verificator |= STACKERRORS_SPOILED_DATA_LEFT_JAGAJAGA;
    if (*stack_get_data_right_jagajaga(stk) != STACK_JAGAJAGA_VALUE) verificator |= STACKERRORS_SPOILED_DATA_RIGHT_JAGAJAGA;
    if (data_tmp_hash != calculate_hash(stk->data, stk->capacity *  sizeof(Elem_t))) verificator |= STACKERRORS_SPOILED_DATA_HASH_VALUE;

    stk->data_hash = data_tmp_hash;
    stk->hash = stack_tmp_hash;

    return verificator;
}


void show_dump_basis(const Stack * stk, const char * stack_name, const Error_t * verificator, const char * func, const int line, const char * file)
{
    MY_ASSERT(stk);
    MY_ASSERT(verificator);

    printf("-------------------------------------------------------------------\n");
    printf("Stack[%p] \"%s\" from %s(%d), %s\n", stk, stack_name, file, line, func);
    printf("{\n");

    printf("    hash =              %lld%s\n",
                stk->hash,     stk->hash == STACK_POISON    ? "(POISON)" : "");
    printf("    data hash =         %lld%s\n",
                stk->data_hash, stk->data_hash == STACK_POISON ? "(POISON)" : "");
    printf("    left canary =       %s%s\n",
                stk->left_jagajaga == STACK_JAGAJAGA_VALUE  ? "verified" : "spoiled",
                stk->left_jagajaga == STACK_POISON ? "(POISON)" : "");
    printf("    right canary =      %s%s\n",
                stk->right_jagajaga == STACK_JAGAJAGA_VALUE ? "verified" : "spoiled",
                stk->left_jagajaga == STACK_POISON ? "(POISON)" : "");

    if (stk->data != nullptr)
    {
    printf("    left data canary =  %s%s\n",
                *stack_get_data_left_jagajaga(stk) == STACK_JAGAJAGA_VALUE ? "verified" : "spoiled",
                *stack_get_data_left_jagajaga(stk) == STACK_POISON ? "(POISON)" : "");
    printf("    right data canary = %s%s\n",
                *stack_get_data_right_jagajaga(stk) == STACK_JAGAJAGA_VALUE ? "verified" : "spoiled",
                *stack_get_data_right_jagajaga(stk) == STACK_POISON ? "(POISON)" : "");
    }

    printf("    size =     %d%s\n",   stk->size,     stk->size == STACK_POISON     ? "(POISON)" : "");
    printf("    capacity = %d%s\n",   stk->capacity, stk->capacity == STACK_POISON ? "(POISON)" : "");
    printf("    data[%p%s]\n",        stk->data,     stk->data == STACK_POISON_PTR ? "(POISON)" : "");
    printf("    {\n");

    int i = 0;

    while (i < stk->size && stk->data != nullptr)
    {
        printf("        [%d] = " ELEM_SPEC "\n", i, stk->data[i]);

        i++;
    }

    printf("    }\n");
    printf("}\n");

    if (*verificator)
    {
        static StackError const errors[] = {
            create_stack_error(STACKERRORS_INVALID_SIZE,
                               "Invalid size.\n"),
            create_stack_error(STACKERRORS_INVALID_CAPACITY,
                               "Invalid capacity.\n"),
            create_stack_error(STACKERRORS_INVALID_SIZECAPACITY,
                               "Capacity less than size.\n"),
            create_stack_error(STACKERRORS_INVALID_DATA,
                               "Data is nullptr.\n"),
            create_stack_error(STACKERRORS_CANT_ALLOCATE_MEMORY,
                               "Can't allocate a memory.\n"),
            create_stack_error(STACKERRORS_CANT_CONSTRUCT,
                               "Can't construct a constructed stack.\n"),
            create_stack_error(STACKERRORS_CANT_DESTRUCT,
                               "Can't destruct a destructed stack.\n"),
            create_stack_error(STACKERRORS_CANT_CONSTRICT,
                               "Can't constrict the stack capacity.\n"),
            create_stack_error(STACKERRORS_EMPTY_STACK,
                               "Can't pop an empty stack.\n"),
            create_stack_error(STACKERRORS_SPOILED_LEFT_JAGAJAGA,
                               "Spoiled left canary.\n"),
            create_stack_error(STACKERRORS_SPOILED_RIGHT_JAGAJAGA,
                               "Spoiled right canary.\n"),
            create_stack_error(STACKERRORS_SPOILED_DATA_LEFT_JAGAJAGA,
                               "Spoiled left data canary.\n"),
            create_stack_error(STACKERRORS_SPOILED_DATA_RIGHT_JAGAJAGA,
                               "Spoiled right data canary.\n"),
            create_stack_error(STACKERRORS_SPOILED_HASH_VALUE,
                               "Spoiled hash value.\n"),
            create_stack_error(STACKERRORS_SPOILED_DATA_HASH_VALUE,
                               "Spoiled data hash value.\n"),
        };

        printf("Errors:\n");

        size_t array_size = sizeof(errors) / sizeof(errors[0]);
        size_t j = 0;

        while (j < array_size)
        {
            if (errors[j].mask & *verificator)
                fprintf(OUTPUT_ERROR_FILE, RED_COLOR "%s" DEFAULT_COLOR, errors[j].output_error);

            j++;
        }
    }

    printf("-------------------------------------------------------------------\n");
}


static StackError create_stack_error(StackErrorsMasks error_mask, const char * error_output_info)
{
    StackError error = {
        .mask = error_mask,
        .output_error = error_output_info,
    };

    return error;
}

#ifndef NHASHPOTECTION
    Hash_t calculate_hash(void * stk, const size_t size)
    {
        if (stk != nullptr)
        {
            char * pointer = (char *) stk;
            // char * start_pointer = pointer;
            char * end_pointer = (char *) stk + size - 1;
            Hash_t hash = 0;
            double value = 2;
            int start_pow = 8;
            Hash_t mod = 13371337;

            while (pointer < end_pointer)
            {
                hash += ((Hash_t) (pow(value, (double) (start_pow + (end_pointer - pointer))) * (*pointer))) % mod;

                pointer++;
            }

            return hash;
        }
        else
        {
            return 0;
        }
    }

    Hash_t stack_recalculate_hash(Stack * stk, const size_t size)
    {
        if (stk != nullptr)
        {
            stk->hash = 0;
            stk->data_hash = 0;

            return calculate_hash(stk, size);
        }
        else
        {
            return 0;
        }
    }

#else // NHASHPROTECTION

    Hash_t calculate_hash(Stack * stk, const size_t size)
    {
        return (Hash_t) 0;
    }


    Hash_t stack_recalculate_hash(Stack * stk, const size_t size)
    {
        return (Hash_t) 0;
    }

#endif // NHASHPOTECTION