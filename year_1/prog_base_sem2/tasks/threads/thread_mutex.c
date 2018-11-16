#include <windows.h>
#include <stdlib.h>
#include "thread.h"
#include "mutex.h"

/* THREAD PART */

struct thread_s
{
    HANDLE win_thread;
    DWORD win_tid;
    void *result;
};

typedef struct _thread_input_args_s
{
    thread_func_t fun; // function to run
    void *args; // arguments for fun (?!)
    void **result; // return value for fun (?!)
} _thread_input_args_t;

static DWORD _thread_with_result(LPVOID win_args)
{
    _thread_input_args_t *args = (_thread_input_args_t *)win_args;
    *args->result = args->fun(args->args);
    free(win_args);
    return (0); // Success
}

thread_t *thread_getCurrentThread(void)
{
    thread_t * th = malloc(sizeof(struct thread_s));
    th->win_thread = GetCurrentThread();
    th->win_tid = GetCurrentThreadId();
    th->result = (void *)NULL;
    return (th);
}

thread_t *thread_create_args(thread_func_t func, void *args)
{
    thread_t * t = malloc(sizeof(struct thread_s));
    t->result = (void *)NULL;

    _thread_input_args_t * wrapped_args = malloc(sizeof(_thread_input_args_t));
    wrapped_args->fun = func;
    wrapped_args->args = args;
    wrapped_args->result = (void **)&t->result;
    t->win_thread = CreateThread(
                        NULL,
                        0,
                        (LPTHREAD_START_ROUTINE)_thread_with_result,
                        (LPVOID)wrapped_args,
                        0,
                        &t->win_tid);
    return (t);
}

void thread_free(thread_t *self)
{
    CloseHandle(self->win_thread);
    free(self);
}

int thread_join(const thread_t *self)
{
    WaitForSingleObject(self->win_thread, INFINITE);
    return (0);
}

int thread_join_result(const thread_t *self, void **result)
{
    WaitForSingleObject(self->win_thread, INFINITE);
    *result = (void *)self->result;
    return (0);
}

int thread_terminate(const thread_t *self)
{
    TerminateThread(self->win_thread, 0);
    return (0);
}

void thread_exit(void * retval)
{
    ExitThread(0);
}

int thread_equals(const thread_t *t1, const thread_t *t2)
{
    return (t1->win_tid == t2->win_tid);
}

DWORD thread_get_id(const thread_t *t1)
{
    return (t1->win_tid);
}

/* MUTEX PART */
struct mutex_s
{
    HANDLE win_mutex;
};

mutex_t *mutex_new(void)
{
    mutex_t *ins = malloc(sizeof(struct mutex_s));
    ins->win_mutex = CreateMutex(
                         NULL,
                         FALSE,
                         NULL);
    return (ins);
}

void mutex_free(mutex_t *self)
{
    CloseHandle(self->win_mutex);
    free(self);
}

void mutex_lock(mutex_t *self)
{
    WaitForSingleObject(self->win_mutex, INFINITE);
}

void mutex_unlock(mutex_t *self)
{
    ReleaseMutex(self->win_mutex);
}
