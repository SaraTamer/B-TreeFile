//
// Created by Apex on 26-Dec-23.
//

#include "BTreeIndex.h"
#include "bits/stdc++.h"

using namespace std;

BTreeIndex::BTreeIndex() = default;



void BTreeIndex::loadFile()
{
    indexFile.open("indexFile.txt", ios::in);
    // empty vector
    nodes.clear();
    int temp, recNum = 0;

    pair<int,vector<node>> pair;
    nodes.push_back(pair);

    while(!indexFile.fail())
    {
        int indicator;
        if(nodes[recNum].second.empty())
        {
            indexFile >> indicator;
            nodes[recNum].first = indicator;
        }
        char c = indexFile.peek();
        if(c == '\n')
        {
            recNum++;
            nodes.push_back(pair);
        }
        else
        {
            node n;
            indexFile >> temp;
            n.index = temp;
            indexFile >> temp;
            n.reference = temp;
            nodes[recNum].second.push_back(n);
        }

    }
}