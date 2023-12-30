
#include "BTreeIndex.h"
#include "bits/stdc++.h"

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

    bTreeIndex.CreateIndexFileFile(fileName, numberOfRecords, m);

    while(true)
    {
        int choice, recId, reference;
        cout << "1- Display The file.\n"
                "2- Insert an index.\n"
                "3- Delete an index.\n"
                "4- Search for an index.\n";
        cin >> choice;
        switch (choice) {
            case 1:
                bTreeIndex.DisplayIndexFileContent(fileName);
                break;
            case 2:
                cout << "Enter the record id: ";
                cin >> recId;
                cout << "Enter the record reference: ";
                cin >> reference;
                bTreeIndex.InsertNewRecordAtIndex(fileName,recId, reference);
                break;
            case 3:
                cout << "Enter the record id: ";
                cin >> recId;
                bTreeIndex.DeleteRecordFromIndex(fileName, recId);
                break;
            case 4:
                cout << "Enter the record id: ";
                cin >> recId;
                bTreeIndex.SearchARecord(fileName, recId);
            default:
                return 0;
        }
    }

    return 0;
}
