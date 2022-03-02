#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

int n;
std::vector<std::vector<int>> G;

void parseFile(std::string fileName)
{
    std::ifstream file(fileName);

    std::string line;
    // Read one line at a time into the variable line:
    while (std::getline(file, line))
    {
        std::vector<int> lineData;
        std::stringstream lineStream(line);

        int value;
        // Read an integer at a time from the line
        while (lineStream >> value)
        {
            // Add the integers from a line to a 1D array (vector)
            lineData.push_back(value);
        }
        // When all the integers have been read, add the 1D array
        // into a 2D array (as one line in the 2D array)
        if (lineData.size() == 1)
        {
            n = lineData[0];
        }
        else
        {
            G.push_back(lineData);
        }
    }
}

int main(int argc, char const *argv[])
{
    std::string fileName = "./data/graf_10_5.txt";

    parseFile(fileName);

    std::cout << "n = " << n << std::endl;
    std::cout << "G = " << std::endl;
    for (int i = 0; i < G.size(); i++)
    {
        for (int j = 0; j < G[i].size(); j++)
        {
            std::cout << G[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // std::cout << "Hello World! " << argc << " " << argv << std::endl;
    return 0;
}
