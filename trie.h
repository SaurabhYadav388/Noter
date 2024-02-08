#ifndef TRIE_H
#define TRIE_H

#include <QString>
#include <QObject>

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

    QStringList wordList;
    const QStringList& getAllWordList() const;
};

class TrieManager : public QObject
{

    Q_OBJECT

private:
    static Trie* myinstance;


public:
    static Trie* getTrieInstance();
    static void setTrieInstance(Trie *_myTrieInst);
    void trieInitialize();
    TrieManager();

signals:
    void trieInitializeCompleteSignal();
};

#endif // TRIE_H
