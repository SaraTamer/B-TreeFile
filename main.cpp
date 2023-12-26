
#include "BTreeIndex.h"
#include "bits/stdc++.h"

using namespace std;
int main() {
    BTreeIndex bTreeIndex;
//    string myString = "indexFile.txt";
//    char charArray[myString.size() + 1];
//    strcpy(charArray, myString.c_str());
//
//    bTreeIndex.DeleteRecordFromIndex(charArray, 8);

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

    // Create an initial index file with user-specified values
    bTreeIndex.CreateIndexFileFile(fileName, numberOfRecords, m);
    bTreeIndex.loadFile(fileName);


    return 0;
}
