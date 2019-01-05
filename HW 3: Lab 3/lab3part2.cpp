#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <stack>

using namespace std;


//clause class
class Clause {
public:
    Clause(vector<int> clause): clause(clause) {}
    vector<int> getClause() const {
        return clause;
    }

    void setClause(vector<int> clause) {
        this -> clause = clause;
    }

private:
    vector<int> clause;
};

//singleton
vector<int> findSingleton(vector<Clause> clauses) {
    vector<int> singletons = {};
    for(Clause c: clauses) {
        vector<int> aClause = c.getClause();
        //atom by itself and haven't added to the vector
        if (aClause.size() == 1 && find(singletons.begin(), singletons.end(), aClause[0]) ==singletons.end() ) {
            singletons.push_back(aClause[0]);
        }
    }
    return singletons;
}

vector<int> findPureLiteral(vector<Clause> clauses) {
    vector<int> pureLiterals = {};
    map<int, bool> hash = {};
    for(Clause c: clauses) {
        vector<int> clause = c.getClause();
        for(int i = 0; i < clause.size(); i++) {
            //add the atom to the hash and set it to true if it doesnt exist
            int atom = clause[i];
            if (hash.find(abs(atom)) == hash.end()) {
                hash[atom] = true;
            }
            //if its negative counterpart exist, then set false in the hash
            else {
                hash[abs(atom)] = false;
            }
        }
    }
    //find all of the true case in hash
    for (auto a : hash) {
        if (a.second) {
            pureLiterals.push_back(a.first);
        }
    }
    return pureLiterals;
}

//propagate, call by reference
void propagate(vector<Clause>& clauses,int atom) {
    for(int i = 0; i < clauses.size(); i++) {
        for(int j = 0; j < clauses[i].getClause().size(); j++) {
            //erase the clause if its true
            if (clauses[i].getClause()[j] == atom) {
                clauses.erase(clauses.begin() + i);
                i --;
                break;
            }
            //delete atom if its false;
            if (clauses[i].getClause()[j] == atom * -1) {
                vector<int> temp = clauses[i].getClause();
                temp.erase(temp.begin()+ j);
                clauses[i].setClause(temp);
                j--;
            } 
        }
    }
}

//for testing purposes
void printClause(vector<Clause> clause) {
    for(auto c: clause) {
        for(auto x: c.getClause()) {
            cout << x << " ";
        }
        cout << endl;
    }
}

map<int, char> DP(vector<Clause> clauses, set<int> atoms) {

    map<int, char> res = {};
    stack<tuple<vector<Clause>, set<int>, map<int, char>, int>> answers = {};
    while (!clauses.empty()) {

        bool emptyClause = false;

        //also check if any of clauses are empty, nil cases
        for(auto clause: clauses) {
            if (clause.getClause().empty()) {
                emptyClause = true;
            }
        }

        if (emptyClause) {
            //failed

            //if stack is empty means no solution
            if(answers.empty()) {
                return {};
            }

            //goes back to the previous answer in the stack
            tuple<vector<Clause>, set<int>, map<int, char>, int> ans = answers.top();

            //delete that answer
            answers.pop();
            //use the previous answer
            clauses = get<0>(ans);
            atoms = get<1>(ans);
            res = get<2>(ans);
            int atom = get<3>(ans);
            //now set the atom to be false instead of true;
            res[atom] = 'F';
            atoms.erase(atom);
            atom = atom * -1;
            propagate(clauses, atom);

        }

        //order
        //first check singleton, the pure literal, then hard case

        //singleton
        vector<int> singleton = findSingleton(clauses);

        //pure literal
        vector<int> pureLiteral = findPureLiteral(clauses);

        if (!singleton.empty()) {
            for(int i = 0; i < singleton.size(); i++) {
                int atom = singleton[i];
                //set each atom to true or false 
                if (atom < 0) {
                    res[abs(atom)] = 'F';
                }
                else {
                    res[atom] = 'T';
                }
                //propagate the clauses
                propagate(clauses, atom);

                //remove from set of atoms
                atoms.erase(abs(atom));
            }
        }
        else if (!pureLiteral.empty()) {
            for(int i = 0; i < pureLiteral.size(); i++) {
                int atom = pureLiteral[i];
                //set each atom to true or false 
                if (atom < 0) {
                    res[abs(atom)] = 'F';
                }
                else {
                    res[atom] = 'T';
                }
                //propagate the clauses
                propagate(clauses, atom);

                //remove from set of atoms
                atoms.erase(abs(atom));
            }
        }
        else {

            //hard case, try true before false;
            int atom = *atoms.begin();
            //add current answer to stack in case this path fails
            tuple<vector<Clause>, set<int>, map<int, char>, int> pre = {clauses, atoms, res, atom};
            answers.push(pre);
            propagate(clauses, atom);
            atoms.erase(atom);
            res[atom] = 'T';

        }
    }

    //set all of unbound atoms to true
    for(int atom: atoms) {
        res[atom] = 'T';
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
    
    string line;
    set<int> atoms;
    vector<Clause> clauses;
    string extra = "";
    bool end = false;
    
    //read through each line of the file
    while (getline(inputFile, line)) {
        vector<int> clause = {};
        int atom;
        //convert string to stream to parse through int
        istringstream iss (line);
        while (iss >> atom && !end) {
            //end when atom is 0
            if (atom == 0) {
                end = true;
                break;
            }
            clause.push_back(atom); 
            atoms.insert(abs(atom));
        }
        //only add if not empty line
        if (!end && !clause.empty()) {
            Clause aClause = Clause(clause);
            clauses.push_back(aClause);
        }
        //all of the extra after the 0
        if(end){
            extra += line + "\n";
        }
    }

    map<int, char> dp = DP(clauses, atoms);
    inputFile.close();

    //output file
    ofstream outputFile("text.txt");
    if (!outputFile){
        cerr << "Could not open output file" << endl;
        return -9;
    }

    //if empty map, no solution 

    if (dp.empty()) {
        outputFile << "NO SOLUTION" << endl;
    }
    else {
        //writing to outputfile
        for(auto val: dp) {
            outputFile << val.first << " " << val.second << endl;
        }
    }
    outputFile << extra;
    outputFile.close();
}