// Author: Luka Napotnik <luka.napotnik@gmail.com>
// This code is public domain.

#include <stdlib.h>
#include <fcntl.h> 
#include <signal.h> 
#include <stdio.h> 
#include <string.h> 
#include <sys/mman.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <assert.h>

#define _
struct _memory_watchlist_addr {
    void *addr;
    size_t size;
    struct _memory_watchlist_addr *next, *prev;
};

struct _memory_watchlist_addr *_memory_watchlist = NULL;

void _watch_address(void *addr, size_t size, int prot) {
    struct _memory_watchlist_addr *watch_addr = malloc(sizeof(*watch_addr));
    watch_addr->addr = addr;
    watch_addr->size = size;

    if (_memory_watchlist)
        _memory_watchlist->prev = watch_addr;
    // prepend watch to list
    watch_addr->next = _memory_watchlist;
    watch_addr->prev = NULL;
    _memory_watchlist = watch_addr;
    // now protect the memory map
    mprotect(addr, size, prot);
}

void _sigsegv_protector(int s, siginfo_t *sig_info, void *context)
{
    printf("---\n");
    printf("%s: Process received segmentation fault, examening ...\n", __func__);
    printf("%s: cause was address %p\n", __func__, sig_info->si_addr);
    
    struct _memory_watchlist_addr *watched_addr = _memory_watchlist;

    for (; watched_addr != NULL; watched_addr = watched_addr->next) {
        if (watched_addr->addr == sig_info->si_addr)
            break;
    }

    if (watched_addr) {
        printf("%s: raised because of invalid r/w acces to address (was in watchlist) ...\n", __func__);
        if (watched_addr->prev)
            watched_addr->prev->next = watched_addr->next;

        _memory_watchlist = watched_addr->next;
        mprotect (sig_info->si_addr, watched_addr->size, PROT_READ | PROT_WRITE);
        free(watched_addr);
        printf("---\n");
    } else {
        printf("---\n");
        exit(1);
    }
}

void test_segfault_protector()
{
    char *memory;
    int fd, alloc_size;
    // size of one page on our system
    alloc_size = getpagesize(); 

    // allocate page-aligned memory
    fd = open ("/dev/zero", O_RDONLY); 
    memory = mmap(NULL, alloc_size, PROT_WRITE, MAP_PRIVATE, fd, 0); 
    assert(memory != NULL);
    close (fd); 
    // obtain a private copy of the page
    memory[0] = 0;

    // Here's the main part ... add the allocated memory to the
    // watchlist. After this call, the memory will be unreadable/unwriteble.
    _watch_address(memory, alloc_size, PROT_NONE);
    
    // Let's try this out. The following line should cause a SIGSEGV.
    char *ref = &memory[0];

    // And we're finished.
    munmap (memory, alloc_size); 
}

int main () 
{
    struct sigaction sa;

    // Setup our signal handler 
    memset (&sa, 0, sizeof (sa)); 
    sa.sa_sigaction = &_sigsegv_protector;
    sa.sa_flags = SA_SIGINFO;
    sigaction (SIGSEGV, &sa, NULL); 

    // Run test
    test_segfault_protector();

    printf("Program exited\n");
    return 0; 
} 
