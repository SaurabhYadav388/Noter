#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include "trie.h"
#include <QObject>
#include <QTextEdit>

class SpellChecker : public QObject
{
    Q_OBJECT

public:
    SpellChecker(QTextEdit *textEdit);

    bool isSpelledCorrectly(const QString &word) const;

public slots:
    void checkSpellingRealTime();

private:
    QTextEdit *scTextEdit;
    Trie *triePtr = TrieManager::getTrieInstance();
};

#endif // SPELLCHECKER_H
