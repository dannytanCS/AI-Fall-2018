//
//  AI Lab1.cpp
//  
//
//  Created by Danny Tan on 9/8/18.
//
//

#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <tuple>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;


//define the node of the parse tree
class Tree {
public:
    //constructor
    Tree (string phrase, int startPhrase, int endPhrase, string word, Tree* left, Tree* right, double prob):phrase(phrase),startPhrase(startPhrase), endPhrase(endPhrase), word(word), left(left), right(right), prob(prob){}
    
    //get probability
    float getProb() const {
        return prob;
    }
    
    //set probability
    
    void setProb(float prob) {
        this->prob = prob;
    }
    
    //get left tree
    
    Tree* getLeft() const {
        return left;
    }
    
    //set left tree
    void setLeft(Tree* left) {
        this->left = left;
    }
    
    
    //get right tree
    
    Tree* getRight() const {
        return right;
    }
    
    
    //set right tree
    void setRight(Tree* right) {
        this->right = right;
    }
    
    string getPhrase() const {
        return phrase;
    }
    
    int getStart() const {
        return startPhrase;
    }
    
    int getEnd() const {
        return endPhrase;
    }
    
    string getWord() const {
        return word;
    }

    
private:
    string phrase; // the part of speech
    int startPhrase; // indices starting word
    int endPhrase; //indices of ending word
    string word;
    Tree* left;
    Tree* right;
    double prob;
};


//removes [] and formats probability into a float
double formatProbability(string probability) {
    probability.erase(probability.begin());
    probability.erase(probability.end()-1);
    return stod(probability);
}

//put all of the grammar from file into a map
map<string, vector<tuple<string, double>>> grammarFileIntoMap() {
    map<string, vector<tuple<string, double>>> rules;
    
    //calls the grammar file
    ifstream grammarFile("grammar.txt");
    if (!grammarFile){
        cerr << "Could not open file" << endl;
    }
    
    string line;
    //parse through the grammar file and store rules into a map (word => list of (pos, probability))
    while (getline(grammarFile, line)) {
        istringstream lineStream(line);
        
        string pos, word, secondWord, arrow, probability;
        
        if (lineStream >> pos >> arrow >> word) {
            //non terms
            if (lineStream >> secondWord >> probability) {
                tuple<string, double> values = {pos, formatProbability(probability)};
                if ( rules.find(word+secondWord) == rules.end() ) {
                    // not found
                    rules[word+" "+secondWord] = {values};
                } else {
                    // found
                    rules[word+secondWord].push_back(values);
                }
            }
            
            //regular part of speech
            else {
                tuple<string, double> values = {pos, formatProbability(secondWord)};
                if ( rules.find(word) == rules.end() ) {
                    rules[word] = {values};
                }
                else {
                    rules[word].push_back(values);
                }
            }
        }
    }
    return rules;
}

