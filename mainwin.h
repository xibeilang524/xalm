#pragma once

#include <QtGui>

class MdiChild;
class MainWin : public QMainWindow
{
    Q_OBJECT
public:
    MainWin(QWidget * parent = 0, Qt::WindowFlags flags = 0);

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void customEvent(QEvent* event);

private slots:
    void newChild();
    void openText();

    void save();
    void saveAs();
    void cut();
    void copy();
    void paste();

    void opt();
    void colorSetting();

    void about();
    void test();
    void testAuto();

    void updateMenus();
    void updateWindowMenu();
    MdiChild* createMdiChild();
    void setActiveSubWindow(QWidget *window);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString&);

    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;
    QToolBar *fileToolBar;

    QMenu *fileMenu;
    QAction *newAct;
    QAction *exitAct;

    QMenu *alamMenu;
    QMenu *eventMenu;

    QMenu *textMenu;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    QMenu *toolsMenu;
    QAction *colorAct;
    QAction *optAct;

    QMenu *windowMenu;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;

    QMenu *helpMenu;
    QAction *aboutAct;
    QAction *testAutoAct;
    QAction *testAct;
};

