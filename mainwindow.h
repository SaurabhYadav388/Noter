#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
//
#include <QTextEdit>
#include <QTextCursor>
#include <QTextCharFormat>

#include <QFont>

#include "trie.h"//

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

///
class SimpleSpellChecker
{
    public:
        SimpleSpellChecker();

        bool isSpelledCorrectly(const QString &word) const;

    private:
        QSet<QString> dictionary;

        Trie *triePtr=TrieManager::getTrieInstance();

};
///
class RealTimeSpellChecker : public QObject
{
    Q_OBJECT

    public:
        RealTimeSpellChecker(QTextEdit *textEdit, SimpleSpellChecker *spellChecker);

    public slots:
        void checkSpellingRealTime();


    private:
        QTextEdit *RTtextEdit;
        SimpleSpellChecker *RTspellChecker;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);

        ~MainWindow();

    private slots:
        void on_actionNew_triggered();

        void on_actionCut_triggered();

        void on_actionOpen_triggered();

        void on_actionSave_triggered();

        void on_actionSave_As_triggered();

        void on_actionCopy_triggered();

        void on_actionPaste_triggered();

        void on_actionRedo_triggered();

        void on_actionUndo_triggered();


        void onTextChanged();//////


        void on_actionChange_Font_triggered();
        void saveFontSettings(QFont &newFont);
        void loadFontSettings();

    private:
        Ui::MainWindow *ui;
        QString file_path;

        QFont defaultInitialFont=QFont("Arial", 12);

        QTextEdit *mytextEdit;//not used anywhere...?
        SimpleSpellChecker *spellChecker;
        RealTimeSpellChecker *realTimeSpellChecker;//////these pointer will be used an given memory in main function;
        ////and passed as parameter to other classs constructor to get used
};
#endif // MAINWINDOW_H
