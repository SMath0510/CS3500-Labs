#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef struct __node_t {
    int         key;
    struct __node_t     *next;
    pthread_mutex_t lock;
} node_t;

typedef struct __list_t {
    node_t      *head;
    pthread_mutex_t lock;
} list_t;

void 
List_Init(list_t *L) 
{
    L->head = NULL;
    pthread_mutex_init(&L->lock, NULL); // initialize the lock of the list
}

void 
List_Insert(list_t *L, int key) 
{
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL) { perror("malloc"); return; }
    new->key  = key;    
    pthread_mutex_init(&new->lock, NULL); // initialize the new lock for the new element
    pthread_mutex_lock(&L->lock);  // lock the list while changing it
    pthread_mutex_lock(&new->lock); // lock the new element
    new->next = L->head; 
    L->head   = new;
    pthread_mutex_unlock(&L->lock); // unlock the list once done editing 
}

int 
List_Lookup(list_t *L, int key) 
{
    node_t *tmp = L->head;
    pthread_mutex_lock(&L->lock); // lock the list while look up
    while (tmp) 
    {
        if (tmp->key == key)
        {
            pthread_mutex_unlock(&L->lock); // unlock list once lookup done
            return 1;
        }
        tmp = tmp->next;
    }
    pthread_mutex_unlock(&L->lock); // unlock list once lookup done
    return 0;
}

void 
List_Print(list_t *L) 
{
    node_t *tmp = L->head;
    pthread_mutex_lock(&L->lock); // lock before printing the current list
    while (tmp) 
    {
        printf("%d ", tmp->key);
        tmp = tmp->next;
    }
    printf("\n");
    pthread_mutex_unlock(&L->lock); // unlock once printing done
}

typedef struct _my_struct
{
    list_t *myList; // the list 
    int n; // value of n
    int thread_idx; // the thread index
} my_struct_t;

void * 
my_function(void * inp_arg)
{
    my_struct_t *args = (my_struct_t *)inp_arg;
    int n = args -> n;
    int factor = args -> thread_idx;
    list_t * myList = args -> myList;
    for(int i = 0; i < n; i++){
        int ins_val = (factor * n) + i + 1;
        List_Insert(myList, ins_val);
    }
    return NULL;
}


int main(){
    int n, k, l, r;
    scanf("%d %d %d %d", &n, &k, &l, &r);

    my_struct_t args[k]; // set of arguments per thread
    pthread_t thread[k]; // k threads

    list_t *myList = (list_t *) malloc(sizeof(list_t)); // making the list pointer
    List_Init(myList);

    for(int i = 0; i < k; i++)
    {
        args[i].myList = myList; // setting the list
        args[i].n = n; // setting the value n
        args[i].thread_idx = i;  // thread number i
        printf("Creating the THREAD %d\n", i);
        pthread_create(&thread[i], NULL, my_function, (void *)&args[i]);
    }

    // Finally joining (so that each of their results are available)
    for(int i = 0; i < k; i++){
        printf("Joining the THREAD %d\n", i);
        pthread_join(thread[i], NULL);
    }

    // Look up l to r
    int count = 0;
    for(int i = l; i <= r; i++){
        if(List_Lookup(myList, i)){
            count++;
        }
    }
    printf("%d\n", count);

    printf("Printing the list \n");
    List_Print(myList);
    return 0;
}