#include "ctrie.hpp"
#include <memory>
#include <cstring>


using namespace std;

CompressedTrie::CompressedTrie() {
    root = make_unique<CompressedTrieNode>();
}

void CompressedTrie::inc(CompressedTrieNode* curr_node, const int& val){
    if(curr_node == root.get()) {
        curr_node->num_leafs += val;
        return;
    }
    else {
        curr_node->num_leafs += val;
        inc(curr_node->parent, val);
    }
}

bool CompressedTrie::insert(const Slice &key, const Slice &value) {
    string word = string(key.data);

    if(word == "") return false;
    // No matching edge present, just insert entire word
    if(root->children.find(word[0]) == root->children.end()) {

        root->children[word[0]] = make_unique<CompressedTrieNode>();
        CompressedTrieNode* curr_node = root->children[word[0]].get();
        curr_node->edgelabel = word;
        curr_node->isLeaf = true;
        curr_node->parent = root.get();
        
        curr_node->value = make_unique<Slice>();
        Slice* newSlice = curr_node->value.get();
        newSlice->size = value.size;
        newSlice->data = new char[value.size + 1];
        strcpy(newSlice->data, value.data);
        inc(curr_node, 1);
    }
    else {
        size_t i=0, j=0;
        string word_to_cmp;
        CompressedTrieNode* curr_node = root->children[word[0]].get();
        while(i < word.length()) {
            word_to_cmp = curr_node->edgelabel;
            j = 0;
            while(i<word.length() && j<word_to_cmp.length() && word[i] == word_to_cmp[j]) {
                i++;
                j++;
            }
            // i complete
            if(i == word.length()) {
                // j also complete - mark this as leaf node
                if(j == word_to_cmp.length()) {
                    curr_node->isLeaf = true;
                    /* curr_node->num_leafs++; */
                    curr_node->value = make_unique<Slice>();
                    Slice* newSlice = curr_node->value.get();
                    newSlice->size = value.size;
                    newSlice->data = new char[value.size + 1];
                    strcpy(newSlice->data, value.data);
                    inc(curr_node, 1);
                }
                // j remaining - split word into 2
                else {
                    string rem_word = word_to_cmp.substr(j);
                    unique_ptr<CompressedTrieNode> newnode = make_unique<CompressedTrieNode>();
                    newnode->edgelabel = rem_word;
                    newnode->isLeaf = curr_node->isLeaf;
                    newnode->parent = curr_node;
                    newnode->num_leafs = curr_node->num_leafs;
                    if(curr_node->isLeaf){
                        newnode->value = std::move(curr_node->value);
                    }
                    newnode->children = std::move(curr_node->children);
                    for(auto &baccha: newnode->children) {
                        baccha.second.get()->parent = newnode.get();
                    }

                    curr_node->edgelabel = word_to_cmp.substr(0,j);

                    curr_node->isLeaf = true;
                    curr_node->children.clear();
                    curr_node->children[rem_word[0]] = std::move(newnode);
                    curr_node->value = make_unique<Slice>();
                    Slice* newSlice = curr_node->value.get();
                    newSlice->size = value.size;
                    newSlice->data = new char[value.size + 1];
                    strcpy(newSlice->data, value.data);
                    inc(curr_node, 1);

                }
            }
            // i not complete, j complete
            else if( j == word_to_cmp.length()) {
                // no remaining edge
                if (curr_node->children.find(word[i]) == curr_node->children.end()) {
                    CompressedTrieNode* curr_parent = curr_node;
                    curr_node->children[word[i]] = make_unique<CompressedTrieNode>();
                    curr_node = curr_node->children[word[i]].get();
                    curr_node->edgelabel = word.substr(i);
                    curr_node->isLeaf = true;
                    /* curr_node->num_leafs++; */
                    curr_node->parent = curr_parent;
                    i = word.length();
                    curr_node->value = make_unique<Slice>();
                    Slice* newSlice = curr_node->value.get();
                    newSlice->size = value.size;
                    newSlice->data = new char[value.size + 1];
                    strcpy(newSlice->data, value.data);
                    inc(curr_node, 1);

                }
                else {
                    // remaining edge - continue with matching
                    curr_node = curr_node->children[word[i]].get();
                }
            }
            // i not complete & j not complete. Split into two and insert
            else {
                string rem_word_i = word.substr(i);
                string rem_word_j = word_to_cmp.substr(j);
                string match_word = word_to_cmp.substr(0,j);
                
                unique_ptr<CompressedTrieNode> newnode = make_unique<CompressedTrieNode>();
                newnode->isLeaf = curr_node->isLeaf;
                newnode->num_leafs = curr_node->num_leafs;
                if(curr_node->isLeaf){
                    newnode->value = std::move(curr_node->value);
                }
                newnode->children = std::move(curr_node->children);
                newnode->edgelabel = rem_word_j;
                newnode->parent = curr_node;
                for(auto &baccha: newnode->children){
                    baccha.second.get()->parent = newnode.get();
                }

                
                unique_ptr<CompressedTrieNode> newnode2 = make_unique<CompressedTrieNode>();
                newnode2->isLeaf = true;
                newnode2->num_leafs++;
                newnode2->edgelabel = rem_word_i;
                newnode2->value = make_unique<Slice>();
                newnode2->parent = curr_node;
                Slice* newSlice = newnode2->value.get();
                newSlice->size = value.size;
                newSlice->data = new char[value.size + 1];
                strcpy(newSlice->data, value.data);


                curr_node->isLeaf = false;
                curr_node->edgelabel = match_word;

                curr_node->children.clear();
                curr_node->children[rem_word_j[0]] = std::move(newnode);
                curr_node->children[rem_word_i[0]] = std::move(newnode2);
                inc(curr_node, 1);
                
                i = word.length();

            }
        }
    }

    return true;
}

