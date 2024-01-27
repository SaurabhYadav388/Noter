#include "spellchecker.h"


//constructor of SpC
SpellChecker::SpellChecker(QTextEdit *textEdit)
    : QObject(textEdit), scTextEdit(textEdit)
{
    connect(scTextEdit, &QTextEdit::textChanged, this, &SpellChecker::checkSpellingRealTime);
}


bool SpellChecker::isSpelledCorrectly(const QString &word) const
{
    qDebug() << "inside spellcorrect;? message";
    // Convert the word to lowercase for case-insensitive comparison
    QString lowercaseWord = word.toLower();

    // Check if the lowercase word is in the dictionary
    //return dictionary.contains(lowercaseWord);
    return triePtr->search(lowercaseWord);              /////error possibility
}


void SpellChecker::checkSpellingRealTime()
{

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
                qDebug()<<"ha;";
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1); // Adjust for overshoot
                break;
            }

        }

        word = cursor.selectedText().trimmed();//for last space

        qDebug() << "Word before space: " << word;

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

