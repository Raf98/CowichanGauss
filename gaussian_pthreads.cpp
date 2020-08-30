#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <pthread.h>
#include "utils.hpp"

void forwardElimination(int numRC, double **matrix, double *vector);
double *backwardSubstitution(int numRC, double **matrix, double *vector);
void *normalize(void *Data);
void *normalizeMatrix(void *Data);

//modelo de dados a ser paralelizado
class Data
{
public:
    int i, j, k, numRC;
    double multiplyFactor;
    double **matrix;
    double *vector;

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
    if (argc < 2)
    {
        std::cout << "ERROR! Program should be executed like this:" << std::endl;
        std::cout << "./<executableName> <inputFileName>" << std::endl;
        exit(1);
    }

    int i, j, k, numRC;

    std::fstream file;

    file.open(argv[1], std::fstream::in | std::fstream::out | std::fstream::app);

    std::string line;

    std::getline(file, line);
    numRC = std::atoi(line.c_str());

    double **matrix = new double *[numRC];
    for (int i = 0; i < numRC; i++)
        matrix[i] = new double[numRC];

    double *vector = new double[numRC];

    utils::readInputFile(file, numRC, vector, matrix);
    utils::printInfos(numRC, matrix, vector, "\t\t****************INITIAL MATRIX*********************");

    pthread_mutex_init(&mutex, NULL);

    //realiza o escalonamento reduzido da matriz; é a fase de eliminação
    forwardElimination(numRC, matrix, vector);
    utils::printInfos(numRC, matrix, vector, "\t****************FORWARD ELIMINATION MATRIX*********************");

    double *solutions = backwardSubstitution(numRC, matrix, vector);
    utils::printResults(numRC, matrix, vector, solutions);

    // desalocar memoria usando o operador delete[]
    for (int i = 0; i < numRC; i++)
        delete[] matrix[i];

    delete[] matrix;
    delete[] vector;
    delete[] solutions;

    return 0;
}

void forwardElimination(int numRC, double **matrix, double *vector)
{
    double multiplyFactor = 0;

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
                countI++;
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
            //std::cout << "Created row thread " << i << std::endl;
        }

        for (int i = 0; i < numRT; i++)
        {
            pthread_join(rowsThreads[i], NULL);
            //std::cout << "Synchronizing row thread " << i << "\n\n";
        }

        for (int i = 0; i < numCT; i++)
        {
            for (int j = k; j < numRC; j++)
            {
                pthread_create(&(columnsThreads[j]), NULL, normalizeMatrix, (void *)&dataM[i][j]);
                //std::cout << "Created column thread " << i << " " << j << std::endl;
            }

            for (int j = k; j < numRC; j++)
            {
                pthread_join(columnsThreads[j], NULL);
                //std::cout << "Synchronizing column thread " << j << "\n\n";
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

    pthread_mutex_lock(&mutex);
    data->vector[i] = data->vector[k] - data->multiplyFactor * data->vector[i];
    pthread_mutex_unlock(&mutex);
}

void *normalizeMatrix(void *dataFormal)
{
    Data *data = (Data *)dataFormal;

    int i = data->i;
    int j = data->j;
    int k = data->k;

    pthread_mutex_lock(&mutex);
    data->matrix[i][j] = data->matrix[k][j] - data->multiplyFactor * data->matrix[i][j];
    pthread_mutex_unlock(&mutex);
}

double *backwardSubstitution(int numRC, double **matrix, double *vector)
{
    double *solutions = new double[numRC];
    double sumMatrix = 0;

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