string CompressedTrie::search(const int& N){
    CompressedTrieNode* curr_node = root.get();
    
    int left = N;

    string result = "";

    while(true){
        for(auto &edge: curr_node->children){
            int child_cnt = edge.second.get()->num_leafs;
            if(child_cnt == 0) continue;

            if(left > child_cnt){
                left -= child_cnt;
            } else {
                result += edge.second.get()->edgelabel;
                curr_node = edge.second.get();
                if(curr_node->isLeaf) left--;

                if(left == 0){
                    return result;
                }

                break;
            }
        }
    }

}

bool CompressedTrie::del(const int& N) {
    CompressedTrieNode* curr_node = root.get();

    int left = N;

    while(true){
        for(auto &edge: curr_node->children){
            int child_cnt = edge.second.get()->num_leafs;
            if(child_cnt == 0) continue;

            if(left > child_cnt){
                left -= child_cnt;
            } else {
                curr_node = edge.second.get();
                if(curr_node->isLeaf) left--;

                if(left == 0){
                    curr_node->isLeaf = 0;
                    curr_node->value.reset();
                    inc(curr_node, -1);
                    return true;
                }

                break;
            }
        }
    }
}

Slice* CompressedTrie::search(const Slice& key) const {
    size_t i=0, j=0;
    Slice* to_ret = NULL;
    string word = string(key.data);
    string word_to_match;
    if(word == "") return NULL;
    if(root->children.find(word[0]) == root->children.end()) {
        return NULL;
    }
    bool ispresent = false;
    CompressedTrieNode* curr_node = root->children[word[0]].get();
    while (i < word.length()) {
        j=0;
        word_to_match = curr_node->edgelabel;
        while(i < word.length() && j < word_to_match.length() && word[i] == word_to_match[j]) {
            i++;
            j++;
        }
        // completed matching
        if( i == word.length()) {
            if(j == word_to_match.length() && curr_node->isLeaf) {
                to_ret = curr_node->value.get();
                ispresent = true;
            }
            else to_ret = NULL, ispresent = false;
        }
        // match remaining
        else {
            // j completed
            if( j == word_to_match.length()) {
                // nowhere to go
                if(curr_node->children.find(word[i]) == curr_node->children.end()) {
                    to_ret = NULL;
                    ispresent = false;
                    break;
                }
                else {
                    // continue matching
                    curr_node = curr_node->children[word[i]].get();
                }
            }
            // j remaining, no match
            else {
                to_ret = NULL;
                ispresent = false;
                break;
            }
        }
    }
    return to_ret;
}

