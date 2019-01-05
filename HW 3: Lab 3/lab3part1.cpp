#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <set>

using namespace std;

//the form key at the bottom
string formKey(int vertices) {
    string res = "0\n";
    for (int i = 0; i < vertices; i++) {
        for (int j = 1; j < vertices + 1; j++) {
            char letter = 'A' + i;
            int num = i * vertices + j;
            res += to_string(num) + " ";
            res += letter;
            res += " " + to_string(j) + "\n";
        }
    }
    return res;
}

//case 1: Every vertex is traversed at some time

string case1(int vertices) {
    string res = "";
    for(int i = 0; i < vertices; i++) {
        for (int j = 1; j < vertices + 1; j++) {
            int num = i * vertices + j;
            res += to_string(num) + " ";
        }
        res += "\n";
    }
    return res;
}

//get the list of all the vertices
set<char> setOfVertex(int vertices) {
    set<char> res = {};
    for(int i = 0; i < vertices; i++) {
        char letter = 'A' + i;
        res.insert(letter);
    }
    return res;
}

//case 2: No pair of vertices are traversed at the same time

string case2(int vertices) {
    string res = "";
    //get all of vertices
    set<char> setOfVertices = setOfVertex(vertices);
    //for each letter
    for(char letter: setOfVertices) {
        //for each pair of letters, no repeat so we need to loop from the difference of letter and 'A'
        int difference = letter - 'A';
        for(int i = difference + 1; i < vertices; i++) {
            //go through all of the indices for each letter
            for(int j = 1; j < vertices + 1; j++) {
                int numberOfCurrentVertice = -1* (( letter - 'A' ) * vertices + j); // ~X1 ... ~Xn
                int numberOfAdjVertice = -1*((i) * vertices + j); // ~Y1 ... ~Yn
                res += to_string(numberOfCurrentVertice) + " " + to_string(numberOfAdjVertice) + "\n";
            }
        }
    }
    return res;
}


//returns the non adj for the vertex
map<char, vector<char>> notVertexAdj(int vertices, map<char, vector<char>> adjList) {
    map<char, vector<char>> nonAdj = {};
    set<char> setOfVertices = setOfVertex(vertices);
    for(auto val: adjList) {
        nonAdj[val.first] = {};
        setOfVertices.erase(val.first);
        for (int i = 0; i < vertices; i++) {
            char letter = 'A' + i;
            
            //if it match itself go to the next one
            if (letter == val.first){
                continue;
            }
         
            //if it matches with any from the adj list
            //go to the next indice 
            if (find(val.second.begin(), val.second.end(), letter) != val.second.end()) {
                continue;
            }
            else {
                //not found add to non adj
                nonAdj[val.first].push_back(letter);
            }
        }
    }
    //set the letter that is not connected
    for(char letterRemain: setOfVertices) {
        nonAdj[letterRemain] = {};
        for(int i = 0; i < vertices; i++) {
            char letter = 'A' + i;
            //dont include itself
            if (letter == letterRemain){
                continue;
            }
            else {
                nonAdj[letterRemain].push_back(letter);
            }
        }
    }
    return nonAdj;
}



//case 3: You cannot go from U at time T to W at time T+1 if there is no edge from U to W
string case3(int vertices, map<char, vector<char>> notVertexAdj) {
    string res = "";
    for(auto val: notVertexAdj) {
        for (int i = 0; i < val.second.size(); i++) {
            //add the current vertex from 1 to n-1 and its non adj from 2 to n respectively
            for(int j = 1; j < vertices; j++) {
                int numberOfCurrentVertice = -1* (( val.first - 'A' ) * vertices + j); // ~X1 ... ~Xn-1
                int numberOfAdjVertice = -1*((val.second[i] - 'A') * vertices + j + 1); // ~Y2 ... ~Yn
                res += to_string(numberOfCurrentVertice) + " " + to_string(numberOfAdjVertice) + "\n";
            }
        }
    }
    return res;
}


int main() {
    //calls the input file
    ifstream inputFile("text.txt");
    if (!inputFile){
        cerr << "Could not open input file" << endl;
        return -9;
    }

    int vertices;
    char vertex1;
    char vertex2;
    map<char, vector<char>> vertexAdj = {};

    //get the number of vertices
    inputFile >> vertices;
    //parse the file for vertices that are connected
    while (inputFile >> vertex1 >> vertex2) {
        if (vertexAdj.find(vertex1) != vertexAdj.end()) {
            //append to the adj list if the vertex exist in the hash
            vertexAdj[vertex1].push_back(vertex2);
        }
        else {
            //create a new adj list if the vertex doesn't exist in the hash
            vertexAdj[vertex1] = {vertex2};
        }
    }
    //do all the different cases
    string caseOne = case1(vertices);
    string caseTwo = case2(vertices);
    map<char, vector<char>> nonAdj = notVertexAdj(vertices, vertexAdj);
    string caseThree = case3(vertices, nonAdj); 
    string keyMap = formKey(vertices);

    inputFile.close();


    //output file
    ofstream outputFile("text.txt");
    if (!outputFile){
        cerr << "Could not open output file" << endl;
        return -9;
    }



    //write to output file
    outputFile << caseOne << endl;
    outputFile << caseTwo << endl;
    outputFile << caseThree << endl;
    outputFile << keyMap << endl;
    outputFile.close();
}