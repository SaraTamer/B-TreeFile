//
// Created by Apex on 26-Dec-23.
//

#include "BTreeIndex.h"
#include "bits/stdc++.h"

using namespace std;

BTreeIndex::BTreeIndex() = default;

void BTreeIndex::DeleteRecordFromIndex(char *filename, int RecordID)
{
    loadFile(filename);
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
        int nextBrotherRecNum = nodes[i].second[j - 1].reference;
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
        if (nodes[recNum].second[i].index != -1)
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
void BTreeIndex::CreateIndexFileFile(char *filename, int numberOfRecords, int m)
{
    tree.setM(m);
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
            if (j < ((m * 2) + 1) - 1)
            {
                indexFile.write("\t", strlen("\t") + 1);
            }
        }
        indexFile.write("\n", strlen("\n") + 1);
    }
    indexFile.close();

}
int BTreeIndex ::SearchARecord(char *filename, int RecordID) {
    loadFile(filename);
    int leaf = nodes[1].first;
    int refrance = 900;
    vector<node> temp = nodes[1].second;
    if (leaf == -1)
        return leaf;
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
    loadFile(filename);

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
int BTree::insert(int index, int reference) {
    // check if the tree is empty
    if(this->isEmpty()) {
        //assign the root to a new node
        this->root = new Node(index, reference);
        return 0;
    } else {
        if(this->root->isLeaf) {
            if(root->keys.size() == this->m) {
                Node* node = new Node();
                Node* left = new Node();
                Node* right = new Node();
                node->isLeaf = false;
                left-> isLeaf = true;
                right-> isLeaf = true;
                for(int i = 0; i < (this->m) / 2 + 1; i++) {
                    left->keys.push_back(root->keys[i]);
                    left->references.push_back(root->references[i]);
                }
                for(int i = (this->m) / 2 +1 ; i < this->m; ++i) {
                    right->keys.push_back(root->keys[i]);
                    right->references.push_back(root->references[i]);
                }
                //--------------------------------------------------
                node->keys.push_back(root->keys[(this->m ) / 2]);
                node->references.push_back(root->references[(this->m) / 2]);
                node->keys.push_back(root->keys[(this->m -1 ) ]);
                node->references.push_back(root->references[(this->m -1)]);


//------------------------------------------------------------


                right->Parent = node;
                left->Parent = node;
                Node * temp = root;
                root = node;
                root->children.push_back(left);
                root->children.push_back(right);
                delete temp;
                //insert
                if(index > root->keys[0]) {
                    insertRec(right, index, reference);
                } else {
                    insertRec(left, index, reference);
                }
            } else {
                int i = 0;
                for(auto it = root->keys.begin(); it != root->keys.end(); it++) {
                    if(index < *it) {
                        root->keys.insert(it, index);
                        break;
                    }
                    if(it + 1 == root->keys.end()) {
                        root->keys.push_back(index);
                        root->references.push_back(reference);
                        return 0;
                    }
                    i++;
                }
                root->references.insert(root->references.begin() + i, reference);
            }
        } else {
            Node* currentNode = root;
            int i = 0;
            int childIndex = 0;

            while(!currentNode->isLeaf) {
                if(index < currentNode->keys[0]) {
                    currentNode = currentNode->children[0];
                    continue;
                }

                i = 0;
                for(auto it = currentNode->keys.begin(); it != currentNode->keys.end(); it++) {
                    if(index > *it && index < *(it + 1)) {

                        currentNode = currentNode->children[i + 1];
                        childIndex = i + 1;
                        break;
                    }
                    if((it + 1) == currentNode->keys.end()) {

                        currentNode = currentNode->children[i + 1];
                        childIndex = i + 1;
                        break;
                    }
                    i++;
                }
            }
            // If the leaf node is full, split it
            if (currentNode->keys.size() == this->m) {
                Node* parent = currentNode->Parent;
                Node* left = new Node();
                Node* right = new Node();

                // Move median key and reference to parent
                insertRec(parent, currentNode->keys[currentNode->keys.size() / 2], currentNode->references[currentNode->references.size() / 2]);

                // Distribute keys and references between the new nodes
                for (int j = 0; j < this->m / 2; j++) {
                    left->keys.push_back(currentNode->keys[j]);
                    left->references.push_back(currentNode->references[j]);
                }
                for (int j = this->m / 2+1 ; j < this->m ; ++j) {
                    right->keys.push_back(currentNode->keys[j]);
                    right->references.push_back(currentNode->references[j]);
                }

                // Update parent pointers for the new nodes
                left->Parent = parent;
                right->Parent = parent;

                // Replace full leaf node with two new nodes in the parent
                Node* oldChild = parent->children[childIndex];
                parent->children.erase(parent->children.begin() + childIndex);
                parent->children.insert(parent->children.begin() + childIndex, left);
                parent->children.insert(parent->children.begin() + childIndex + 1, right);

                // Continue the insertion
                if (index > parent->keys[0]) {
                    insertRec(right, index, reference);
                } else {
                    insertRec(left, index, reference);
                }
            } else {
                // Insert directly into the leaf node (not full)
                int i = 0;
                for (auto it = currentNode->keys.begin(); it != currentNode->keys.end(); it++) {
                    if (index < *it) {
                        currentNode->keys.insert(it, index);
                        currentNode->references.insert(currentNode->references.begin() + i, reference);
                        break;
                    }
                    if (it + 1 == currentNode->keys.end()) {
                        currentNode->keys.push_back(index);
                        currentNode->references.push_back(reference);
                        break;
                    }
                    i++;
                }
            }
        }
    }
    return 0;
}

bool BTree::isEmpty() {
    return (this->root == nullptr);
}

void BTree::insertRec(BTree::Node *subTree, int index, int reference) {
    int i = 0;
    for(auto it = subTree->keys.begin(); it != subTree->keys.end(); it++) {
        if(index < *it) {
            subTree->keys.insert(it, index);
            break;
        }
        if(it + 1 == subTree->keys.end()) {
            subTree->keys.push_back(index);
            break;
        }
        i++;
    }
    subTree->references.insert(subTree->references.begin() + i, reference);
}
int BTreeIndex::InsertNewRecordAtIndex(char* filename, int RecordID, int Reference) {
    fstream file("../index.txt", ios::in | ios::out);
    tree.insert(RecordID, Reference);
    tree.writeToFile(&file, nodes);
    writeToFileReversed(filename);
    return 0;
}

void BTree::writeToFile(fstream *file, vector<pair<int, vector<node>>>& nodes) {
    // function to write the btree to the file, each line represents a tree node
    //  if the node is a leaf node, it writes each key followed by the corresponding reference
    // if it isn't a leaf, it writes each key followed by the corresponding child record index

    pair<int, vector<node>> recordPair;

    int counter = 1;
    queue<Node*> q;
    queue<Node*> q2;
    q2.push(this->root);
    q.push(this->root);
    Node* current = this->root;
    //traverse the tree pushing each node to the queue
    while(!q2.empty()) {
        current = q2.front();
        q2.pop();
        if(!current->isLeaf) {
            for(auto it = current->children.begin(); it != current->children.end(); it++) {
                q.push(*it);
                q2.push(*it);
                counter++;
            }
        }
    }
    //write the first line
    nodes.clear();
    vector<node> nds;
    nds.push_back({counter + 1, -1});
    recordPair.first = -1;

    for(int i = 0; i < this->m - 1; i++) {
        nds.push_back({-1, -1});
    }
    recordPair.second = nds;
    nodes.push_back(recordPair);


    //write each node to the file
    int childCounter = 1;
    counter = 1;
    queue<pair<int, int>> temp; //queue to hold the keys and references
    while(!q.empty()) {
        nds.clear();
        *file << "\n";
        current = q.front();
        q.pop();
        if(current->isLeaf) {
            recordPair.first = 0;
            for(int i = 0; i < this->m; i++) {
                if(i == current->keys.size()) {
                    if (!temp.empty()) {
                        pair<int, int> tempPair = temp.front();
                        temp.pop();
                        if(current->keys[i-1] != tempPair.first)
                            nds.push_back({tempPair.first, tempPair.second});
                    } else {
                        nds.push_back({-1, -1});
                    }
                }else if(i > current->keys.size()-1) {
                    nds.push_back({-1, -1});
                } else {
                    nds.push_back({current->keys[i], current->references[i]});
                }
            }
        } else {
            for(int i = 0; i < current->keys.size(); i++) {
                temp.push(make_pair(current->keys[i], current->references[i]));
            }
            *file << 1 << "\t";
            for(int i = 0; i < this->m; i++) {
                if(i > current->keys.size() - 1) {
                    nds.push_back({-1, -1});
                } else {
                    nds.push_back({current->keys[i], childCounter + 1});
                    childCounter++;
                }
            }
        }
        recordPair.second = nds;
        nodes.push_back(recordPair);
    }

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
    }

     // Close the file
     outFile.close();
}
