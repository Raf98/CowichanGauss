#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>

void printInfos(int numRC, float **matrix, float *vector);
void printResults(int numRC, float **matrix, float *vector, float* solutions);
void forwardElimination(int numRC, float **matrix, float *vector);
float *backwardSubstitution(int numRC, float **matrix, float *vector);

int main(int argc, char const *argv[])
{
    int i, j, k, numRC;

    //read number of rows and columns
    numRC = 3;

    srand(time(NULL));

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

    /* 		
    //matriz deve ser diagonal dominante e simetrica
    for (i = 0; i < numCols; i++)
    {
        for (j = 0; j < numCols; j++)
        {
            matrix[i][j] = rand % 128;
        }
    }
    */
    int init[3] = {76, 40, 91};

    for (i = 0; i < numRC; i++)
    {
        vector[i] = init[i]; //rand() % 128;
        printf("%f\n", vector[i]);
    }

    printInfos(numRC, matrix, vector);

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

    for (int k = 0; k < numRC-1; k++)
    {
        //itera sobre as linhas da matriz restantes a serem normalizadas, para formar a diagonal de 0s
        for (int i = k + 1; i < numRC; i++)
        {
            if (matrix[i][k] == 0) //se a coluna for zero, a pula
                continue;

            //encontra o fator de multiplicação a ser utilizado de forma a zerar os valores das colunas
            //que estejam na linha i ou abaixo. Usa o índice k pois esse é o indice da coluna e da linha
            //anteriores que serão usados para realizar as operações de subtração e multiplicação necesarias
            //para formar a diagonal inferior de 0s
            multiplyFactor = matrix[k][k] / matrix[i][k];

            //normaliza todas as colunas de indice i para 0 e realiza o mesmo calculo para as demais
            //colunas, não necessariamente as anulando
            for (int j = k; j < numRC; j++)
            {
                matrix[i][j] = matrix[k][j] - multiplyFactor * matrix[i][j];
            }

            //realiza a mesma operação para o vetor de resultados
            vector[i] = vector[k] - multiplyFactor * vector[i];
        }
    }
}

float *backwardSubstitution(int numRC, float **matrix, float *vector)
{
    float *solutions = new float[numRC];
    float sumMatrix = 0;

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
