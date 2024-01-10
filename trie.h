#ifndef TRIE_H
#define TRIE_H

#include <QString>

class TrieNode{
public:
    TrieNode *children[26];//default nullptr
    bool isend;//default false
};
class Trie {
private:
    TrieNode* root;

public:
    Trie();
    void insert(const QString& word);
    bool search(const QString& word);
};

class TrieManager {
public:

    static Trie* getTrieInstance();
private:
    TrieManager(); // Private constructor to prevent direct instantiation
};

#endif // TRIE_H
