#include "spellchecker.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QWidget>
#include <QAction>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

//constructor of SpC
SpellChecker::SpellChecker(QTextEdit *textEdit)
    : QObject(textEdit), scTextEdit(textEdit)
{

    connect(scTextEdit, &QTextEdit::textChanged, this, &SpellChecker::checkSpellingRealTime);

    scTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(scTextEdit,SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(suggestSpelling(const QPoint&)) );
    qDebug()<<"spellchecker const."<<QThread::currentThread();
}


bool SpellChecker::isSpelledCorrectly(const QString &word) const
{
    // Convert the word to lowercase for case-insensitive comparison
    QString lowercaseWord = word.toLower();
    // Check if the lowercase word is in the dictionary
    //return dictionary.contains(lowercaseWord);
    return triePtr->search(lowercaseWord);
}


void SpellChecker::checkSpellingRealTime()
{
    qDebug()<<"checkspelling"<<QThread::currentThread();


    // Disconnect the textChanged signal temporarily
    disconnect(scTextEdit, &QTextEdit::textChanged, this, &SpellChecker::checkSpellingRealTime);

    QTextCursor cursor = scTextEdit->textCursor();

    // Check if the previous character is a space
    cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor,1);
    QString letter=cursor.selectedText();
    QString word;
    if (letter == " ") {
        // Space detected, select the word before it:

        // Move the cursor to the beginning of the word
        while (cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1)) {
            // Continue moving left until non-word character is reached
            QString txt=cursor.selectedText();
            if(txt[0]==' '){
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1); // Adjust for overshoot
                break;
            }

        }

        word = cursor.selectedText().trimmed();//for last space

        QTextCharFormat format;
        format.setForeground(Qt::red); // Set font color to red (or any other color)
        //format.setFontWeight(QFont::Bold); // Set font weight to bold (optional)

        if (isSpelledCorrectly(word))
        {
            qDebug() << "Correctly spelled word:" << word;
            //default format
            cursor.setCharFormat(QTextCharFormat());
        }
        else
        {
            qDebug() << "Incorrectly spelled word:" << word;
            cursor.setCharFormat(format);

            scTextEdit->setCurrentCharFormat(QTextCharFormat());

        }

    }

    // Reconnect the textChanged signal
    connect(scTextEdit, &QTextEdit::textChanged, this, &SpellChecker::checkSpellingRealTime);////////
}

void SpellChecker::suggestSpelling(const QPoint& pos)
{
    qDebug()<<"trigger"<<QThread::currentThread();
;
    QTextCursor cursor = scTextEdit->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);

    QString currentWord = cursor.selectedText().trimmed();
    qDebug()<<"currentWord:"<<currentWord;

    QMenu suggestionMenu;

    // Get suggestions for the misspelled word
    //QStringList suggestions =getWordSuggestions(currentWord,triePtr->getAllWordList());

    // QFuture<QStringList> suggestionsFuture =QtConcurrent::run(&SpellChecker::getWordSuggestions,this,currentWord,triePtr->getAllWordList());

    // //suggestionsFuture.waitForFinished();//no need to wait result by default waits till we get result.
    // QStringList suggestions = suggestionsFuture.result();
    QStringList suggestions;
    QStringList wordList=triePtr->getAllWordList();
    QList<QStringList> subLists;
    int partSize = wordList.size() / 4;
    for (int i = 0; i < 4; ++i) {
        int start = i * partSize;
        int end = std::min(start + partSize, int(wordList.size()));
        subLists.append(wordList.mid(start, end - start));
    }

    QList<QFuture<QStringList>> futures;
    for (const QStringList &subList : subLists) {
        QFuture<QStringList> future = QtConcurrent::run(&SpellChecker::getWordSuggestions, this, currentWord, subList);
        futures.append(future);
    }

    for (const QFuture<QStringList> &future : futures) {
        suggestions.append(future.result());
    }

    // Add actions for each suggestion
    for (const QString &suggestion : suggestions)
    {
        qDebug()<<"suggestion:"<<suggestion;

        QAction *action =suggestionMenu.addAction(suggestion);
        connect(action, &QAction::triggered, [this, suggestion,&cursor]() {
            if(cursor.hasSelection())
            {
                cursor.removeSelectedText();
                cursor.setCharFormat(QTextCharFormat());
                cursor.insertText(suggestion);
            }

        });
    }

    // Show the suggestion menu at the cursor position
    suggestionMenu.exec(scTextEdit->mapToGlobal(pos));
}




QStringList SpellChecker::getWordSuggestions(const QString &misSpeltWord,const QStringList &wordList)
{
    qDebug()<<"calc suggestions"<<QThread::currentThread();


    QStringList suggestions ={};

    const int len1 = misSpeltWord.length();

    for (const QString &word : wordList)
    {
        const int len2 = word.length();

        if( (len1-len2) > 1 || (len1-len2) < -1)
            continue;

        QVector<QVector<int>> dp(len1 + 1, QVector<int>(len2 + 1, 0));

        for (int i = 0; i <= len1; ++i)
            dp[i][0] = i;

        for (int j = 0; j <= len2; ++j)
            dp[0][j] = j;

        for (int i = 1; i <= len1; ++i) {
            for (int j = 1; j <= len2; ++j) {
                if(misSpeltWord[i - 1] == word[j - 1])
                    dp[i][j]=dp[i - 1][j - 1]+0;
                else
                    dp[i][j] = 1 + std::min( { dp[i - 1][j] , dp[i][j - 1] , dp[i-1][j-1] } );
            }
        }

        if(dp[len1][len2]==1)
            suggestions<<word;
    }

    // qDebug()<<"my sugg:"<<QThread::currentThread();
    // for(auto sug:suggestions)
    //     qDebug()<<sug;

    return suggestions;
}
