#include <vector>
#include <string>
#include <stack>
#include <iostream>
#include <fstream>
#include <map>
#include <tuple>
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
    ifstream grammarFile("input.txt");
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
string printTree1(Tree* tree) {
    string ans = "";
    if (tree != nullptr) {
        ans += tree -> getPhrase();
        if (tree->getWord() != "") {
            ans += " " + tree->getWord();
        }
        ans += " ";
        //print left tree
        ans += printTree1(tree->getLeft());
        //print right tree
        ans += printTree1(tree->getRight());
    }
    return ans;
}


//print from S
string printTree(map<tuple <string, int, int>, Tree*> chart, int size) {
    return printTree1(chart[{"S", 0, size}]);
}


stack<string> wordStack(string sentence) {
    stack<string> words= {};
    string word = "";
    //adding each word in the sentence to the stack
    for(int i = 0; i < sentence.size(); i++) {
        //if theres a space, means a new word
        if (sentence[i] == ' ' ) {
            words.push(word);
            word = "";
        }
        else {
            word += sentence[i];
        }
    }
    //adds the last word
    if (word != "") {
        words.push(word);
    }
    return words;
}


void addToCountMap(string word, map<string,int>& countOfText) {
    //check if exist in map
    if (countOfText.find(word) != countOfText.end()) {
        countOfText[word] ++;
    }
    else {
        //create instant if doesn't  exist
        countOfText[word] = 1;
    }
}

//check if the association is finished, if finished add to map
void finishAssociation(map< vector<string> , int>& countOfAssociation, vector<string>& association) {
    //check if exist in map
    if (countOfAssociation.find(association) != countOfAssociation.end()) {
        countOfAssociation[association]++;
    } 
    else {
        countOfAssociation[association] = 1;
    }

    association.clear();
}

//compute the probability of each association
map<vector<string>, float> getProbability(map< vector<string> , int> countOfAssociation,  map<string, int> countOfText) {

    map<vector<string>, float> ans = {};

    for(auto const& association: countOfAssociation) {
        //count of the association
        float count = association.second;
        //count of the text
        float total = countOfText[association.first[0]];

        float probability = count/total;
        ans[association.first] = probability;
    }
    return ans;
}


map<vector<string>, float> formatAssociation(vector<string> sentences) {

    //map of phrase marker or part of phrase with its count
    map<string, int> countOfText = {};

    //map of associations with its count
    map< vector<string> , int> countOfAssociation = {};

    for(const string& sentence: sentences) {

        stack<string> words = wordStack(sentence);

        vector<string> grammarAssociations = {};
        vector<string> lexiconAssociations = {};


        while(!words.empty()) {
            string word = words.top();
            //check if its pm or pos
            if (word[0] == '*') {
                word.erase(word.begin());

                addToCountMap(word, countOfText);

                //add pm to the front
                grammarAssociations.insert(grammarAssociations.begin(), word);

                //if the size of grammar has 3 which is full 
                if (grammarAssociations.size() == 3) {
                    finishAssociation(countOfAssociation, grammarAssociations);
                }

                //add pm back as the first for next pm
                grammarAssociations.insert(grammarAssociations.begin(), word);
            }
            else if (word[0] == '+') {
                word.erase(word.begin());
                //add pos to the front
                lexiconAssociations.insert(lexiconAssociations.begin(), word);

                //add pos to the front of grammar vector
                grammarAssociations.insert(grammarAssociations.begin(), word);
                
                addToCountMap(word, countOfText);

                //if the size of lexicon has 2 which is full 
                if (lexiconAssociations.size() == 2) {
                    finishAssociation(countOfAssociation, lexiconAssociations);
                }

            }
            else {
                //add word to beginning of vector
                lexiconAssociations.insert(lexiconAssociations.begin(), word);
            }
            words.pop();
        }
    }

    return getProbability(countOfAssociation, countOfText);
}

//print out the probability
string printOutProability(map<vector<string>, float> probability) {
    //separate grammar with lexicon
    string grammar = "Grammar\n";
    string lexicon = "Lexicon\n";

    for(auto const& prob: probability) {
        
        if (prob.first.size() == 3) {
            //grammar
            //print out the first
            grammar += prob.first[0] + " -> ";
            //print rest of association
            for(int i = 1; i < prob.first.size(); i++) {
                grammar += prob.first[i] + " ";
            }
            grammar += "[" + to_string(prob.second) + "]" + "\n";
        }
        //size = 2
        else {
            //lexicon
            //print out the first
            lexicon += prob.first[0] + " -> ";
            //print rest of association
            for(int i = 1; i < prob.first.size(); i++) {
                lexicon += prob.first[i] + " ";
            }
            lexicon += "[" + to_string(prob.second) + "]" + "\n";
        }    
    }
    cout << grammar << "\n" << lexicon << endl; 

    return grammar + "\n" + lexicon;
}

