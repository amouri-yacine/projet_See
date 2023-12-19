//AMOURI YACINE L3 GROUPE 3
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Global variables
int n1, m1, n2, m2, in, produced, count, consumed, somme;
int **A;
int **B;
int **C;
int *BUFFER; // Utilisation d'un tableau simple pour le buffer au lieu d'une matrice
int shouldExit = 0;

// Buffer
#define N 5 // max element dans notre buffer

// Semaphores
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

// Producer
void *producer(void *arg)
{
    int index = *(int *)arg;
    int somme;

    for (int i = 0; i < m2; i++)
    {
        somme = 0;
        for (int j = 0; j < m1; j++)
        {
            somme += B[index][j] * C[j][i];
        }

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        BUFFER[in] = somme;
        in = (in + 1) % N;

        count++;
        produced++;

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }

    pthread_exit(NULL);
}

// Consumer
void *consumer(void *arg)
{
    int index = *(int *)arg;
    int somme;

    while (!shouldExit)
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        if (shouldExit)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }

        somme = BUFFER[in];
        A[index][in] = somme;
        in = (in + 1) % N;
        count--;
        consumed++;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }

    pthread_exit(NULL);
}


void generatorMatrices()
{
    
    B = (int **)malloc(n1 * sizeof(int *));
    C = (int **)malloc(m1 * sizeof(int *));
    for (int i = 0; i < n1; i++)
    {
        B[i] = (int *)malloc(m1 * sizeof(int));
    }
    for (int i = 0; i < m1; i++)
    {
        C[i] = (int *)malloc(m2 * sizeof(int));
    }

    //remplir matrices B et C
    for (int i = 0; i < n1; i++)
    {
        for (int j = 0; j < m1; j++)
        {
            B[i][j] = rand() % 10;
        }
    }

    for (int i = 0; i < m1; i++)
    {
        for (int j = 0; j < m2; j++)
        {
            C[i][j] = rand() % 10;
        }
    }

    
    BUFFER = (int *)malloc(N * sizeof(int));
}

// afficher matrices
void printMatrices()
{
    printf("\n| Matrice B\n");
    for (int i = 0; i < n1; i++)
    {
        for (int j = 0; j < m1; j++)
        {
            printf("%d  ", B[i][j]);
        }
        printf("\n");
    }

   
    printf("\n| Matrice C\n");
    for (int i = 0; i < m1; i++)
    {
        for (int j = 0; j < m2; j++)
        {
            printf("%d  ", C[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    printf("Entre les colonnes et les lignes de la matrice B :\n ");
    scanf("%d %d", &n1, &m1);
    printf("Entre les colonnes et les lignes de la matrice C : \n");
    scanf("%d %d", &n2, &m2);

    if (m1 != n2)
    {
        perror("Impossible de multiplier B et C.\n");
        exit(EXIT_FAILURE);
    }

    
    generatorMatrices();

  
    A = (int **)malloc(n1 * sizeof(int *));
    for (int i = 0; i < n1; i++)
    {
        A[i] = (int *)malloc(m2 * sizeof(int));
    }

    // Declarer threads
    pthread_t producers_t[n1];
    pthread_t consumer_t[N];

    // Creation thread du producers
    for (int i = 0; i < n1; i++)
    {
        int *index = malloc(sizeof(int));
        *index = i;
        if (pthread_create(&producers_t[i], NULL, producer, index) != 0)
        {
            perror("Erreur de création du thread producer.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Creation threa du consumer
    for (int i = 0; i < N; i++)
    {
        int *index = malloc(sizeof(int));
        *index = i;
        if (pthread_create(&consumer_t[i], NULL, consumer, index) != 0)
        {
            perror("Erreur de création du thread consumer.\n");
            exit(EXIT_FAILURE);
        }
    }

    
    for (int i = 0; i < n1; i++)
    {
        pthread_join(producers_t[i], NULL);
    }

   
     for (int i = 0; i < N; i++)
    {
        pthread_join(consumer_t[i], NULL);
    }

    // affiche matrix A
    printf("\n| Matrice A\n");
    for (int i = 0; i < n1; i++)
    {
        for (int j = 0; j < m2; j++)
        {
            printf("%d  ", A[i][j]);
        }
        printf("\n");
    }

   
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
}