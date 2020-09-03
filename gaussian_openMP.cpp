#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <list>

#include <omp.h>

#include "utils.hpp"

void forwardElimination(int numRC, double **matrix, double *vector);
double *backwardSubstitution(int numRC, double **matrix, double *vector);
void readInputFile(std::fstream &file, int numRC, double *vector, double **matrix);

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
    std::string fileName = argv[1];

    double **matrix;
    double *vector;
    double *solutions;

    double **matrixOriginal;
    double *vectorOriginal;

    file.open(fileName, std::fstream::in | std::fstream::out | std::fstream::app);

    std::string line;

    std::getline(file, line);
    numRC = std::atoi(line.c_str());

    matrixOriginal = new double *[numRC];
    for (int i = 0; i < numRC; i++)
        matrixOriginal[i] = new double[numRC];

    vectorOriginal = new double[numRC];

    utils::readInputFile(file, numRC, vectorOriginal, matrixOriginal);

    matrix = new double *[numRC];
    for (int i = 0; i < numRC; i++)
        matrix[i] = new double[numRC];

    vector = new double[numRC];

    const int NUM_TESTS = 100;

    double *execTimeVector = new double[NUM_TESTS];

    for (int index = 0; index < NUM_TESTS; index++)
    {
        for (int i = 0; i < numRC; i++)
        {
            for (int j = 0; j < numRC; j++)
            {
                matrix[i][j] = matrixOriginal[i][j];
            }
            vector[i] = vectorOriginal[i];
        }

        //utils::printInfos(numRC, matrix, vector, "\t\t****************INITIAL MATRIX*********************");

        //double start = omp_get_wtime();
        auto start = std::chrono::high_resolution_clock::now();

        forwardElimination(numRC, matrix, vector);
        solutions = backwardSubstitution(numRC, matrix, vector);

        //double end = omp_get_wtime();
        //double execTime = end - start;

        auto end = std::chrono::high_resolution_clock::now();
        auto execTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        execTimeVector[index] = execTime.count();

        //utils::printResults(numRC, matrix, vector, solutions);

        //std::cout << "Execution Time: " << execTime << std::endl;
        //std::cout << "Execution Time: " << execTime.count() << std::endl;
        //utils::writeOutputFile(file, execTime, fileName, "Pthreads", index, numRC, vector, matrix, solutions);
    }

    utils::writeOutputFile(file, execTimeVector, fileName, "OpenMP", numRC, NUM_TESTS);

    for (int i = 0; i < numRC; i++)
        delete[] matrix[i];

    delete[] matrix;
    delete[] vector;
    delete[] solutions;

    delete[] execTimeVector;

    return 0;
}

void forwardElimination(int numRC, double **matrix, double *vector)
{
    double multiplyFactor = 0;

    int i = 0, j = 0;

    int numThreads = numRC;

    for (int k = 0; k < numRC - 1; k++)
    {
        i = 0;
        j = 0;

#pragma omp parallel for shared(matrix, vector) private(i, multiplyFactor) schedule(static, 1) num_threads(numThreads)
        for (i = k + 1; i < numRC; i++)
        {

            //printf("%d\n", omp_get_num_threads());

            if (matrix[i][k] != 0)
            {
                //#pragma omp critical
                multiplyFactor = matrix[i][k] / matrix[k][k];
                vector[i] -= multiplyFactor * vector[k];

                //#pragma omp parallel for shared(matrix) private(j) schedule(static, 1) //num_threads(numThreads)
                for (int j = k; j < numRC; j++)
                {
                    matrix[i][j] -= multiplyFactor * matrix[k][j];
                }
            }
        }
    }
}

double *backwardSubstitution(int numRC, double **matrix, double *vector)
{
    double *solutions = new double[numRC];
    double sumMatrix = 0;

    //faz o cálculo das soluções do exercício
    for (int i = numRC - 1; i >= 0; i--)
    {
        sumMatrix = 0;

        //soma os indices das colunas multiplicados pelos valores de soluções já encontrados,
        //de forma a descobrir os novos valores de soluções
        for (int j = i + 1; j <= numRC - 1; j++)
        {
            sumMatrix += matrix[i][j] * solutions[j];
        }

        //encontra a solução substraindo do vetor de resultados a soma dos indices multiplicados pelos valores de soluções
        //ja encontradas da matriz, e os divide pelo coeficiente da soluçao a ser encontrada, a qual fica localizada na linha
        //e coluna i
        solutions[i] = (vector[i] - sumMatrix) / matrix[i][i];
    }

    return solutions;
}
