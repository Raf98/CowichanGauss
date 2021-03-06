#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

namespace utils
{

    // Implementa um metodo de split para strings do C++
    std::list<std::string> split(std::string str, char delimiter)
    {
        const char *cStr = str.c_str();

        std::list<std::string> strList;
        std::string aux;

        //std::cout << str << std::endl;

        for (int i = 0; i < str.size(); i++)
        {
            if (cStr[i] != delimiter)
            {
                if (cStr[i] != ' ' && cStr[i] != '\n') // Ignora espaços
                {
                    aux += cStr[i];

                    if (i == str.size() - 1)
                    {
                        strList.push_back(aux);
                        aux.clear();
                        return strList;
                    }
                }
                else
                {
                    if (i == str.size() - 1 && cStr[i] == ' ')
                    {
                        strList.push_back(aux);
                        aux.clear();
                        return strList;
                    }
                }
            }
            else
            {
                strList.push_back(aux);
                aux.clear();
            }
        }

        return strList;
    };

    void printResults(int numRC, double **matrix, double *vector, double *solutions)
    {
        std::cout << "\t\t****************FINAL MATRIX*********************" << std::endl;
        for (int i = 0; i < numRC; i++)
        {
            for (int j = 0; j < numRC; j++)
            {
                printf("%.2f ", matrix[i][j]);
            }

            printf(" | %.2f", vector[i]);
            printf(" -> %.5f\n", solutions[i]);
        }
    }

    void printInfos(int numRC, double **matrix, double *vector, const char *message)
    {
        printf("%s\n\n", message);

        for (int i = 0; i < numRC; i++)
        {
            for (int j = 0; j < numRC; j++)
            {
                printf("%.2f ", matrix[i][j]);
            }

            printf(" | %.2f\n", vector[i]);
        }

        printf("\n\n");
    }

    void readInputFile(std::fstream &file, int numRC, double *vector, double **matrix)
    {
        /*
        std::cout << "\t*****************INITIALIZING MATRIX AND VECTOR*********************"
                  << "\n\n";

        std::cout << "NUMBER OF ROWS AND COLUMNS: " << numRC << "\n\n";
        */
        std::string line;

        //std::cout << "MATRIX:" << std::endl;

        for (int i = 0; i < numRC; i++)
        {
            std::getline(file, line);
            std::list<std::string> strNumbers = utils::split(line, ',');
            for (int j = 0; j < numRC; j++)
            {
                matrix[i][j] = std::atof(strNumbers.front().c_str());
                //std::cout << matrix[i][j] << "  ";
                strNumbers.pop_front();
            }

            //std::cout << std::endl;
        }

        //std::cout << "VECTOR:" << std::endl;

        for (int i = 0; i < numRC; i++)
        {
            std::getline(file, line);
            vector[i] = std::atof(line.c_str());
            //std::cout << vector[i] << "\t";
        }
        //std::cout << std::endl;

        file.close();
    }

    void writeOutputFile(std::fstream &file, std::chrono::_V2::high_resolution_clock::duration execTime,
                         std::string inputFileName, std::string trialType, int index, int numRC, double *vector,
                         double **matrix, double *solutions)
    {
        std::string outputFileName = "output_" + trialType + "_" + std::to_string(numRC) + "_" + inputFileName;

        file.open(outputFileName, std::fstream::in | std::fstream::out | std::fstream::app);

        //file << "Execution time" + std::to_string(index) + ": " << execTime.count() << " ns"
        //     << "\n\n";

        file << execTime.count() << ", ";

        /*
        for (int i = 0; i < numRC; i++)
        {
            for (int j = 0; j < numRC; j++)
            {
                file << matrix[i][j] << " ";
            }

            file << " | " << vector[i];
            file << " -> " << solutions[i] << "\n\n";
        }

        file << "\n\n\n";
        */

        file.close();
    }

    void writeOutputFile(std::fstream &file, double *execTimeVector,
                         std::string inputFileName, std::string trialType, int numRC, int vectorSize)
    {
        std::string outputFileName = "output_" + trialType + "_" + std::to_string(numRC) + "_" + inputFileName;

        file.open(outputFileName, std::fstream::in | std::fstream::out | std::fstream::app);

        //std::cout << outputFileName << std::endl;

        for (int i = 0; i < vectorSize; i++)
        {
            file << execTimeVector[i] << ", ";
           // std::cout << "Execution time " + std::to_string(i) + ": " << execTimeVector[i] << " ns"
           //       << "\n\n";
        }

        file.close();
    }

    void writeOutputFile(std::fstream &file, double execTime,
                         std::string inputFileName, std::string trialType, int index, int numRC, double *vector,
                         double **matrix, double *solutions)
    {
        std::string outputFileName = "output_" + trialType + "_" + std::to_string(numRC) + "_" + inputFileName;

        file.open(outputFileName, std::fstream::in | std::fstream::out | std::fstream::app);

        std::cout << outputFileName << std::endl;

        std::cout << "Execution time " + std::to_string(index) + ": " << execTime << " ns"
                  << "\n\n";

        file << execTime << ", ";

        file.close();
    }

} // namespace utils