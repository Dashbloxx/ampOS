#pragma once

typedef struct
{
    unsigned int lock;
} spinlock_t;

void initialize_spinlock(spinlock_t* lock);
void spinlock_acquire(spinlock_t* lock);
void spinlock_release(spinlock_t* lock);