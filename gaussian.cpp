#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <chrono>
#include "utils.hpp"

void forwardElimination(int numRC, double **matrix, double *vector);
double *backwardSubstitution(int numRC, double **matrix, double *vector);

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

    for (int index = 0; index < 100; index++)
    {
        file.open(fileName, std::fstream::in | std::fstream::out | std::fstream::app);

        std::string line;

        std::getline(file, line);
        numRC = std::atoi(line.c_str());

        matrix = new double *[numRC];
        for (int i = 0; i < numRC; i++)
            matrix[i] = new double[numRC];

        vector = new double[numRC];

        utils::readInputFile(file, numRC, vector, matrix);
        //utils::printInfos(numRC, matrix, vector, "\t\t****************INITIAL MATRIX*********************");

        auto start = std::chrono::high_resolution_clock::now();

        forwardElimination(numRC, matrix, vector);
        solutions = backwardSubstitution(numRC, matrix, vector);

        auto end = std::chrono::high_resolution_clock::now();
        auto execTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        //utils::printResults(numRC, matrix, vector, solutions);

        //std::cout << "Execution Time: " << execTime.count() << std::endl;
        utils::writeOutputFile(file, execTime, fileName, "Seq", index, numRC, vector, matrix, solutions);
    }

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

    for (int k = 0; k < numRC - 1; k++)
    {
        //itera sobre as linhas da matriz restantes a serem normalizadas, para formar a diagonal de 0s
        for (int i = k + 1; i < numRC; i++)
        {
            if (matrix[i][k] == 0) //se a coluna for zero, a pula
                continue;

            //a[1][0] = a[1][0] - (a[1][0]/a[0][0])*(a[0][0])

            //encontra o fator de multiplicação a ser utilizado de forma a zerar os valores das colunas
            //que estejam na linha i ou abaixo. Usa o índice k pois esse é o indice da coluna e da linha
            //anteriores que serão usados para realizar as operações de subtração e multiplicação necesarias
            //para formar a diagonal inferior de 0s
            multiplyFactor = matrix[i][k] / matrix[k][k];

            //normaliza todas as colunas de indice i para 0 e realiza o mesmo calculo para as demais
            //colunas, não necessariamente as anulando
            for (int j = k; j < numRC; j++)
            {
                matrix[i][j] = matrix[i][j] - multiplyFactor * matrix[k][j];
            }

            //realiza a mesma operação para o vetor de resultados
            vector[i] = vector[i] - multiplyFactor * vector[k];

            //utils::printInfos(numRC, matrix, vector, "PARTIAL MATRIX");
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
