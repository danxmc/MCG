#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Integer number representing the number of vertices of graph G, 100 >= n >= 10.
int n;
// Integer number, 5 =< a =< n/2.
int a;
// Connected undirected weighted graph with n vertices and average vertex degree k, weigths of edges are from interval <80,120>.
std::vector<std::vector<int>> G;
// Minimum edges cut weight sum
int minEdgeCut = INT32_MAX;

void ParseGraphFile(std::string fileName)
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

int GetCutTotalWeights(std::vector<bool> &visitedVertices)
{
    // Edges between a cut
    int intersectionEdgesWeights = 0;
    int excludedEdgesWeights = 0;

    // Get edge weights for connected vertices
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (visitedVertices[i])
            {
                if (visitedVertices[j])
                {
                    intersectionEdgesWeights += G[i][j];
                }
                if (G[i][j] != 0)
                {
                    excludedEdgesWeights += G[i][j];
                }
            }
        }
    }

    return (excludedEdgesWeights - intersectionEdgesWeights);
}

void DFS(int vertexId, int cut, std::vector<bool> &visitedVertices, std::vector<bool> &edgeCuts)
{
    // Number of cuts reached
    if (cut == a)
    {
        int cutWeight = GetCutTotalWeights(visitedVertices);

        // Override current minCutWeight sum if better
        if (cutWeight < minEdgeCut)
        {
            minEdgeCut = cutWeight;

            // Mark an edgeCut if we have already visited the vertex
            for (int i = 0; i < n; i++)
            {
                edgeCuts[i] = visitedVertices[i];
            }
        }
    }
    else
    {
        // Mark vertex as visited
        visitedVertices[vertexId] = true;

        // Check current vertex connections to other vertices (edges)
        for (int i = 0; i < n; i++)
        {
            // Check if there's a connection between a vertex and it hasn't been visited
            if ((!visitedVertices[i]) && G[vertexId][i] != 0)
            {
                // Add a cut recursively
                DFS(i, cut + 1, visitedVertices, edgeCuts);

                visitedVertices[i] = false;
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    std::string fileName = "./data/graf_10_5.txt";
    a = atoi(argv[argc - 1]);
    ParseGraphFile(fileName);

    // std::cout << "n = " << n << std::endl;
    // std::cout << "a = " << a << std::endl;
    // std::cout << "G = {" << std::endl;
    // for (int i = 0; i < G.size(); i++)
    // {
    //     std::cout << "\t{ ";
    //     for (int j = 0; j < G[i].size(); j++)
    //     {
    //         std::cout << G[i][j] << " ";
    //     }
    //     std::cout << "}" << std::endl;
    // }
    // std::cout << "}" << std::endl;

    // Create array of visited vertices and cuts
    std::vector<bool> isVertexVisited(n, false);
    std::vector<bool> isEdgeCut(n, false);

    DFS(0, 0, isVertexVisited, isEdgeCut);

    std::cout << "MinEdgeCut = " << minEdgeCut << std::endl;

    // std::cout << "Hello World! " << argc << " " << argv[argc - 1] << std::endl;
    return 0;
}
