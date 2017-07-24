#include <textchild.h>

TextChild::TextChild(QWidget* parent/*=0*/) : QTextEdit(parent), MdiChild(MdiChild::Text)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(friendlyTitle());

    isUntitled = true;
}

void TextChild::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = QString("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
}

bool TextChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("错误"),
                             QString::fromLocal8Bit("加载文件错误 ") +
                             QString("%1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    return true;
}

bool TextChild::save()
{
    if (isUntitled)
    {
        return saveAs();
    }
    else
    {
        return saveFile(curFile);
    }
}

bool TextChild::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("另存为..."),
                       curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool TextChild::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("错误"),
                             QString::fromLocal8Bit("加载文件错误 ") +
                             QString("%1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

QString TextChild::friendlyTitle() const
{
    return QFileInfo(curFile).fileName();
}

void TextChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void TextChild::documentWasModified()
{
    setWindowModified(document()->isModified());
}

bool TextChild::maybeSave()
{
    if (document()->isModified())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("提示"),
                                   QString("'%1' ").arg(friendlyTitle()) +
                                   QString::fromLocal8Bit("文件已经被修改。\n需要保存变化吗?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void TextChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(friendlyTitle() + "[*]");
}

