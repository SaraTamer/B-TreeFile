//
// Created by Apex on 26-Dec-23.
//

#ifndef B_TREEFILE_BTREEINDEX_H
#define B_TREEFILE_BTREEINDEX_H

#include "bits/stdc++.h"

using namespace std;

struct node{
    int index;
    int reference;
};

class BTreeIndex {
private:
    fstream indexFile;
    vector<pair<int, vector<node>>> nodes;
public:
    // project features
    BTreeIndex();
    void CreateIndexFileFile (char* filename, int numberOfRecords, int m) ;
    int InsertNewRecordAtIndex (char* filename, int RecordID, int Reference);
    //insert function should return -1 if there is no place to insert the record or the index of the node where the new record is inserted if the record was inserted successfully.
    void DeleteRecordFromIndex (char* filename, int RecordID);
    void DisplayIndexFileContent (char* filename);
    // this method should display content of the file, each node in a line.
    int SearchARecord (char* filename, int RecordID);

    // needed methods
    void loadFile(char* filename);
    int getRecSize(int recNum);
    void balanceTree(vector<pair<int , int>>);
    void updateParents(vector<pair<int , int>> parentsPositions);
};


#endif //B_TREEFILE_BTREEINDEX_H