//split the sentence into words
vector<string> sentenceIntoWords(string sentence) {
    vector<string> words = {};
    string delimiter = " ";
    
    string word = "";
    for (auto x : sentence)
    {
        if (x == ' ')
        {
            words.push_back(word);
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    return words;
}



map<tuple <string, int, int>, Tree*> parser(string sentence, map<string, vector<tuple<string, double>>> rules) {
    map< tuple<string, int, int>, Tree*> chart = {};
    string output = "";
    vector<string> words = sentenceIntoWords(sentence);
    //loop through the words in the sentence
    for (int i = 0; i < words.size(); i++) {
        string word = words[i];
        //find the word in the rules
        if(rules.find(word) != rules.end()) {
            //create a Tree node for different part of speech of the word
            for(tuple<string,double>& PoS: rules[word]) {
                tuple<string,int,int> key = {get<0>(PoS), i, i};
                Tree* tree = new Tree(get<0>(PoS),i,i,word,nullptr,nullptr,get<1>(PoS));
                chart[key] = tree;
            }
            
        }
        else {
            //delete all of the pointers
            for(auto& s: chart) {
                delete s.second;
            }
            chart.clear();
            return chart;
        }
    }

    //length of phrase
    for(int length = 1; length < words.size() + 1; length++) {
        //start of phrase
        for (int i = 0; i < words.size() - length + 1; i++) {
            //end of phrase
            int j = i+length -1;
            
            
            //initialized for all non terms
            
            for(auto& t: rules) {
                string association = t.first;
                //find all of the nonTerms
                
                
                
                //add a whitespace to the end to split the last word
                vector<string> nonTermKey = sentenceIntoWords(association + " ");
                
                if (nonTermKey.size() > 1) {
                    string nonTerm = get<0>(t.second[0]);
                    chart[{nonTerm, i, j}] = new Tree(nonTerm, i, j, "", nullptr, nullptr, 0.0);
                }
            }
        }
    }
    
    //length of phrase
    for(int length = 1; length < words.size() + 1; length++) {
        //start of phrase
        for (int i = 0; i < words.size() - length + 1; i++) {
            //end of phrase
            int j = i+length -1;
            for(int k = i; k < j; k++) {
                for(auto& t: rules) {
                    string association = t.first;
                    vector<string> nonTermKey = sentenceIntoWords(association + " ");
                            
                    //compute for non terms
                    if (nonTermKey.size() > 1) {
                        //current probability
                        double prob = get<1>(t.second[0]);
                        //non term
                        auto nonTerm = get<0>(t.second[0]);
                        
                        //its association
                        string Y = nonTermKey[0];
                        string Z = nonTermKey[1];
                        
                        
                        double newProb = 0;
                        
                        //only compute new probability if the probability of its associations exist, else it is 0
                        if (chart.find({Y,i,k}) != chart.end() && chart.find({Z, k+1, j}) != chart.end() ) {
                        
                            //check probability
                            newProb = chart[{Y, i, k}]->getProb() * chart[{Z, k+1, j}]->getProb() * prob;
                        }
                    
                        
                        
                        //replace if greater than
                        if (newProb > chart[{nonTerm,i, j}]->getProb()) {
                            chart[{nonTerm, i, j}]->setLeft(chart[{Y, i, k}]);
                            chart[{nonTerm, i, j}]->setRight(chart[{Z, k+1, j}]);
                            chart[{nonTerm, i, j}]->setProb(newProb);
                        }
                    }
                }
            }
        }
    }
    return chart;
}


//print out the tree
void printTree1(Tree* tree, int indent) {
    if (tree != nullptr) {
        for (int i = 0; i < indent; i++) {
            cout << " ";
        }
        cout << tree -> getPhrase();
        if (tree->getWord() != "") {
            cout <<" " << tree->getWord();
        }
        cout << endl;
        //print left tree
        printTree1(tree->getLeft(), indent +3);
        //print right tree
        printTree1(tree->getRight(), indent +3);
    }
}


//print from S
void printTree(map<tuple <string, int, int>, Tree*> chart, int size) {
    printTree1(chart[{"S", 0, size}], 0);
}





int main(int argc, char *argv[]) {
    
    map<string, vector<tuple<string, double>>> rules = grammarFileIntoMap();
    
    
    //input sentence
    string sentence = "";
    
    for (int i = 1; i < argc; i++) {
        sentence += argv[i];
        sentence += ' ';
    }
    
    
    //convert to lower case
    transform(sentence.begin(), sentence.end(), sentence.begin(), ::tolower);
    
    map<tuple <string, int, int>, Tree*> chart = parser(sentence, rules);
    
    vector<string> words = sentenceIntoWords(sentence);
    
    //if one of the words are not valid
    if (chart.size() == 0) {
        cout << "This sentence cannot be parsed" << endl;
        return -1;
    }
    
    if (chart[{"S", 0, words.size() -1}] -> getProb() != 0.0) {
        printTree(chart, words.size()-1);
        cout << "Probability = " << chart[{"S", 0, words.size() -1}] -> getProb() << endl;
    }
    else {
        //cannot be parse if probability is 0
        cout << "This sentence cannot be parsed" << endl;
    }
    
}

