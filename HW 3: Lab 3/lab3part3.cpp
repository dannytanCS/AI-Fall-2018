#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <tuple>
#include <sstream>
#include<math.h>

using namespace std;

//use the key and ans to find the path of the answer
vector<char> path(float numOfVertices, map<int, tuple<char, int>> key, vector<int> ans) {
    //create a vector with number of vertices
    vector<char> res(numOfVertices);
    for(int answer: ans) {
        tuple<char, int> val = key[answer];
        char letter = get<0>(val); //letter 
        char indice = get<1>(val); //it respective index
        //format the letter base on index
        res[indice - 1] = letter;
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

    //key which maps number to letter and incide
    map<int, tuple<char, int>> key = {};

    //number that are true
    vector<int> ans = {};

    bool noSolution = false;

    string line;
    //check if we get to the key part
    bool end = false;

     while (getline(inputFile, line)) {
         //output no solution
         if (line == "NO SOLUTION") {
             noSolution = true;
             break;
         }
        istringstream iss (line);
        
        int num;
        //true or false
        char valid;

        

        while (!end && iss >> num) {
            //break when it is 0
            if (num == 0) {
                end = true;
                break;
            }
            iss >> valid;
            //we only care about the truth number
            if (valid == 'T') {
                ans.push_back(num);
            }
        }

        if (end) {
            //the key
            int num2;
            char letter;
            int indice;
            while (iss >> num2>> letter >> indice) {
                tuple<char, int> letterIndice = {letter, indice};
                key [num2] = letterIndice;
            }
        }
    }


     inputFile.close();
     //output file
    ofstream outputFile("text.txt");
    if (!outputFile){
        cerr << "Could not open output file" << endl;
        return -9;
    }

     if (!noSolution) {
         //there is a solution
          float numOfLetters = sqrt(key.size());
          vector<char> res = path(numOfLetters, key, ans);
        
          for(char letter: res) {
              outputFile << letter << " ";
          }
     }

     else {
        //there is no solution
        outputFile << "NO SOLUTION" << endl;
     }
    outputFile.close();
}