void writeToFile(string data) {
    ofstream outputFile("input.txt");
    if (!outputFile){
        cerr << "Could not open output file" << endl;
    }
    outputFile << data;
    outputFile.close();
}

//put in tags
string putInTags(string sentence, map<vector<string>, float> probability) {
    vector<string> words = sentenceIntoWords(sentence);
    //loop through each word
    for(int i = 0; i < words.size(); i++) {
        //check whether its a lexicon or grammar
        for(auto const& text: probability) {
            //compare with first word
            if (words[i] == text.first[0]) {
                //size 3 is grammar, size 2 is lexicon
                if (text.first.size() == 3) {
                    words[i] = "*" + words[i];
                }
                //size is 2
                else {
                    words[i] = "+" + words[i];
                }
            }
        }
    }

    string ans = "";
    for (int i = 0; i < words.size(); i++) {
        ans += words[i] + " ";
    }
    //erase last space
    ans.erase(ans.end() - 1);  
    return ans;
}

//right or wrong, compare parse with original sentence, return the correct sentence and right or wrong
tuple<string, string> compareParse(string sentence, map<vector<string>, float> probability, string original) {

    map<string, vector<tuple<string, double>>> rules = grammarFileIntoMap();

    //convert to lower case
    transform(sentence.begin(), sentence.end(), sentence.begin(), ::tolower);
    
    map<tuple <string, int, int>, Tree*> chart = parser(sentence, rules);
    
    vector<string> words = sentenceIntoWords(sentence);
    
    
    //if one of the words are not valid
    if (chart.size() == 0) {
        return {"This sentence cannot be parsed", "Wrong"};
    }
    
    if (chart[{"S", 0, words.size() -1}] -> getProb() != 0.0) {
        //parse sentenced, the most optimal
        string parseSentence = printTree(chart, words.size()-1);
        //put in respective symbol tag
        string tagSentence = putInTags(parseSentence, probability);
        //compare tag sentence with original sentence

        if (tagSentence == original) {
            //right
            return {tagSentence, "Right"};
        }
        else {
            //wrong, write the correct sentence
            return {tagSentence, "Wrong"};
        }
    }
    else {
        //cannot be parse if probability is 0
        return {"This sentence cannot be parsed", "Wrong"};
    }
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        cerr << "Wrong input" << endl;
        return 8;
    }

    bool flag = false;
    if (argc == 3) {
        //check if the two strings are equal
        if (strncmp(argv[2],"-f", 2) == 0) {
            flag = true;
        }
    }

    //calls the input file
    ifstream inputFile("input.txt");
    if (!inputFile){
        cerr << "Could not open input file" << endl;
        return -9;
    }

    string line;
    vector<string> sentences = {};

    //training size
    int size = stoi(argv[1]);

    //read file by line
    while (size > 0 && getline(inputFile, line)) {
        sentences.push_back(line);
        size --;
    }

    //remaining sentences
    vector<string> remainingSentences = {};

    while(getline(inputFile, line)) {
        remainingSentences.push_back(line);
    }


    map<vector<string>, float> probability = formatAssociation(sentences);
    string probabilityString = printOutProability(probability);

    inputFile.close();

    writeToFile(probabilityString);

    if (flag) {
        cout << "Parses:" << endl;
    }

    float correct = 0;

    for (string sentence: remainingSentences) {
        //delete all of tags

        //remove the last space
        string original = sentence;
        original.erase(original.end() - 1);

        vector<string> words = sentenceIntoWords(sentence);
        sentence = "";
        for (string word: words) {
            //check if its lexicon or pm
            if(word[0] != '*' && word[0]!='+') {
                sentence += word + " ";
            }
        }
        tuple<string, string> ans = compareParse(sentence, probability, original);
        //check if theres a flag, there is print detailed output
        if(flag) {
            cout << get<0>(ans) << " " << get<1>(ans) << endl;
        }
        //check the number of correct
        if (get<1>(ans) == "Right") {
            correct ++;
        }
    }   
    //formatting
    if (flag) {
        cout << endl;
    }

    float accuracy = correct/remainingSentences.size();
    string accuracyString = to_string(accuracy);
    cout << "Accuracy: The parser was tested on " << remainingSentences.size() << " sentences. It got " << correct << " right, for an accuracy of " << accuracyString << endl;
}