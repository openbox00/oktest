#include <okl4/env.h>
#include <okl4/init.h>
void __malloc_init(uintptr_t head_base,  uintptr_t heap_end);
void __sys_entry(void * env);
void __sys_thread_entry(void);
int main(int argc, char **argv);

void *_okl4_forced_symbols = &okl4_forced_symbols;
void
__sys_entry(void *env)
{
    okl4_env_segment_t *heap;
    okl4_env_args_t *args;
    int argc = 0;
    char **argv = NULL;
    int result;

    if ((int)&_okl4_forced_symbols == 1) {
        for (;;);
    }
    /* Setup environment. */
    __okl4_environ = env;

    /* Get the heap address and size from the environment. */
    heap = okl4_env_get_segment("MAIN_HEAP");

    /* Get the command line arguments from the environment. */
    args = OKL4_ENV_GET_ARGS("MAIN_ARGS");
    if (args != NULL) {
        argc = args->argc;
        argv = &args->argv;
    }
    /* Initialise heap. */
    __malloc_init(heap->virt_addr, heap->virt_addr + heap->size);

    result = main(argc, argv);
}

void
__sys_thread_entry(void)
{
    int result;
    result = main(0, NULL);
}

