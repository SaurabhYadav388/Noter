#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QComboBox>
#include <QFont>
#include <QFontDialog>
#include <QSettings>



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
    : QObject(textEdit), RTtextEdit(textEdit), RTspellChecker(spellChecker)
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

            if (RTspellChecker->isSpelledCorrectly(word))
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

    ui->actionDark_Theme->setCheckable(true);
    loadThemeSettings();
    // Set default font for text editor from saved setting or hardcoded defaultInitialfont in main class
    loadFontSettings();



    // Create the spell checker (and assign to global var)
    spellChecker = new SimpleSpellChecker();

    // Create the real-time spell checker(and assign to global var) and connect it to the text edit
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

