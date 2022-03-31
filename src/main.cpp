#include <vector>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <omp.h>

// Integer number representing the number of vertices of graph G, 100 >= n >= 10.
int n;
// Integer number, 5 =< a =< n/2.
int a;
// Connected undirected weighted graph with n vertices and average vertex degree k, weigths of edges are from interval <80,120>.
std::vector<std::vector<int>> G;
// Minimum edges cut weight sum
int minEdgeCut = INT32_MAX;
int bfsMaxLevel = 1;
// N recursion calls
int recursionCalls = 0;

struct Node
{
    int id;
    std::vector<bool> visitedNodes;
};
std::vector<Node> nodes;

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
    recursionCalls++;

    // Number of cuts reached
    if (cut == a)
    {
        int cutWeight = GetCutTotalWeights(visitedVertices);

        #pragma omp critical
        {
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
                int parallelCuts = cut == 1;

                #pragma omp task if (parallelCuts)
                {
                    DFS(i, cut + 1, visitedVertices, edgeCuts);
                }

                #pragma omp taskwait
                visitedVertices[i] = false;
            }
        }
    }
}

void BFS(int vertexId, int depth, std::vector<bool> &visitedVertices, std::vector<bool> &edgeCuts)
{
    std::queue<int> q;
    q.push(vertexId);
    visitedVertices[vertexId] = true;

    Node currentNode;
    for (int i = 0; i < n; i++)
    {
        currentNode.visitedNodes.push_back(false);
    }
    currentNode.id = vertexId;
    currentNode.visitedNodes[vertexId] = true;
    nodes.push_back(currentNode);

    std::vector<int> maxDepthNodes;

    while (depth < bfsMaxLevel)
    {
        int nElements = q.size();
        while (!q.empty() && nElements > 0)
        {
            int vertexId = q.front();
            q.pop();

            for (int i = 0; i < n; i++)
            {
                if (!visitedVertices[i] && G[vertexId][i] != 0)
                {
                    q.push(i);
                    visitedVertices[i] = true;
                    maxDepthNodes.push_back(i);

                    // Create a copy of the current visited node
                    Node visitedNode;
                    visitedNode.id = i;
                    for (int j = 0; j < n; j++)
                    {
                        visitedNode.visitedNodes.push_back(false);
                    }
                    visitedNode.visitedNodes[vertexId] = true;
                    visitedNode.visitedNodes[i] = true;

                    nodes.push_back(visitedNode);

                    for (int j = 0; j < n; j++)
                    {
                        if (currentNode.visitedNodes[j])
                        {
                            visitedNode.visitedNodes[j] = true;
                        }
                    }

                    if (depth + 1 < bfsMaxLevel)
                    {
                        maxDepthNodes.clear();
                    }
                }
                
            }
            nElements--;
            if (nElements == 0)
            {
                depth++;
            }  
        }
    }
    // DFS
    int tempMinCut = 0;
    #pragma omp parallel
    {
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < nodes.size(); i++)
        {
            for (int j = 0; j < maxDepthNodes.size(); j++)
            {
                if (nodes[i].id == maxDepthNodes[j])
                {
                    std::vector<bool> tempVisitedNodes;
                    for (int k = 0; k < n; k++)
                    {
                        tempVisitedNodes.push_back(nodes[i].visitedNodes[k]);
                    }
                    DFS(nodes[i].id, depth, tempVisitedNodes, edgeCuts);
                    
                }
                
            }
            
        }
        
    }
    
}

void PrintEdgeCutVertices(std::vector<bool> &edgeCuts)
{
    std::cout << "X = { ";
    for (int i = 0; i < n; i++)
    {
        if (edgeCuts[i])
        {
            std::cout << i << " ";
        }
    }
    std::cout << "}" << std::endl;
}

void PrintNonEdgeCutVertices(std::vector<bool> &edgeCuts)
{
    std::cout << "Y = { ";
    for (int i = 0; i < n; i++)
    {
        if (!edgeCuts[i])
        {
            std::cout << i << " ";
        }
    }
    std::cout << "}" << std::endl;
}

void PrintInputs()
{
    std::cout << "n = " << n << std::endl;
    std::cout << "a = " << a << std::endl;
    std::cout << "G = {" << std::endl;
    for (int i = 0; i < G.size(); i++)
    {
        std::cout << "\t{ ";
        for (int j = 0; j < G[i].size(); j++)
        {
            std::cout << G[i][j] << " ";
        }
        std::cout << "}" << std::endl;
    }
    std::cout << "}" << std::endl;
}

int main(int argc, char const *argv[])
{
    std::string fileName = "./data/graf_15_14.txt";
    a = atoi(argv[argc - 1]);
    ParseGraphFile(fileName);

    // Create array of visited vertices and cuts
    std::vector<bool> isVertexVisited(n, false);
    std::vector<bool> isEdgeCut(n, false);

    // Begin Timer
    std::chrono::high_resolution_clock::time_point timerStart = std::chrono::high_resolution_clock::now();

    if (a < bfsMaxLevel)
    {
        #pragma omp parallel
        {
            #pragma omp single
            DFS(0, 0, isVertexVisited, isEdgeCut);
        }
    }
    else
    {
        std::queue<int> bfsNodes;
        bfsNodes.push(0);
        bfsNodes.push(-1);
        BFS(0, 0, isVertexVisited, isEdgeCut);
    }

    // End Timer
    std::chrono::high_resolution_clock::time_point timerEnd = std::chrono::high_resolution_clock::now();

    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(timerEnd - timerStart);

    // std::cout<<"AAAAAAAAAA"<< std::endl;
    // std::cout << "{ ";
    // for (int i = 0; i < n; i++)
    //     {

    //         std::cout << isVertexVisited[i] << " ";

    //     }
    //     std::cout << "}" << std::endl;

    // PrintEdgeCutVertices(isEdgeCut);

    std::cout << "MinEdgeCut = " << minEdgeCut << std::endl;
    std::cout << "Time = " << duration.count() << " ms" << std::endl;

    // std::cout << "Recursion calls = " << recursionCalls << std::endl;

    return 0;
}
