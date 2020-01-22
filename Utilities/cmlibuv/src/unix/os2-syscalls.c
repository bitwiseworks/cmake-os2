/*
 * scandir() and clock_gettime() implementation for OS/2 kLIBC
 * POSIX semaphore implementation for OS/2 kLIBC
 *
 * Copyright (C) 2016 KO Myung-Hun <komh@chollian.net>
 * Copyright (C) 2020 bww bitwise wroks GmbH
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>

#include "uv/semaphore.h"

#include <dirent.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

/**
 * scandir()
 *
 * @remark OS/2 kLIBC declares scandir() differently from POSIX
 */
int scandir( const char *dir, struct dirent ***namelist,
             int ( *sel )(/* const */ struct dirent * ),
             int ( *compare )( const /* struct dirent ** */ void *,
                               const /* struct dirent ** */ void *))
{
    DIR *dp;
    struct dirent **list = NULL;
    size_t list_size = 0;
    int list_count = 0;
    struct dirent *d;
    int saved_errno;

    dp = opendir( dir );
    if( dp == NULL )
        return -1;

    /* Save original errno */
    saved_errno = errno;

    /* Clear errno to test later */
    errno = 0;

    while(( d = readdir( dp )) != NULL )
    {
        int selected = !sel || sel( d );

        /* Clear errno modified by sel() */
        errno = 0;

        if( selected )
        {
            struct dirent *d1;
            size_t len;

            /* List full ? */
            if( list_count == list_size )
            {
                struct dirent **new_list;

                if( list_size == 0 )
                    list_size = 20;
                else
                    list_size += list_size;

                new_list = ( struct dirent ** )realloc( list,
                                                        list_size *
                                                            sizeof( *list ));
                if( !new_list )
                {
                    errno = ENOMEM;
                    break;
                }

                list = new_list;
            }

            /* On OS/2 kLIBC, d_name is not the last member of struct dirent.
             * So just allocate the size of struct dirent. */
            len = sizeof( struct dirent );
            d1 = ( struct dirent * )malloc( len );
            if( !d1 )
            {
                errno = ENOMEM;

                break;
            }

            memcpy( d1, d, len );

            list[ list_count++ ] = d1;
        }
    }

    /* Error ? */
    if( errno )
    {
        /* Store errno to use later */
        saved_errno = errno;

        /* Free a directory list */
        while( list_count > 0 )
            free( list[ --list_count ]);
        free( list );

        /* Indicate an error */
        list_count = -1;
    }
    else
    {
        /* If compare is present, sort */
        if( compare != NULL )
            qsort( list, list_count, sizeof( *list ), compare );

        *namelist = list;
    }

    /* Ignore error of closedir() */
    closedir( dp );

    errno = saved_errno;

    return list_count;
}


int clock_gettime(clockid_t id, struct timespec *ts)
{
    struct timeval tv;

    gettimeofday (&tv, NULL);
    ts->tv_sec = tv.tv_sec;
    ts->tv_nsec = tv.tv_usec * 1000;

    return 0;
}


#define EPILOGUE                    \
    do {                            \
        int err = rc2errno( rc );   \
        if( err )                   \
        {                           \
            errno = err;            \
            return -1;              \
        }                           \
    } while( 0 );                   \
    return 0

/**
 * Return errno value corresponding to OS/2 error code
 *
 * @param[in] rc OS/2 error code.
 * @return 0 if @a rc is treated as no error, otherwise errno value
 *         corresponding to OS/2 error code.
 */
static int rc2errno( ULONG rc )
{
    int err = 0;

    switch( rc )
    {
        case ERROR_INVALID_HANDLE :
            err = EINVAL;
            break;

        case ERROR_NOT_ENOUGH_MEMORY :
            err = ENOMEM;
            break;

        case ERROR_INVALID_PARAMETER :
            err = EINVAL;
            break;

        case ERROR_INTERRUPT :
            err = EINTR;
            break;

        case ERROR_TOO_MANY_HANDLES :
            err = ENOSPC;
            break;

        case ERROR_SEM_BUSY :
            err = EBUSY;
            break;

        case ERROR_TIMEOUT :
            err = EAGAIN;
            break;
    }

    return err;
}

/**
 * sem_init()
 *
 * @todo Support @a pshared
 */
int sem_init( sem_t *sem, int pshared, unsigned int value )
{
    if( value > SEM_VALUE_MAX )
    {
        errno = EINVAL;

        return -1;
    }

    if( DosCreateEventSem( NULL, &sem->hev, 0, value > 0 ? TRUE : FALSE ) ||
        DosCreateMutexSem( NULL, &sem->hmtxWait, 0, FALSE ) ||
        DosCreateMutexSem( NULL, &sem->hmtxCount, 0, FALSE ))
    {
        errno = ENOSPC;

        sem_destroy( sem );

        return -1;
    }

    sem->count = value;

    return 0;
}

/**
 * sem_destroy()
 */
int sem_destroy( sem_t *sem )
{
    ULONG rc;

    rc = DosCloseEventSem( sem->hev );

    if( !rc )
        rc = DosCloseMutexSem( sem->hmtxWait );

    if( !rc )
        rc = DosCloseMutexSem( sem->hmtxCount );

    EPILOGUE;
}

/**
 * sem_post()
 */
int sem_post( sem_t *sem )
{
    ULONG rc = 0;

    DosRequestMutexSem( sem->hmtxCount, SEM_INDEFINITE_WAIT );

    if( sem->count < SEM_VALUE_MAX )
    {
        sem->count++;

        rc = DosPostEventSem(sem->hev);
    }

    DosReleaseMutexSem(sem->hmtxCount);

    EPILOGUE;
}

static int sem_wait_common( sem_t *sem, ULONG ulTimeout )
{
    ULONG rc;

    DosRequestMutexSem( sem->hmtxWait, SEM_INDEFINITE_WAIT );

    rc = DosWaitEventSem( sem->hev, ulTimeout );

    if( !rc )
    {
        DosRequestMutexSem( sem->hmtxCount, SEM_INDEFINITE_WAIT );

        sem->count--;
        if( sem->count == 0 )
        {
            ULONG ulCount;

            DosResetEventSem( sem->hev, &ulCount );
        }

        DosReleaseMutexSem( sem->hmtxCount );
    }

    DosReleaseMutexSem( sem->hmtxWait );

    EPILOGUE;
}

/**
 * sem_wait()
 */
int sem_wait( sem_t *sem )
{
    return sem_wait_common( sem, SEM_INDEFINITE_WAIT );
}

/**
 * sem_trywait()
 */
int sem_trywait( sem_t *sem )
{
    return sem_wait_common( sem, SEM_IMMEDIATE_RETURN );
}

