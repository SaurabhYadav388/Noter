#include "trie.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStringList>

Trie::Trie() : root(new TrieNode()) {}

void Trie::insert(const QString& word) {
    TrieNode* trav = root;
    for (const QChar& ch : word) {
        char c=ch.toLatin1();
        if(trav->children[c-'a']==nullptr)
        {
            trav->children[c-'a']=new TrieNode();
        }
        trav=trav->children[c-'a'];
    }
    trav->isend=true;
}

bool Trie::search(const QString& word) {
    TrieNode* trav = root;
    for (const QChar& ch : word) {

        char c=ch.toLatin1();

        //add check if character is not alphabet.
        if(c < 'a' || c > 'z')
            return false;

        if(trav->children[c-'a']==nullptr)
        {
            return false;
        }
        trav=trav->children[c-'a'];
    }
    return trav->isend;
}


Trie* TrieManager::getTrieInstance(){
    static Trie* instance=nullptr; // Create the instance on first access
    //declare here because have to use constexpr to allow assign nullptr in class,but later cant reassign new Node
    if(!instance){
        instance = new Trie();
        qDebug()<<"new instance first time";
        QStringList words;
        QFile file(":/words.txt");

        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                words.append( line.split('\n') ); // Split into words
                //qDebug()<<words[words.size()-1];//check
            }
            file.close();
            for(int i=0;i<words.size();i++)
            {
                instance->insert( words[i].toLower() );
            }
        } else {
            qWarning() << "Error opening file";
        }

    }
    qDebug()<<"inside get instance";

    return instance;
}

