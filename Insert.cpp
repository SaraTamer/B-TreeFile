#include <vector>
#include <fstream>
#include <iostream>
#include <queue>

using namespace std;





void CreateIndexFileFile(char* filename, int numberOfRecords, int m) {
    ofstream file(filename);
    int count = 1;
    for(int i = 0; i < numberOfRecords; i++) {
        for(int j = 0; j <= (m * 2); j++) {
            if(j == 1 && i != (numberOfRecords - 1)) {
                file << count << "\t";
                count++;
                continue;
            }
            file << -1 << "\t";
        }
        file << "\n";
    }
}


int main() {
    char *fileName = "../index.txt";
    CreateIndexFileFile(fileName, 10, 5);
    InsertNewRecordAtIndex(fileName, 3, 12);
    InsertNewRecordAtIndex(fileName, 7, 24);
    InsertNewRecordAtIndex(fileName, 10, 48);
    InsertNewRecordAtIndex(fileName, 24, 60);
    InsertNewRecordAtIndex(fileName, 14, 72);
    InsertNewRecordAtIndex(fileName, 19, 84);
    InsertNewRecordAtIndex(fileName, 15, 108);
    InsertNewRecordAtIndex(fileName, 1, 120);
    InsertNewRecordAtIndex(fileName, 5, 132);
   InsertNewRecordAtIndex(fileName, 30, 96);
   InsertNewRecordAtIndex(fileName, 2, 144);
   InsertNewRecordAtIndex(fileName, 8, 156);
   InsertNewRecordAtIndex(fileName, 6, 180);
// Testing 
   // InsertNewRecordAtIndex(fileName, 9, 164);
    //InsertNewRecordAtIndex(fileName, 11, 192);
   // InsertNewRecordAtIndex(fileName, 12, 204);
   // InsertNewRecordAtIndex(fileName, 17, 216);
    //InsertNewRecordAtIndex(fileName, 18, 228);
    //InsertNewRecordAtIndex(fileName, 32, 240);

    return 0;
}
