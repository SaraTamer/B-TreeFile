#include <iostream>
#include <fstream>
#include <string>
using namespace std;


void CreateIndexFileFile (const string& filename, int numberOfRecords, int m) {
    // Open the file for writing
    ofstream file(filename);

    // Counter to keep track of record IDs
    int count = 1;

    // Loop through each record and write a line to the file
    for (int i = 0; i < numberOfRecords; i++) {
        for(int j = 0; j <= (m * 2) + 2; j++) {
            // Write record ID in the second column
            if(j == 1 && i != (numberOfRecords - 1)) {
                file << count << "\t";
                count++;
                continue;
            }

            // Fill other columns with -1
            file << -1 << "\t";
        }
        file << "\n";
    }
}

int main() {
    // Get file name from user
    cout << "Enter the file name: ";
    string fileName;
    cin >> fileName;

    // Get the number of records from the user
    cout << "Enter the number of records: ";
    int numberOfRecords;
    cin >> numberOfRecords;

    // Get the value of m from the user
    cout << "Enter the value of m: ";
    int m;
    cin >> m;

    // Create an initial index file with user-specified values
    CreateIndexFileFile(fileName, numberOfRecords, m);


    return 0;
}
