#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <pthread.h>

void printInfos(int numRC, float **matrix, float *vector);
void printResults(int numRC, float **matrix, float *vector, float *solutions);
void forwardElimination(int numRC, float **matrix, float *vector);
float *backwardSubstitution(int numRC, float **matrix, float *vector);
void *normalize(void *Data);
void *normalizeMatrix(void *Data);

//modelo de dados a ser paralelizado
class Data
{
public:
    int i, j, k, numRC;
    float multiplyFactor;
    float **matrix;
    float *vector;

    Data()
    {
        i = 0;
        j = 0;
        numRC = 0;
    }
};

pthread_mutex_t mutex;

int main(int argc, char const *argv[])
{
    int i, j, k, numRC;

    //read number of rows and columns
    numRC = 3;

    //srand(time(NULL));

    float **matrix = new float *[numRC];
    for (int i = 0; i < numRC; i++)
        matrix[i] = new float[numRC];

    float *vector = new float[numRC];

    matrix[0][0] = 1;
    matrix[0][1] = 3;
    matrix[0][2] = 2;
    matrix[1][0] = 9;
    matrix[1][1] = 15;
    matrix[1][2] = 5;
    matrix[2][0] = 2;
    matrix[2][1] = 0;
    matrix[2][2] = 8;

    /*
    float matrix[numRC][numRC] = {{1, 3, 2},
                                  {9, 15, 5},
                                  {2, 0, 8}};
    */
    int init[3] = {76, 40, 91};

    for (i = 0; i < numRC; i++)
    {
        vector[i] = init[i]; //rand() % 128;
        printf("%f\n", vector[i]);
    }

    printInfos(numRC, matrix, vector);

    pthread_mutex_init(&mutex, NULL);

    //realiza o escalonamento reduzido da matriz; é a fase de eliminação
    forwardElimination(numRC, matrix, vector);

    printInfos(numRC, matrix, vector);

    float *solutions = backwardSubstitution(numRC, matrix, vector);

    printResults(numRC, matrix, vector, solutions);

    // desalocar memoria usando o operador delete[]
    for (int i = 0; i < numRC; i++)
        delete[] matrix[i];

    delete[] matrix;
    delete[] vector;
    delete[] solutions;

    return 0;
}

void forwardElimination(int numRC, float **matrix, float *vector)
{
    float multiplyFactor = 0;

    Data *dataV = new Data[numRC];
    Data **dataM = new Data *[numRC];
    for (int i = 0; i < numRC; i++)
        dataM[i] = new Data[numRC];

    int numRT;
    int numCT;
    int numRCT;
    int countI = 0;

    //NÃO-PARALELIZAVEL
    for (int k = 0; k < numRC - 1; k++)
    {
        numRT = 0;
        numCT = 0;
        numRCT = 0;

        countI = k + 1;
        for (int i = k + 1; i < numRC; i++)
        {
            if (matrix[i][k] == 0)
            {
                continue;
            }
            dataV[numRT].i = countI;
            dataV[numRT].k = k;
            dataV[numRT].matrix = matrix;
            dataV[numRT].vector = vector;
            dataV[numRT].numRC = numRC;
            dataV[numRT].multiplyFactor = matrix[k][k] / matrix[i][k];
            numRT++;
            countI++;
        }

        countI = k + 1;
        for (int i = k + 1; i < numRC; i++)
        {
            if (matrix[i][k] == 0)
            {
                countI++;
                continue;
            }
            for (int j = k; j < numRC; j++)
            {
                dataM[numCT][j].i = countI;
                dataM[numCT][j].j = j;
                dataM[numCT][j].k = k;
                dataM[numCT][j].matrix = matrix;
                dataM[numCT][j].vector = vector;
                dataM[numCT][j].numRC = numRC;
                dataM[numCT][j].multiplyFactor = matrix[k][k] / matrix[i][k];
            }
            countI++;
            numCT++;
        }

        pthread_t rowsThreads[numRT];
        pthread_t columnsThreads[numCT];

        for (int i = 0; i < numRT; i++)
        {
            pthread_create(&(rowsThreads[i]), NULL, normalize, (void *)&dataV[i]);
            std::cout << "Created row thread " << i << std::endl;
        }

        for (int i = 0; i < numRT; i++)
        {
            pthread_join(rowsThreads[i], NULL);
            std::cout << "Synchronizing row thread " << i << "\n\n";
        }

        for (int i = 0; i < numCT; i++)
        {
            for (int j = k; j < numRC; j++)
            {
                pthread_create(&(columnsThreads[j]), NULL, normalizeMatrix, (void *)&dataM[i][j]);
                std::cout << "Created column thread " << i << " " << j << std::endl;
            }

            for (int j = k; j < numRC; j++)
            {
                pthread_join(columnsThreads[j], NULL);
                std::cout << "Synchronizing column thread " << j << "\n\n";
            }
        }
    }
}

void *normalize(void *dataFormal)
{
    Data *data = (Data *)dataFormal;
    int i = data->i;
    int j = data->j;
    int k = data->k;

    float prev;

    pthread_mutex_lock(&mutex);
    std::cout << data->matrix[i][k] << std::endl;

    prev = data->vector[i];
    data->vector[i] = data->vector[k] - data->multiplyFactor * data->vector[i];
    printf("%.2f = %.2f - %.2f * %.2f\n", data->vector[i], data->vector[k], data->multiplyFactor, prev);

    std::cout << data->vector[i] << std::endl;
    pthread_mutex_unlock(&mutex);
}

void *normalizeMatrix(void *dataFormal)
{
    Data *data = (Data *)dataFormal;

    int i = data->i;
    int j = data->j;
    int k = data->k;

    float prev;

    pthread_mutex_lock(&mutex);

    printf("B4 ops\n %d\n %d\n %d\n", i, j, k);
    printInfos(data->numRC, data->matrix, data->vector);

    data->matrix[i][j] = data->matrix[k][j] - data->multiplyFactor * data->matrix[i][j];

    printf("%.2f = %.2f - %.2f * %.2f\n", data->matrix[i][j], data->matrix[k][j], data->multiplyFactor, prev);

    printInfos(data->numRC, data->matrix, data->vector);
    pthread_mutex_unlock(&mutex);
}

float *backwardSubstitution(int numRC, float **matrix, float *vector)
{
    float *solutions = new float[numRC];
    float sumMatrix = 0;

    //faz o cálculo das soluções do exercício
    for (int i = numRC - 1; i >= 0; i--)
    {
        sumMatrix = 0;

        for (int j = i + 1; j <= numRC - 1; j++)
        {
            sumMatrix += matrix[i][j] * solutions[j];
        }

        solutions[i] = (vector[i] - sumMatrix) / matrix[i][i];
    }

    return solutions;
}

void printResults(int numRC, float **matrix, float *vector, float *solutions)
{
    for (int i = 0; i < numRC; i++)
    {
        for (int j = 0; j < numRC; j++)
        {
            printf("%.2f ", matrix[i][j]);
        }

        printf(" | %.2f", vector[i]);
        printf(" -> %.2f\n", solutions[i]);
    }
}

void printInfos(int numRC, float **matrix, float *vector)
{
    for (int i = 0; i < numRC; i++)
    {
        for (int j = 0; j < numRC; j++)
        {
            printf("%.2f ", matrix[i][j]);
        }

        printf(" | %.2f\n", vector[i]);
    }
}
