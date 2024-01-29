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


TrieManager::TrieManager()
{

}

Trie* TrieManager::myinstance = nullptr;//initialize here

Trie* TrieManager::getTrieInstance(){
    qDebug()<<"inside get instance";
    return myinstance;
}

void TrieManager::setTrieInstance(Trie *_myInst)
{
    myinstance=_myInst;
}


void TrieManager::trieInitialize()
{
    if(TrieManager::myinstance==nullptr)
    {
        setTrieInstance(new Trie());//assigning instance mem.

        qDebug()<<"new instance initialization first time";
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
                myinstance->insert( words[i].toLower() );
            }
        }
        else
        {
            qWarning() << "Error opening file";
            return;
        }
    }
    else
    {
        qDebug()<<"Already initialized trie";
    }
    emit trieInitializeCompleteSignal(); ////emit signal that trie initialization is complete
}