bool CompressedTrie::del(const Slice& key) {
    size_t i=0, j=0;
    Slice* to_ret = NULL;
    string word = string(key.data);
    string word_to_match;
    if(word == "") return false;
    if(root->children.find(word[0]) == root->children.end()) {
        return false;
    }
    bool ispresent = false;
    CompressedTrieNode* curr_node = root->children[word[0]].get();
    while (i < word.length()) {
        j=0;
        word_to_match = curr_node->edgelabel;
        while(i < word.length() && j < word_to_match.length() && word[i] == word_to_match[j]) {
            i++;
            j++;
        }
        // completed matching
        if( i == word.length()) {
            if(j == word_to_match.length() && curr_node->isLeaf) {
                curr_node->isLeaf = false;
                curr_node->value.reset();
                inc(curr_node, -1);
                ispresent = true;
            }
            else to_ret = NULL, ispresent = false;
        }
        // match remaining
        else {
            // j completed
            if( j == word_to_match.length()) {
                // nowhere to go
                if(curr_node->children.find(word[i]) == curr_node->children.end()) {
                    to_ret = NULL;
                    ispresent = false;
                    break;
                }
                else {
                    // continue matching
                    curr_node = curr_node->children[word[i]].get();
                }
            }
            // j remaining, no match
            else {
                to_ret = NULL;
                ispresent = false;
                break;
            }
        }
    }
    return ispresent;
}


/*
bool CompressedTrie::startsWith(const string &word) const {
    size_t i=0, j=0;
    string word_to_match;
    if(word == "") return false;
    if(root->children.find(word[0]) == root->children.end()) {
        return false;
    }
    bool ispresent = false;
    CompressedTrieNode* curr_node = root->children[word[0]].get();
    while (i < word.length()) {
        j=0;
        word_to_match = curr_node->edgelabel;
        while(i < word.length() && j < word_to_match.length() && word[i] == word_to_match[j]) {
            i++;
            j++;
        }
        // completed matching
        if( i == word.length()) {
            ispresent = true;
        }
        // Case 2 : match remaining
        else {
            // Case 2a: j completed
            if( j == word_to_match.length()) {
                // Case 2aa: nowhere to go
                if(curr_node->children.find(word[i]) == curr_node->children.end()) {
                    ispresent = false;
                    break;
                }
                else {
                    // case 2ab : continue matching
                    curr_node = curr_node->children[word[i]].get();
                }
            }
            // case 2b : j remaining, no match
            else {
                ispresent = false;
                break;
            }
        }
    }
    return ispresent;
}
*/

int main() {
    CompressedTrie T;
    Slice A, B, C, D, E;
    A.size = 5;
    A.data = new char[6];
    strcpy(A.data, "aello");
    B.size = 2;
    B.data = new char[3];
    strcpy(B.data, "aa");
    C.size = 8;
    C.data = new char[9];
    strcpy(C.data, "aaazouwu");
    D.size = 6;
    D.data = new char[7];
    strcpy(D.data, "aaazoz");
    E.size = 4;
    E.data = new char[5];
    strcpy(E.data, "hasl");

    T.insert(B, B);
    T.insert(C, C);
    T.insert(E, E);
    T.insert(D, D);
    T.insert(A, A);
    cout << endl;
    /* T.del(D); */
    /* T.del(E); */
    /* T.del(A); */
    /* T.insert(D, E); */

    cout << T.search(D)->data << endl;
    cout << T.search(E) << endl;
    cout << T.search(A) << endl;
    cout << T.search(B)->data << endl;
    cout << T.search(C)->data << endl;
    cout << "------------" << endl;
    cout << T.search(1) << "**" << endl;
    cout << T.search(2) << "**" << endl;
    cout << T.search(3) << "**" << endl;
    cout << T.search(4) << "**" << endl;
    cout << T.search(5) << "**" << endl;
    cout << "*******************" << endl;

    T.del(4);
    T.del(2);

    cout << T.search(1) << "**" << endl;
    cout << T.search(2) << "**" << endl;
    cout << T.search(3) << "**" << endl;
    /* cout << T.search(4) << "**" << endl; */
}
