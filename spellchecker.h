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

public slots:
    void checkSpellingRealTime();
    void suggestSpelling(const QPoint& pos);

private:
    QTextEdit *scTextEdit;
    Trie *triePtr = TrieManager::getTrieInstance();
    bool isSpelledCorrectly(const QString &word) const;
    QStringList getWordSuggestions(const QString &misSpeltWord,const QStringList &wordList);
};

#endif // SPELLCHECKER_H
