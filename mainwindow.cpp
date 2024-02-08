#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QFont>
#include <QFontDialog>
#include <QSettings>
#include <QThread>
#include <QThreadPool>
#include "spellChecker.h"
#include "trie.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //this->setCentralWidget(ui->textEdit);

    spellChecker=nullptr;//init so can delete easily
    spellCheckerThread=new QThread();///not really needed

    trieManager=new TrieManager();///
    trieInitializeThread=new QThread();///

    setWindowTitle("Untitled");

    ui->actionDark_Theme->setCheckable(true);

    //load the Font before Theme(qss),opposite cause irregular fonts.
    loadFontSettings();
    loadThemeSettings();

    //QThreadPool::globalInstance()->setMaxThreadCount(8);
    QThread::currentThread()->setObjectName("Main thread");
    qDebug()<<"mainwindout"<<QThread::currentThread();

    trieManager->moveToThread(trieInitializeThread);
    QObject::connect(trieInitializeThread,&QThread::started,trieManager,&TrieManager::trieInitialize);
    QObject::connect(trieManager,&TrieManager::trieInitializeCompleteSignal,this,&MainWindow::onTrieInitializeComplete);
    QObject::connect(trieManager,&TrieManager::trieInitializeCompleteSignal, trieInitializeThread, &QThread::quit);
    //deleting thread here,so no need to delete in destructor
    QObject::connect(trieInitializeThread,&QThread::finished,trieInitializeThread,&QObject::deleteLater);
    trieInitializeThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(spellChecker!=nullptr)
    {
        qDebug()<<"Spellcheck not nullptr, deleting...";
        delete spellChecker;
    }
    delete trieManager;
    //delete trieInitializeThread;////no need to delete,auto delete using connect() in constr. ,deleteing here :seg fault
}

void MainWindow::onTextChanged()
{
    // Trigger real-time spell checking
    spellChecker->checkSpellingRealTime();
}


void MainWindow::on_actionNew_triggered()
{
    MainWindow *newWindow = new MainWindow();
    // Ensure the new window is deleted when closed
    newWindow->setAttribute(Qt::WA_DeleteOnClose);
    newWindow->show();
}


void MainWindow::on_actionOpen_triggered()
{

    QString file_name=QFileDialog::getOpenFileName(this,"Open the file");
    QFile file(file_name);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,"cannot open","File not Opened");
        return;
    }

    file_path=file_name;//to store the opened file name

    QFileInfo fileInfo(file_name);
    QString winTitleName = fileInfo.fileName();

    setWindowTitle(winTitleName);

    QTextStream in(&file);
    QString text= in.readAll();
    ui->textEdit->setText(text);
    file.close();
}


void MainWindow::on_actionSave_triggered()
{
    if( file_path == "" )//means saving a new file;use save as
    {
        on_actionSave_As_triggered();
        return;
    }

    QFile file(file_path);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this,"cannot open","File not Opened");
        return;
    }

    QTextStream out(&file);
    QString text= ui->textEdit->toPlainText();
    out<<text;
    file.flush();
    file.close();
}


void MainWindow::on_actionSave_As_triggered()
{
    QString file_name=QFileDialog::getSaveFileName(this,"File Save As");

    QFile file(file_name);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this,"cannot open","File not Opened");
        return;
    }

    file_path=file_name;

    QFileInfo fileInfo(file_name);
    QString winTitleName = fileInfo.fileName();
    setWindowTitle(winTitleName);

    QTextStream out(&file);
    QString text= ui->textEdit->toPlainText();
    out<<text;
    file.flush();
    file.close();
}


void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
}


void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}


void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void MainWindow::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}


void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}

void MainWindow::on_actionChange_Font_triggered()
{
    bool ok;
    QSettings settings("MyOrg", "Noter");
    QFont prevFont= settings.value("font", defaultInitialFont).value<QFont>();    //if no font setting(very first) use defaultinitialfont=arial12:
    //above for highlighting current font in font option dialog.

    QFont newFont = QFontDialog::getFont(&ok, prevFont/* initial font*/, this);
    if (ok) {
        ui->textEdit->setFont(newFont);
        saveFontSettings(newFont);  // Save user preferences when the font is changed
    }

}
void MainWindow::saveFontSettings(QFont &newFont)
{
    QSettings settings("MyOrg", "Noter");
    settings.setValue("font", newFont);
}
void MainWindow::loadFontSettings()
{
    QSettings settings("MyOrg", "Noter");
    QFont savedFont = settings.value("font", defaultInitialFont).value<QFont>();    //if no font setting(very first) use defaultinitialfont=arial12:
    //Change/Set ui textfont
    ui->textEdit->setFont(savedFont);
}

void MainWindow::on_actionDark_Theme_triggered()
{
    bool darkThemeEnabled=ui->actionDark_Theme->isChecked();

    // Load the appropriate QSS file
    QFile file(darkThemeEnabled ? ":themes/darkTheme.qss" : ":themes/lightTheme.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);

    QSettings settings("MyOrg", "Noter");
    settings.setValue("darkThemeEnabled", darkThemeEnabled);
}

void MainWindow::loadThemeSettings(){
    QSettings settings("MyOrg", "Noter");
    bool darkThemeEnabled = settings.value("darkThemeEnabled", false).toBool();
    if(darkThemeEnabled)
        ui->actionDark_Theme->setChecked(true);

    // Load the appropriate QSS file
    QFile file(darkThemeEnabled ? ":themes/darkTheme.qss" : ":themes/lightTheme.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);

}

void MainWindow::onTrieInitializeComplete()
{
    qDebug()<<"ontrieInit complete function";
    //initialize spell check only if trieinit complete
    spellChecker = new SpellChecker(ui->textEdit);////////////////////////////////////////////////////////

}

