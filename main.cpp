
#include "BTreeIndex.h"
#include "bits/stdc++.h"

using namespace std;
int main() {
    BTreeIndex bTreeIndex;
    string myString = "indexFile.txt";
    char charArray[myString.size() + 1];
    strcpy(charArray, myString.c_str());

    bTreeIndex.DeleteRecordFromIndex(charArray, 8);
    return 0;
}
