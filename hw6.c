// Name: Akshita Akumalla ID: 59965772
// Name:       ID:

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int pthread_mutex_lock( pthread_mutex_t* mutex );
int pthread_mutex_unlock( pthread_mutex_t* mutex );
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);

//Producer
// while (true) {
// pthread_mutex_lock (&mutex);
// while (/*buffer is full*/){
// pthread_cond_wait(&notfull,&mutex);
// }
// /*produces an item*/
// pthread_cond_signal(&notempty);
// pthread_mutex_unlock(&mutex);
// }

// Consumer:
// while (true) {
// pthread_mutex_lock (&mutex);
// while (/*buffer is empty*/){
// pthread_cond_wait(&notempty,&mutex);
// }
// /*consumes an item*/
// pthread_cond_signal(&notfull);
// pthread_mutex_unlock(&mutex);
// }
int main(){
    return 0;
}