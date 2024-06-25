#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <ctime>
#include <unordered_map>

using namespace std;

class ComplexPerfectHashTable {
public:
    ComplexPerfectHashTable(const vector<complex<int>>& keys) {
        n = keys.size();
        p = findNextPrime(2 * n);
        primaryTable.resize(n, complex<int>(-1, -1));
        secondaryTables.resize(n);
        secondaryHashParams.resize(n, make_pair(-1, -1));
        initializeTables(keys);
    }

    bool find(const complex<int>& complexNum) {
        int key = complexToInt(complexNum);
        int idx = hash(key, primaryHashParams.first, primaryHashParams.second, p, n);
        if (primaryTable[idx] == complex<int>(-1, -1)) {
            return false;
        }
        pair<int, int> secondaryHashParam = secondaryHashParams[idx];
        if (secondaryHashParam.first == -1) {
            return primaryTable[idx] == complexNum;
        }
        vector<complex<int>> secondaryTable = secondaryTables[idx];
        int secondaryIdx = hash(key, secondaryHashParam.first, secondaryHashParam.second, p, secondaryTable.size());
        return secondaryTable[secondaryIdx] == complexNum;
    }

private:
    int n;
    int p;
    vector<complex<int>> primaryTable;
    vector<vector<complex<int>>> secondaryTables;
    vector<pair<int, int>> secondaryHashParams;
    pair<int, int> primaryHashParams;

    int findNextPrime(int num) {
        while (!isPrime(num)) {
            num++;
        }
        return num;
    }

    bool isPrime(int num) {
        if (num < 2) return false;
        for (int i = 2; i <= sqrt(num); i++) {
            if (num % i == 0) return false;
        }
        return true;
    }

    int hash(int key, int a, int b, int p, int m) {
        return ((a * key + b) % p) % m;
    }

    int complexToInt(const complex<int>& complexNum) {
        int a = complexNum.real();
        int b = complexNum.imag();
        return static_cast<int>(sqrt(a * a + b * b) * 1e6);
    }

    void initializeTables(const vector<complex<int>>& keys) {
        vector<int> intKeys;
        for (const auto& key : keys) {
            intKeys.push_back(complexToInt(key));
        }

        bool primarySuccess = false;
        while (!primarySuccess) {
            primaryHashParams = generateHashParams();
            primarySuccess = true;
            fill(primaryTable.begin(), primaryTable.end(), complex<int>(-1, -1));

            unordered_map<int, vector<complex<int>>> collisions;
            for (const auto& key : keys) {
                int keyInt = complexToInt(key);
                int idx = hash(keyInt, primaryHashParams.first, primaryHashParams.second, p, n);
                if (primaryTable[idx] != complex<int>(-1, -1)) {
                    primarySuccess = false;
                    break;
                }
                primaryTable[idx] = key;
                collisions[idx].push_back(key);
            }

            if (primarySuccess) {
                for (auto& pair : collisions) {
                    int idx = pair.first;
                    vector<complex<int>>& collisionKeys = pair.second;
                    if (collisionKeys.size() > 1) {
                        int secondarySize = collisionKeys.size() * collisionKeys.size();
                        bool secondarySuccess = false;
                        while (!secondarySuccess) {
                            auto secondaryHashParams = generateHashParams();
                            vector<complex<int>> secondaryTable(secondarySize, complex<int>(-1, -1));
                            secondarySuccess = true;
                            for (const auto& key : collisionKeys) {
                                int keyInt = complexToInt(key);
                                int secIdx = hash(keyInt, secondaryHashParams.first, secondaryHashParams.second, p, secondarySize);
                                if (secondaryTable[secIdx] != complex<int>(-1, -1)) {
                                    secondarySuccess = false;
                                    break;
                                }
                                secondaryTable[secIdx] = key;
                            }
                            if (secondarySuccess) {
                                this->secondaryHashParams[idx] = secondaryHashParams;
                                secondaryTables[idx] = secondaryTable;
                            }
                        }
                    }
                }
            }
        }
    }

    pair<int, int> generateHashParams() {
        return make_pair(rand() % (p - 1) + 1, rand() % p);
    }
};

int main() {
    srand(time(0));

    vector<complex<int>> keys = { {3, 4}, {1, 2}, {5, 12}, {8, 15}, {7, 24} };
    ComplexPerfectHashTable hashTable(keys);

    cout << "Find (3, 4): " << hashTable.find({3, 4}) << endl;  // Expected 1 (true)
    cout << "Find (6, 8): " << hashTable.find({6, 8}) << endl;  // Expected 0 (false)

    return 0;
}

