#pragma once

#include <QtGui>
#include <mdichild.h>

class TextChild : public QTextEdit, public MdiChild
{
    Q_OBJECT
public:
    TextChild(QWidget* parent = 0);

    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString friendlyTitle() const;
    QString currentFile() const;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void documentWasModified();

private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);

    QString curFile;
    bool isUntitled;
};

inline QString TextChild::currentFile() const
{
    return curFile;
}

