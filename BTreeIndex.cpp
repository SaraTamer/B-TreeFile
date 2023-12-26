//
// Created by Apex on 26-Dec-23.
//

#include "BTreeIndex.h"
#include "bits/stdc++.h"

using namespace std;

BTreeIndex::BTreeIndex() = default;

void BTreeIndex::CreateIndexFileFile (char* filename, int numberOfRecords, int m)
{
    // Open the file for writing
    ofstream file(filename);

    // Counter to keep track of record IDs
    int count = 1;

    // Loop through each record and write a line to the file
    for (int i = 0; i < numberOfRecords; i++) {
        for (int j = 0; j <= (m * 2) + 2; j++) {
            // Write record ID in the second column
            if (j == 1 && i != (numberOfRecords - 1)) {
                file << count << "\t";
                count++;
                continue;
            }

            // Fill other columns with -1
            if(j != (m * 2) + 2)
                file << -1 << "\t";
            else
                file << -1;
        }
        if(i < numberOfRecords-1)
            file << "\n";
    }
}
void BTreeIndex::DeleteRecordFromIndex (char* filename, int RecordID)
{
    loadFile(filename);
    ///TODO: if recordId is not found , return;

    vector<pair<int , int>> parentsPosition;

    // deletion step
    if(nodes[1].first == 0)
    {
        for(int i = 0; i < nodes[1].second.size(); i++)
        {
            if(nodes[1].second[i].index == RecordID)
            {
                nodes[1].second.erase(nodes[1].second.begin() + i);
                nodes[1].second.push_back({-1,-1});
            }
        }
    }
    else
    {
        int i = 1;
        bool  flag = true;
        while(flag)
        {
            for(int j = 0; j < nodes[1].second.size(); j++)
            {
                if(nodes[i].second[j].index >= RecordID && nodes[i].first == 1)
                {
                    parentsPosition.emplace_back(i,j);
                    i = nodes[i].second[j].reference;
                    j = -1;
                }
                if(nodes[i].second[j].index == RecordID && nodes[i].first == 0)
                {
                    nodes[i].second.erase(nodes[i].second.begin() + j);
                    nodes[i].second.push_back({-1,-1});
                    if(getRecSize(i) < nodes[i].second.size() / 2)
                    {
                        balanceTree(parentsPosition);
                    }
                    flag = false;
                    break;
                }
            }
        }
        updateParents(parentsPosition);
    }
    // deletion step done


}
void BTreeIndex::updateParents(vector<pair<int , int>> parentsPositions)
{
    for(int k = parentsPositions.size() - 1; k >= 0; k--)
    {
        // handle parent reference
        int i = parentsPositions[k].first;
        int j = parentsPositions[k].second;
        int recNum = nodes[i].second[j].reference;
        int prevBrotherRecNum = nodes[i].second[j-1].reference;
        int nextBrotherRecNum = nodes[i].second[j-1].reference;
        if(nodes[recNum].first == -1)
        {
            nodes[i].second.erase(nodes[i].second.begin() + j);
            nodes[i].second.push_back({-1,-1});
        }
        else
        {
            nodes[i].second[j].index = nodes[recNum].second[getRecSize(recNum) - 1].index;
        }
        // handle previous and next brother reference
        if(k == parentsPositions.size() - 1)
        {
            if(j != 0)
                nodes[i].second[j-1].index = nodes[prevBrotherRecNum].second[getRecSize(prevBrotherRecNum) - 1].index;
            else
                nodes[i].second[j+1].index = nodes[nextBrotherRecNum].second[getRecSize(nextBrotherRecNum) - 1].index;
        }
    }
}
void BTreeIndex::balanceTree(vector<pair<int , int>> parentsPositions)
{
    int directParentIPosition = parentsPositions[parentsPositions.size() - 1].first;
    int directParentJPosition = parentsPositions[parentsPositions.size() - 1].second;
    // if I have previous brother
    if(directParentJPosition != 0)
    {
        int brotherRecNum = nodes[directParentIPosition].second[directParentJPosition-1].reference;
        int myRecNum = nodes[directParentIPosition].second[directParentJPosition].reference;
        int brotherSize = getRecSize(brotherRecNum);

        // case of transfer element from my previous brother
        if(brotherSize > nodes[0].second.size()/2)
        {
            nodes[myRecNum].second.insert(nodes[myRecNum].second.begin(), nodes[brotherRecNum].second[brotherSize - 1]);
            nodes[myRecNum].second.erase(nodes[myRecNum].second.begin() + nodes[myRecNum].second.size() - 1);
            nodes[brotherRecNum].second.erase(nodes[brotherRecNum].second.begin() + brotherSize - 1);
            nodes[brotherRecNum].second.push_back({-1, -1});
        }
        // case of my brother has exactly = m/2, then merge me and my brother
        else
        {
            // merge in my previous brother
            nodes[brotherRecNum].second.insert(nodes[brotherRecNum].second.begin() + brotherSize, nodes[myRecNum].second.begin(), nodes[myRecNum].second.end());
            nodes[brotherRecNum].second.resize(nodes[myRecNum].second.size());
            // erase my record leaf
            nodes[myRecNum].first = -1;
            for(int i = 0; i < nodes[myRecNum].second.size()/2; i++)
            {
                nodes[myRecNum].second[i].index = -1;
                nodes[myRecNum].second[i].reference = -1;
            }
            int firstEmptyRec = nodes[0].second[0].index;
            nodes[0].second[0].index = myRecNum;
            nodes[myRecNum].second[0].index = firstEmptyRec;
        }
    }
    // if I don't have previous brother, check for the next brother
    else
    {
        int brotherRecNum = nodes[directParentIPosition].second[directParentJPosition + 1].reference;
        int myRecNum = nodes[directParentIPosition].second[directParentJPosition].reference;
        int brotherSize = getRecSize(brotherRecNum);

        // case of transfer element from my next brother
        if(brotherSize > nodes[0].second.size()/2)
        {
            nodes[myRecNum].second.insert(nodes[myRecNum].second.begin() + getRecSize(myRecNum), nodes[brotherRecNum].second[0]);
            nodes[myRecNum].second.erase(nodes[myRecNum].second.end());
            nodes[brotherRecNum].second.erase(nodes[brotherRecNum].second.begin());
            nodes[brotherRecNum].second.push_back({-1, -1});
        }
            // case of my brother has exactly = m/2, then merge me and my brother
        else
        {
            // merge in my next brother
            nodes[myRecNum].second.insert(nodes[brotherRecNum].second.begin() + getRecSize(myRecNum), nodes[brotherRecNum].second.begin(), nodes[brotherRecNum].second.end());
            nodes[myRecNum].second.resize(nodes[brotherRecNum].second.size());
            // erase brother record leaf
            nodes[brotherRecNum].first = -1;
            for(int i = 0; i < nodes[brotherRecNum].second.size()/2; i++)
            {
                nodes[brotherRecNum].second[i].index = -1;
                nodes[brotherRecNum].second[i].reference = -1;
            }
            int firstEmptyRec = nodes[0].second[0].index;
            nodes[0].second[0].index = brotherRecNum;
            nodes[brotherRecNum].second[0].index = firstEmptyRec;
        }
    }
}
int BTreeIndex::getRecSize(int recNum)
{
    int size = 0;
    for(int i = 0; i < nodes[0].second.size(); i++)
    {
        if(nodes[recNum].second[i].index != -1)
        {
            size++;
        }
        else break;
    }

    return size;
}
void BTreeIndex::loadFile(char* filename)
{
    indexFile.open(filename, ios::in);
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
    indexFile.close();
}