#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>

using namespace std;

//object
class Object {
public:
    Object(char name, float value, float weight) {
        this -> name = name;
        this -> weight = weight;
        this -> value = value;
    }
    char getName() const {
        return name;
    }
    float getWeight() const {
        return weight;
    }
    float getValue() const {
        return value;
    }

private:
    char name;
    float weight;
    float value;
};

//get the total value of the objects in the vector
float getTotalValue(vector<Object> objects) {
    float sum = 0;
    for(int i = 0; i < objects.size(); i++) {
        sum += objects[i].getValue();
    }
    return sum;
}

//get the total weight of the objects in the vector
float getTotalWeight(vector<Object> objects) {
    float sum = 0;
    for(int i = 0; i < objects.size(); i++) {
        sum += objects[i].getWeight();
    }
    return sum;
}

//put out the name of objects
void printOut(vector<Object> objects) {
    cout << "Output: ";
    if (objects.empty()) {
        cout << "No solution" << endl;
    }
    else {
        for (int i = 0; i < objects.size(); i++) {
            cout << objects[i].getName() << " ";
        }
        cout << endl;
    }
}


vector<Object> Knapsack_ID(float target, float weight, vector<Object> objects) {
    queue<vector<Object>> queueObjects;
    //add each object as the first level of the state space
    for(int i = 0; i < objects.size(); i++) {
        //if the object by itself satisfies return it
        if(objects[i].getWeight() <= weight && objects[i].getValue() >= target) {
            return {objects[i]};
        }
        vector<Object> obj = {objects[i]};
        queueObjects.push(obj);
    }

    //use bfs
    while(!queueObjects.empty()) {
        vector<Object> obj = queueObjects.front();
        queueObjects.pop();
        //get the indice of last object of the vector of object to see what to append next
        int indice = obj[obj.size() - 1].getName() - objects[0].getName();
        for (int i = indice + 1; i < objects.size(); i++) {
            vector<Object> temp = obj;
            //add the next object
            temp.push_back(objects[i]);

            //see if the weight and value satisfy after object is added
            if (getTotalValue(temp) >= target && getTotalWeight(temp) <= weight) {
                //satisify
                return temp;
            }
            else if (getTotalWeight(temp) > weight) {
                //if the weight exceed, dont need to look append anymore
                continue;
            }
            else {
                //add the vector object if weight didnt exceed
                queueObjects.push(temp);
            }
        }
    }
    //return empty vector if nothing satisfies
    return {};
}

//find error of the vector of objects given target and weight 
float findError(vector<Object> objects, float target, float weight) {
    float errorWeight = getTotalWeight(objects)- weight;
    float errorValue = target - getTotalValue(objects);
    float error = 0;
    if (errorWeight > 0) {
        error += errorWeight;
    }

    if (errorValue > 0) {
        error += errorValue;
    }
    return error;
}

//find all element not in subset
vector<Object> notInSubset(vector<Object> set, const vector<Object>& subset) {
    vector<Object> notInSub = {};
    for(int i = 0; i < set.size(); i++) {
        bool found = false;
        for (int j = 0; j < subset.size(); j++) {
            //found break
            if (subset[j].getName() ==set[i].getName()) {
                found = true;
                break;
            }
        }
        if (!found) {
            notInSub.push_back(set[i]);
        }
    }
    return notInSub;
}

vector<Object> Knapsack_HC(float target, float weight, vector<Object> objects) {
    int restarts = 10;
    //best global error
    float globalError = INT_MAX;
    //best global case
    vector<Object> ans = {};
    // 10 tries to find the best solution
    for (int i = 0; i < restarts ; i++) {
        //start case
        vector<Object> startCase = {};
        for (int j = 0; j < objects.size(); j++) {
            int probability = rand() % 2;
            //probability of adding in is 1/2
            if (probability == 1) {
                startCase.push_back(objects[j]);
            }
        }

        //best local error
        float error = findError(startCase, target, weight);
        //best local case
        vector<Object> local = startCase;

        bool test = true;

        while (test) {

            //best neighbor
            float bestNeighborError = INT_MAX;
            vector<Object> bestNeighbor = {};

            //deletion
            for (int i = 0; i < local.size(); i++) {
                vector<Object> temp = local;
                //delete at index
                temp.erase(temp.begin() + i);
                float deleteError = findError(temp, target, weight);
                if (deleteError < bestNeighborError) {
                    bestNeighborError = deleteError;
                    bestNeighbor = temp;
                }
            }

            vector<Object> notInStartCase = notInSubset(objects, local);

            //addition, with objects not in start case
            for (int i = 0; i < notInStartCase.size(); i++) {
                vector<Object> temp = local;
                temp.push_back(notInStartCase[i]);
                float addError = findError(temp, target, weight);
                if (addError < bestNeighborError) {
                    bestNeighborError = addError;
                    bestNeighbor = temp;
                }
            }

            //replacement with objects not in start case
            for(int i = 0; i < local.size(); i++) {
                for(int j = 0; j < notInStartCase.size(); j++) {
                    vector<Object> temp = local;
                    temp[i] = notInStartCase[j];
                    float replaceError = findError(temp, target, weight);
                    if (replaceError < bestNeighborError) {
                        bestNeighborError = replaceError;
                        bestNeighbor = temp;
                    }
                }
            }

            //check if local max, the current node is peak
            if (bestNeighborError < error) {
                error = bestNeighborError;
                //continue on the best neighbor
                local = bestNeighbor;
            }
            else {
                //end while loop best case found
                test = false;
            }
        
        }

        // best case for the trial
        if (error < globalError) {
            globalError = error;
            ans = local;
        }
    }
    return ans;
}

int main() {
    //reset random number
    srand(time(0));

    vector<Object> objects = {};
    //calls the input file
    ifstream inputFile("input.txt");
    if (!inputFile){
        cerr << "Could not open file" << endl;
        return -9;
    }
    float targetValue;
    float maxWeight;
    //get target value and max weight
    inputFile >> targetValue >> maxWeight;
    char name;
    float value;
    float weight;
    //create object with name value and weight
    while (inputFile >> name >> value >> weight) {
        Object aObject = Object(name, value, weight);
        objects.push_back(aObject); 
    }

    vector<Object> id = Knapsack_ID(targetValue, maxWeight, objects);
    cout << "Knapsack_ID:" << endl;
    printOut(id);

    vector<Object> hc = Knapsack_HC(targetValue, maxWeight, objects);
    cout << endl;
    cout << "Knapsack_HC:" << endl;
    printOut(hc);
    cout << "Total Weight: " << getTotalWeight(hc) << endl;
    cout << "Total Value: " << getTotalValue(hc) << endl;
    cout << "Error: " << findError(hc, targetValue, maxWeight) << endl;

}