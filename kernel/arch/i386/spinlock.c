#include <kernel/arch/i386/spinlock.h>

void initialize_spinlock(spinlock_t* lock)
{
    lock->lock = 0;
}

void spinlock_acquire(spinlock_t* lock)
{
    while (1)
    {
        if (__sync_bool_compare_and_swap(&(lock->lock), 0, 1))
        {
            break;
        }
        asm volatile("pause");
    }
}

void spinlock_release(spinlock_t* lock)
{
    __sync_bool_compare_and_swap(&(lock->lock), 1, 0);
}