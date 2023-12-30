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
class BTree {
private:
    struct Node {
        vector<int> keys;
        vector<int> references;
        vector<Node*> children;
        Node* Parent;
        bool isLeaf;

        Node(): isLeaf(true), Parent(nullptr) {}
        Node(int index, int reference): isLeaf(true) {
            keys.push_back(index);
            references.push_back(reference);
        }
        Node(bool isLeaf): isLeaf(isLeaf) {}
    };
    Node* root;
    int m;
public:
    BTree(): root(nullptr) {}
    BTree(int m) : m(m), root(nullptr){}
    bool isEmpty(void);
    int insert(int index, int reference);
    void insertRec(Node* subTree, int index, int reference);
    void writeToFile(fstream* file, vector<pair<int, vector<node>>>& nodes);
    void setM(int m) {this->m = m; this->root = nullptr;}
};
class BTreeIndex {
private:
    fstream indexFile;
    vector<pair<int, vector<node>>> nodes;
    int numRecords, slots, M;

public:
    BTree tree;
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
    void writeToFileReversed(char *filename);
};




#endif //B_TREEFILE_BTREEINDEX_H
