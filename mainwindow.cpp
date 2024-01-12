#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QComboBox>
#include <QFont>
#include <QFontDialog>



SimpleSpellChecker::SimpleSpellChecker()
{

}

bool SimpleSpellChecker::isSpelledCorrectly(const QString &word) const
{
    qDebug() << "inside spellcorrect;? message";
    // Convert the word to lowercase for case-insensitive comparison
    QString lowercaseWord = word.toLower();

    // Check if the lowercase word is in the dictionary
    //return dictionary.contains(lowercaseWord);
    return triePtr->search(lowercaseWord);
}

//constructor of RTSC
RealTimeSpellChecker::RealTimeSpellChecker(QTextEdit *textEdit, SimpleSpellChecker *spellChecker)
    : QObject(textEdit), RTtextEdit(textEdit), spellChecker(spellChecker)
{
    connect(RTtextEdit, &QTextEdit::textChanged, this, &RealTimeSpellChecker::checkSpellingRealTime);
}

void RealTimeSpellChecker::checkSpellingRealTime()
{

    // Disconnect the textChanged signal temporarily
    disconnect(RTtextEdit, &QTextEdit::textChanged, this, &RealTimeSpellChecker::checkSpellingRealTime);

    QTextCursor cursor = RTtextEdit->textCursor();

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

            if (spellChecker->isSpelledCorrectly(word))
            {
                qDebug() << "Correctly spelled word:" << word;
                //default format
                cursor.setCharFormat(QTextCharFormat());
            }
            else
            {
                qDebug() << "Incorrectly spelled word:" << word;
                cursor.setCharFormat(format);

                RTtextEdit->setCurrentCharFormat(QTextCharFormat());

            }

        }

    // Reconnect the textChanged signal
    connect(RTtextEdit, &QTextEdit::textChanged, this, &RealTimeSpellChecker::checkSpellingRealTime);////////
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //this->setCentralWidget(ui->textEdit);

    setWindowTitle("Untitled");


    // Set default font style and family
    QFont defaultFont("Arial", 12);  // Set your desired default font family and size

    // Set default font for text editor
    ui->textEdit->setFont(defaultFont);


    // Create the spell checker
    spellChecker = new SimpleSpellChecker();

    // Create the real-time spell checker and connect it to the text edit
    realTimeSpellChecker = new RealTimeSpellChecker(ui->textEdit, spellChecker);

}

MainWindow::~MainWindow()
{
    delete ui;


    delete spellChecker;
    delete realTimeSpellChecker;/////////
}

void MainWindow::onTextChanged()
{
    // Trigger real-time spell checking
    realTimeSpellChecker->checkSpellingRealTime();
}


void MainWindow::on_actionNew_triggered()
{
    MainWindow *newWindow = new MainWindow;
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

    setWindowTitle(file_name);

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

    setWindowTitle(file_name);

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
