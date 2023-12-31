//
// Created by Apex on 26-Dec-23.
//

#include "BTreeIndex.h"
#include "bits-stdc++.h"

using namespace std;

BTreeIndex::BTreeIndex() = default;

void BTreeIndex::DeleteRecordFromIndex(char *filename, int RecordID)
{
    /// TODO: if recordId is not found , return;

    vector<pair<int, int>> parentsPosition;

    // deletion step
    if (nodes[1].first == 0)
    {
        for (int i = 0; i < nodes[1].second.size(); i++)
        {
            if (nodes[1].second[i].index == RecordID)
            {
                nodes[1].second.erase(nodes[1].second.begin() + i);
                nodes[1].second.push_back({-1, -1});
            }
        }
    }
    else
    {
        int i = 1;
        bool flag = true;
        while (flag)
        {
            for (int j = 0; j < nodes[1].second.size(); j++)
            {
                if (nodes[i].second[j].index >= RecordID && nodes[i].first == 1)
                {
                    parentsPosition.emplace_back(i, j);
                    i = nodes[i].second[j].reference;
                    j = -1;
                }
                if (nodes[i].second[j].index == RecordID && nodes[i].first == 0)
                {
                    nodes[i].second.erase(nodes[i].second.begin() + j);
                    nodes[i].second.push_back({-1, -1});
                    if (getRecSize(i) < nodes[i].second.size() / 2)
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
    writeToFileReversed(filename);
}
void BTreeIndex::updateParents(vector<pair<int, int>> parentsPositions)
{
    for (int k = parentsPositions.size() - 1; k >= 0; k--)
    {
        // handle parent reference
        int i = parentsPositions[k].first;
        int j = parentsPositions[k].second;
        int recNum = nodes[i].second[j].reference;
        int prevBrotherRecNum = nodes[i].second[j - 1].reference;
        int nextBrotherRecNum = nodes[i].second[j + 1].reference;
        if (nodes[recNum].first == -1)
        {
            nodes[i].second.erase(nodes[i].second.begin() + j);
            nodes[i].second.push_back({-1, -1});
        }
        else
        {
            nodes[i].second[j].index = nodes[recNum].second[getRecSize(recNum) - 1].index;
        }
        // handle previous and next brother reference
        if (k == parentsPositions.size() - 1)
        {
            if (j != 0)
                nodes[i].second[j - 1].index = nodes[prevBrotherRecNum].second[getRecSize(prevBrotherRecNum) - 1].index;
            else
                nodes[i].second[j + 1].index = nodes[nextBrotherRecNum].second[getRecSize(nextBrotherRecNum) - 1].index;
        }
        sort(nodes[i].second.begin(), nodes[i].second.begin() + getRecSize(i));

    }
}
void BTreeIndex::balanceTree(vector<pair<int, int>> parentsPositions)
{
    int directParentIPosition = parentsPositions[parentsPositions.size() - 1].first;
    int directParentJPosition = parentsPositions[parentsPositions.size() - 1].second;
    // if I have previous brother
    if (directParentJPosition != 0)
    {
        int brotherRecNum = nodes[directParentIPosition].second[directParentJPosition - 1].reference;
        int myRecNum = nodes[directParentIPosition].second[directParentJPosition].reference;
        int brotherSize = getRecSize(brotherRecNum);

        // case of transfer element from my previous brother
        if (brotherSize > nodes[0].second.size() / 2)
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
            for (int i = 0; i < nodes[myRecNum].second.size() / 2; i++)
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
        if (brotherSize > nodes[0].second.size() / 2)
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
            for (int i = 0; i < nodes[brotherRecNum].second.size() / 2; i++)
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
    for (int i = 0; i < nodes[0].second.size(); i++)
    {
        if (nodes[recNum].second[i].reference != -1)
        {
            size++;
        }
        else
            break;
    }

    return size;
}
void BTreeIndex::loadFile(char *filename)
{
    // empty vector
    nodes.clear();
    pair<int, vector<node>> recordPair;

    // Open the file for reading in binary mode
    indexFile.open(filename, ios::in | ios::binary);

    if (!indexFile.is_open())
    {
        cout << "Could not open index file for reading" << endl;
        return;
    }

    string ch;
    node nd;
    // Read the file content
    for (int i = 0; i < numRecords; i++)
    {
        int indicator;
        if (indexFile.read((char *)(&indicator), sizeof(indicator)))
        {
            // skip the tab character
            indexFile.read((char *)(&ch), strlen("\t") + 1);

            vector<node> nds;
            nds.clear();
            for (int j = 0; j < M; j++)
            {
                int index, reference;

                if (indexFile.read((char *)(&index), sizeof(index)) &&
                    indexFile.read((char *)(&ch), strlen("\t") + 1) &&
                    indexFile.read((char *)(&reference), sizeof(reference)) &&
                    indexFile.read((char *)(&ch), strlen("\t") + 1))
                {
                    nd.index = index;
                    nd.reference = reference;
                    nds.push_back(nd);
                }
                else
                {
                    break;
                }
            }
            indexFile.read((char *)(&ch), strlen("\t") + 1);
            recordPair.first = indicator;
            recordPair.second = nds;

            nodes.push_back(recordPair);
        }
        else
        {
            break;
        }
    }

    // Close the file
    indexFile.close();



}
int BTreeIndex ::SearchARecord(char *filename, int RecordID) {
    int leaf = nodes[1].first;
    int refrance = 900;
    vector<node> temp = nodes[1].second;
    if (leaf == -1)
        return -1;
    if (leaf == 0) {
        for (int i = 0; i < temp.size(); ++i) {
            if (RecordID == temp[i].index)
                return temp[i].reference;
        }
        return -1;
    }

    while (leaf != 0) {
        for (int i = 0; i < temp.size(); ++i) {
            if (RecordID <= temp[i].index) {
                refrance = temp[i].reference;
                break;
            }
        }
        if (refrance == 900)
            return -1;
        leaf = nodes[refrance].first;
        temp = nodes[refrance].second;
    }
    for (int i = 0; i < temp.size(); ++i) {
        if (RecordID == temp[i].index)
            return temp[i].reference;
    }
    return -1;
}
void BTreeIndex::DisplayIndexFileContent(char *filename)
{

    // Iterate through the vector and print each pair
    for (int i = 0; i < nodes.size(); i++)
    {
        cout << nodes[i].first << "\t";

        for (int j = 0; j < nodes[i].second.size(); j++)
        {
            cout << nodes[i].second[j].index << "\t" << nodes[i].second[j].reference << "\t";
        }

        cout << endl;
    }
    return;
}

int BTreeIndex::InsertNewRecordAtIndex(char* filename, int RecordID, int Reference) {

    if(SearchARecord(filename, RecordID) != -1)
    {
        return - 1;
    }
    vector<pair<int,int>> parentsPositions;

    vector<node> currentRec = nodes[1].second;
    int indicator = nodes[1].first;
    int i = 1;
    while(indicator == 1)
    {
        for(int j = 0; j < getRecSize(i); j++)
        {
            if(nodes[i].second[j].index > RecordID)
            {
                parentsPositions.emplace_back(i,j);
                i = nodes[i].second[j].reference;
                indicator = nodes[i].first;
                break;
            }
            else if(j == getRecSize(i)-1)
            {
                parentsPositions.emplace_back(i,j);
                i = nodes[i].second[j].reference;
                indicator = nodes[i].first;
                break;
            }
        }
    }

    if(getRecSize(i) == M) {
        if (i == 1) {
            nodes[1].second.push_back({RecordID, Reference});
            sort(nodes[i].second.begin(), nodes[i].second.begin() + getRecSize(i) + 1);
            nodes[1].first = 1;
            if(nodes[2].first == -1 && nodes[2].first == -1)
            {
                nodes[2].first = 0;
                nodes[3].first = 0;
                nodes[0].second[0].index = nodes[3].second[0].index;
                nodes[2].second[0].index = -1;
                nodes[3].second[0].index = -1;
            }
            nodes[2].second.insert(nodes[2].second.begin(), nodes[1].second.begin(),
                                   nodes[1].second.begin() + M / 2 + 1);
            fill(nodes[2].second.begin()+ M/2+1, nodes[2].second.end() , node{-1, -1});
            // update next empty rec
            nodes[3].second.insert(nodes[3].second.begin(), nodes[1].second.begin() + M / 2 + 1, nodes[1].second.end());
            fill(nodes[3].second.begin()+ M/2+1, nodes[3].second.end() , node{-1, -1});
            nodes[2].second.resize(M);
            nodes[3].second.resize(M);

            nodes[1].second.insert(nodes[1].second.begin(), node{nodes[3].second[getRecSize(3) - 1].index, 3});
            nodes[1].second.insert(nodes[1].second.begin(), node{nodes[2].second[getRecSize(2) - 1].index,2});
            fill(nodes[1].second.begin() + 2, nodes[1].second.end() , node{-1, -1});
            nodes[1].second.resize(M);
        }
        else if (getRecSize(parentsPositions[parentsPositions.size() - 1].first) == M)
        {
            int firstNextEmptyRec = nodes[0].second[0].index;       // to split the leaf
            int secondNextEmptyRec = nodes[firstNextEmptyRec].second[0].index;      // for the first parent
            int thirdNextEmptyRec = nodes[secondNextEmptyRec].second[0].index;      // for the second parent
            // needs 3 empty lines to split
            if(firstNextEmptyRec != -1 && secondNextEmptyRec != -1 && thirdNextEmptyRec != -1)
            {
                nodes[i].second.push_back({RecordID, Reference});
                sort(nodes[i].second.begin(), nodes[i].second.end());

                // update next empty record
                nodes[0].second[0].index = nodes[thirdNextEmptyRec].second[0].index;
                // no more empty lines
                nodes[firstNextEmptyRec].second[0].index = -1;
                nodes[secondNextEmptyRec].second[0].index = -1;
                nodes[thirdNextEmptyRec].second[0].index = -1;
                // child or parent
                nodes[firstNextEmptyRec].first = 0;
                nodes[secondNextEmptyRec].first = 1;
                nodes[thirdNextEmptyRec].first = 1;
                // handle the new leaf
                nodes[firstNextEmptyRec].second.insert(nodes[firstNextEmptyRec].second.begin(),
                                nodes[i].second.begin() + M / 2 + 1, nodes[i].second.end());
                nodes[firstNextEmptyRec].second.resize(M);
                fill(nodes[i].second.begin()+ M/2+1, nodes[i].second.end() , node{-1, -1});
                nodes[i].second.resize(M);
                // handle the new parents
                int oldParent = parentsPositions[parentsPositions.size() - 1].first;
                nodes[oldParent].second.push_back({nodes[firstNextEmptyRec].second[getRecSize(firstNextEmptyRec)-1].index, firstNextEmptyRec});
                updateParents(parentsPositions);

                sort(nodes[oldParent].second.begin(), nodes[oldParent].second.end());
                nodes[secondNextEmptyRec].second.insert(nodes[secondNextEmptyRec].second.begin(),
                               nodes[oldParent].second.begin(), nodes[oldParent].second.begin() + M/2 + 1);

                nodes[thirdNextEmptyRec].second.insert(nodes[thirdNextEmptyRec].second.begin(),
                               nodes[oldParent].second.begin() + M/2 + 1, nodes[oldParent].second.end());
                nodes[secondNextEmptyRec].second.resize(M);
                nodes[thirdNextEmptyRec].second.resize(M);

                // handle the old parent
                nodes[oldParent].second[0].index = nodes[secondNextEmptyRec].second[getRecSize(secondNextEmptyRec) - 1].index;
                nodes[oldParent].second[0].reference = secondNextEmptyRec;
                nodes[oldParent].second[1].index = nodes[thirdNextEmptyRec].second[getRecSize(thirdNextEmptyRec) - 1].index;
                nodes[oldParent].second[1].reference = thirdNextEmptyRec;
                nodes[oldParent].second.resize(M);
                fill(nodes[oldParent].second.begin() + 2, nodes[oldParent].second.end(), node{-1 ,-1});
            }

        }
        else
        {
            int nextEmptyRec = nodes[0].second[0].index;
            if(nextEmptyRec != -1)
            {
                nodes[i].second.push_back({RecordID, Reference});
                sort(nodes[i].second.begin(), nodes[i].second.end());

                // update next empty record
                nodes[0].second[0].index = nodes[nextEmptyRec].second[0].index;
                nodes[nextEmptyRec].second[0].index = -1;
                nodes[nextEmptyRec].first = 0;
                nodes[nextEmptyRec].second.insert(nodes[nextEmptyRec].second.begin(), nodes[i].second.begin() + M/2+1, nodes[i].second.end());
                nodes[nextEmptyRec].second.resize(M);
                fill(nodes[i].second.begin()+ M/2+1, nodes[i].second.end() , node{-1, -1});
                nodes[i].second.resize(M);

                int x = parentsPositions[parentsPositions.size() - 1].first;
                int y = getRecSize(x)-1;
                nodes[x].second[getRecSize(x)].reference = nextEmptyRec;
                nodes[x].second[getRecSize(x)-1].index = nodes[nextEmptyRec].second[getRecSize(nextEmptyRec)-1].index;
                sort(nodes[x].second.begin(), nodes[x].second.begin() + getRecSize(x)-1);
                updateParents(parentsPositions);
            }
        }
    }
    else
    {
        if(nodes[i].first == -1)
        {
            nodes[i].first = 0;
            nodes[0].second[0].index = nodes[i].second[0].index;
            nodes[i].second[0].index = -1;
        }
        nodes[i].second.insert(nodes[i].second.begin(), {RecordID, Reference});
        nodes[i].second.resize(M);
        sort(nodes[i].second.begin(), nodes[i].second.begin() + getRecSize(i));
        updateParents(parentsPositions);
    }

    writeToFileReversed(filename);
    return 0;
}

void BTreeIndex::writeToFileReversed(char *filename)
{
    // Open the file for writing in binary mode
    ofstream outFile(filename, ios::out | ios::binary);

    if (!outFile.is_open())
    {
        cout << "Could not open file for writing" << endl;
        return;
    }

    // Write the content in reversed order
    for (int i = 0; i < nodes.size(); i++)
    {
        int indicator = nodes[i].first;

        // Write indicator to file
         outFile.write(reinterpret_cast<char *>(&indicator), sizeof(indicator));
        // Write tab character
         outFile.write("\t", strlen("\t") + 1);

        // Write nodes to file
        for (const auto &nd : nodes[i].second)
        {
            int index = nd.index;
            int reference = nd.reference;

            // Write index to file
            outFile.write(reinterpret_cast<char *>(&index), sizeof(index));
            // Write tab character
            outFile.write("\t", strlen("\t") + 1);
            // Write reference to file
            outFile.write(reinterpret_cast<char *>(&reference), sizeof(reference));
            // Write tab character
            outFile.write("\t", strlen("\t") + 1);
        }
        outFile.write("\n", strlen("\n"));
    }

     // Close the file
     outFile.close();
}

void BTreeIndex::CreateIndexFileFile(char *filename, int numberOfRecords, int m)
{
    numRecords = numberOfRecords;
    slots = (m * 2) + 1;
    M = m;
    // Open the file for writing in binary mode
    indexFile.open(filename, ios::out | ios::binary);

    if (!indexFile.is_open())
    {
        cout << "Could not open index file" << endl;
        return;
    }

    // Counter to keep track of record IDs
    int count = 1;

    // Loop through each record and write a line to the file
    for (int i = 0; i < numberOfRecords; i++)
    {
        for (int j = 0; j < (m * 2) + 1; j++)
        {
            // Write record ID in the second column
            if (j == 1 && i != (numberOfRecords - 1))
            {
                indexFile.write(reinterpret_cast<char *>(&count), sizeof(count));
                indexFile.write("\t", strlen("\t") + 1);
                count++;
                continue;
            }

            // Fill other columns with -1
            int value = -1;
            indexFile.write(reinterpret_cast<char *>(&value), sizeof(value));

            // Write a tab character unless it's the last column

            indexFile.write("\t", strlen("\t") + 1);

        }
        indexFile.write("\n", strlen("\n") + 1);
    }
    indexFile.close();

}
