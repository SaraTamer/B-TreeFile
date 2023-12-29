
#include "BTreeIndex.h"
#include "bits-stdc++.h"

using namespace std;
int main() {
    BTreeIndex bTreeIndex;

    // Get file name from user
    const int maxFileNameSize = 100;
    char fileName[maxFileNameSize];
    cout << "Enter the file name: ";
    cin.getline(fileName, maxFileNameSize);

    // Get the number of records from the user
    cout << "Enter the number of records: ";
    int numberOfRecords;
    cin >> numberOfRecords;

    // Get the value of m from the user
    cout << "Enter the value of m: ";
    int m;
    cin >> m;
    return 0;
}
