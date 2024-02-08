#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

#include "spellchecker.h"
#include <QThread>
#include "trie.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


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

        void onTextChanged();

        void on_actionChange_Font_triggered();
        void saveFontSettings(QFont &newFont);
        void loadFontSettings();

        void on_actionDark_Theme_triggered();
        void loadThemeSettings();

    public slots:
        void onTrieInitializeComplete();

    private:
        Ui::MainWindow *ui;
        QString file_path;
        QFont defaultInitialFont=QFont("Arial", 12);

        //use pointers instead of simple stack instance( internally Qt's parent-child
        //ownership hierarchy of QObjects, compiler tries to free even if stack mem popped )
        SpellChecker *spellChecker;
        QThread *spellCheckerThread;

        TrieManager *trieManager;
        QThread *trieInitializeThread;
};
#endif // MAINWINDOW_H
