#ifndef STACK_H
    #define STACK_H

    typedef double Elem_t;
    typedef int Error_t;
    typedef long long Jagajaga_t;
    typedef long long Hash_t;

    #define show_dump(stk, verificator) show_dump_basis((stk), #stk, (verificator), __func__, __LINE__, __FILE__)
    #define ELEM_SPEC "%lf"

    enum StackErrorsMasks {
        STACKERRORS_INVALID_SIZE =                1 << 0,
        STACKERRORS_INVALID_CAPACITY =            1 << 1,
        STACKERRORS_INVALID_SIZECAPACITY =        1 << 2,
        STACKERRORS_INVALID_DATA =                1 << 3,
        STACKERRORS_CANT_ALLOCATE_MEMORY =        1 << 4,
        STACKERRORS_CANT_CONSTRUCT =              1 << 5,
        STACKERRORS_CANT_DESTRUCT =               1 << 6,
        STACKERRORS_CANT_CONSTRICT =              1 << 7,
        STACKERRORS_EMPTY_STACK =                 1 << 8,
        STACKERRORS_SPOILED_LEFT_JAGAJAGA =       1 << 9,
        STACKERRORS_SPOILED_RIGHT_JAGAJAGA =      1 << 10,
        STACKERRORS_SPOILED_DATA_LEFT_JAGAJAGA =  1 << 11,
        STACKERRORS_SPOILED_DATA_RIGHT_JAGAJAGA = 1 << 12,
        STACKERRORS_SPOILED_HASH_VALUE =          1 << 13,
        STACKERRORS_SPOILED_DATA_HASH_VALUE =     1 << 14,
    };

    #if 0
        #define ENABLE_CANARY(...)
        #ifdef NCANARYPROTECTION

        #else
            __VA_ARGS__
        #endif
    #endif

    struct Stack {
        Jagajaga_t left_jagajaga;
        int size;
        int capacity;
        Elem_t * data;
        Hash_t hash;
        Hash_t data_hash;
        Jagajaga_t right_jagajaga;
    };

    Error_t stack_ctor(Stack * stk);
    Error_t stack_dtor(Stack * stk);
    Error_t stack_push(Stack * stk, const Elem_t value);
    Error_t stack_pop (Stack * stk, Elem_t * value);

    Jagajaga_t * stack_get_data_left_jagajaga(const Stack * stk);
    Jagajaga_t * stack_get_data_right_jagajaga(const Stack * stk);
    Hash_t calculate_hash(void * stk, const size_t size);
    Hash_t stack_recalculate_hash(Stack * stk, const size_t size);
    void show_dump_basis(const Stack * stk, const char * stack_name, const Error_t * verificator, const char * func, const int line, const char * file);

#endif // STACK_H
