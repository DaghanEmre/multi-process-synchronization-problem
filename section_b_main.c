#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define RAND_DIVISOR 100000000
#define TRUE 1
#define FULL 0
#define EMPTY 0

/*I got inspired from these 2 links below*/
/*While learning the true implementation*/
/*Then, i modified, merged, and changed  these codes according to our homework*/
/* http://www.linuxdevcenter.com/pub/a/linux/2007/05/24/semaphores-in-linux.html?page=6&x-maxdepth=0#thread */
/* http://stackoverflow.com/questions/19844716/producer-consumer-program-using-semaphores-and-pthreads */

	int *buf;
	int buf_Size;
	sem_t empty_sem;
	sem_t full_sem;
	pthread_mutex_t mutex;
	int in, out;

void insert(int item, int index){
	sem_wait(&empty_sem);
	pthread_mutex_lock(&mutex);
	
	buf[in] = item;
	in = (in + 1) % buf_Size;
	printf("Producing random number: %d ... nextIn : %d\n", item, in);
	fflush(stdout);
	
	pthread_mutex_unlock(&mutex);
	sem_post(&full_sem);
	
	if(index % 2 == 1)
		sleep(1);
}
	
	
void *producer(){
	int index,item;
	for(index=0 ; index < buf_Size-1; index++){
		insert(item = rand(), index);
	}
	return NULL;
}

void remover(int index){
	int item;
	
	sem_wait(&full_sem);
	pthread_mutex_lock(&mutex);
	
	
	item = buf[out];
	out = (out + 1) % buf_Size;
	printf(" 	Consuming random number: %d ... nextOut: %d\n", item, out);
	fflush(stdout);
	
	pthread_mutex_unlock(&mutex);
	sem_post(&empty_sem);
	
	if(index % 2 == 1)
		sleep(1);
} 

void *consumer(){
	int index;
	for(index=0; index < buf_Size-1; index++){
		remover(index);
	}
	return NULL; 
}



int main(int argc, char *argv[]){

	pthread_t producer_ID, consumer_ID;
	int buffer_Size = atoi(argv[1]);

	buf = (int*) malloc(sizeof(int *) * buffer_Size) ;
	
	if(argc != 3){
		fprintf(stderr, "ERROR: Wrong Argument Sequence\n");
		fprintf(stderr, "USAGE:./main <INT> <INT>\n");
	}
	
	
	int consum_Num = atoi(argv[2]);

	buf_Size = buffer_Size;
	in = 0,  out = 0;
	sem_init(&empty_sem, 0, buf_Size - 1);
	sem_init(&full_sem, 0, 0);
	pthread_mutex_init(&mutex,NULL);
	
	int i;

	if( pthread_create(&producer_ID,NULL,producer, (void*)NULL) ){
		printf("\n ERROR: Could Not Create Producer Thread");
		exit(1);
	}
	for(i=0 ; i < consum_Num; i++)
	{
		if( pthread_create(&consumer_ID,NULL,consumer, (void*)NULL) ){
			printf("\n ERROR: Could Not Create Consumer Thread");
			exit(1);
		}
	}
	
	sleep(5);

	if(pthread_join(producer_ID, NULL)){
		printf("\n ERROR joining thread");
		exit(1);
	}
	for(i=0 ; i < consum_Num; i++)
	{
		if(pthread_join(consumer_ID, NULL)){
			printf("\n ERROR joining thread");
			exit(1);
		}
		
	}
	
	sem_destroy(&empty_sem);
	sem_destroy(&full_sem);
	
	pthread_exit(NULL);
	return 1;

}