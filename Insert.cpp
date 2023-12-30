#include <vector>
#include <fstream>
#include <iostream>
#include <queue>

using namespace std;

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
    void writeToFile(fstream* file);
};

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
                for(int i = 0; i < (this->m) / 2; i++) {
                    left->keys.push_back(root->keys[i]);
                    left->references.push_back(root->references[i]);
                }
                for(int i = (this->m) / 2 +1 ; i < this->m; ++i) {
                    right->keys.push_back(root->keys[i]);
                    right->references.push_back(root->references[i]);
                }
                node->keys.push_back(root->keys[(this->m ) / 2]);
                node->references.push_back(root->references[(this->m) / 2]);
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

void BTree::writeToFile(fstream *file) {
    // function to write the btree to the file, each line represents a tree node
    //  if the node is a leaf node, it writes each key followed by the corresponding reference
    // if it isn't a leaf, it writes each key followed by the corresponding child record index
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
    *file << -1 << "\t" << counter + 1 << "\t";
    for(int i = 0; i < (this->m * 2) - 1; i++) {
        *file << -1 << "\t";
    }
    //write each node to the file
    int childCounter = 1;
    counter = 1;
    queue<pair<int, int>> temp; //queue to hold the keys and references
    while(!q.empty()) {
        *file << "\n";
        current = q.front();
        q.pop();
        if(current->isLeaf) {
            *file << 0 << "\t";
            for(int i = 0; i < this->m; i++) {
                if(i == current->keys.size()) {
                    if(!temp.empty()) {
                        pair<int, int> tempPair = temp.front();
                        temp.pop();
                        *file << tempPair.first << "\t" << tempPair.second << "\t";
                    } else {
                        *file << -1 << "\t" << -1 << "\t";
                    }
                } else if(i > current->keys.size()) {
                    *file << -1 << "\t" << -1 << "\t";
                } else {
                    *file << current->keys[i] << "\t" << current->references[i] << "\t";
                }
            }
        } else {
            for(int i = 0; i < current->keys.size(); i++) {
                temp.push(make_pair(current->keys[i], current->references[i]));
            }
            *file << 1 << "\t";
            for(int i = 0; i < this->m; i++) {
                if(i > current->keys.size() - 1) {
                    *file << -1 << "\t" << -1 << "\t";
                } else {
                    *file << current->keys[i] << "\t" << childCounter + 1 << "\t";
                    childCounter++;
                }
            }
        }
    }
}

BTree tree(5);

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

int InsertNewRecordAtIndex(char* filename, int RecordID, int Reference) {
    fstream file("../index.txt", ios::in | ios::out);
    tree.insert(RecordID, Reference);
    tree.writeToFile(&file);
    return 0;
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
