#include <iostream>
#include <mdichild.h>
#include <alarmchild.h>
#include <eventchild.h>
#include <textchild.h>
#include <testdlg.h>
#include <wtdlg.h>
#include <displaysetting.h>
#include <mainwin.h>

using namespace std;

/// 默认构造
MainWin::MainWin(QWidget* parent/*=0*/, Qt::WindowFlags flags/*=0*/) : QMainWindow(parent, flags)
{
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(QString::fromLocal8Bit("多窗口"));
}

/// 重载关闭事件
void MainWin::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow())
        event->ignore();
    else
    {
        writeSettings();
        event->accept();
    }
}

/// 事件派发
void MainWin::customEvent(QEvent* event)
{
    AlarmEvent* almevent = dynamic_cast<AlarmEvent*>(event);
    if (almevent)
    {
        AlarmData data = almevent->data(); 
	foreach (QMdiSubWindow *window, mdiArea->subWindowList())
        {
            MdiChild *mdiChild = dynamic_cast<MdiChild*>(window->widget());
            if (mdiChild && mdiChild->mdiType() != MdiChild::Text)
                QCoreApplication::postEvent(window->widget(), new AlarmEvent(data, event->type()));
        }
    }
}

// 新建窗口
void MainWin::newChild()
{
    MdiChild* child = createMdiChild();
    if (child == NULL) return;

    QWidget* subwin = NULL;
    if (child->mdiType() == MdiChild::Alarm)
    {
        AlarmChild* win = (AlarmChild*)child;
        subwin = dynamic_cast<QWidget*>(win);
    }
    else if (child->mdiType() == MdiChild::Event)
    {
        EventChild* win = (EventChild*)child;
        subwin = dynamic_cast<QWidget*>(win);
    }
    else if (child->mdiType() == MdiChild::Text)
    {
        TextChild* win = (TextChild*)child;
        subwin = dynamic_cast<QWidget*>(win);
        win->newFile();
    }

    subwin->show();
}

MdiChild *MainWin::createMdiChild()
{
    WtDlg dlg(this);
    if (dlg.exec() != QDialog::Accepted) return NULL;

    QWidget* child = NULL;
    if (dlg.wType() == MdiChild::Alarm)
        child = new AlarmChild(this);
    else if (dlg.wType() == MdiChild::Event)
        child = new EventChild(this);
    else if (dlg.wType() == MdiChild::Text)
        child = new TextChild(this);

    if (child)
        mdiArea->addSubWindow(child);

    return dynamic_cast<MdiChild*>(child);
}

void MainWin::about()
{
    QMessageBox::about(this, QString::fromLocal8Bit("关于..."),
                       QString::fromLocal8Bit("2017.07.11"));
}

void MainWin::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    MdiChild *child = activeMdiChild();
    textMenu->setEnabled(child && child->mdiType() == MdiChild::Text);
    saveAct->setEnabled(child && child->mdiType() == MdiChild::Text);
    saveAsAct->setEnabled(child && child->mdiType() == MdiChild::Text);
}

void MainWin::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());
    for (int i = 0; i < windows.size(); ++i)
    {
        MdiChild *child = dynamic_cast<MdiChild*>(windows.at(i)->widget());
        if (child == NULL) continue;

        QString text;
        if (i < 9)
            text = tr("&%1 %2").arg(i + 1).arg(child->friendlyTitle());
        else
            text = tr("%1 %2").arg(i + 1).arg(child->friendlyTitle());

        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

void MainWin::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), QString::fromLocal8Bit("新建.."), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(QString::fromLocal8Bit("新建一个窗口"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newChild()));

    openAct = new QAction(QIcon(":/images/open.png"), QString::fromLocal8Bit("打开..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(QString::fromLocal8Bit("打开文本文件"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openText()));

    exitAct = new QAction(QString::fromLocal8Bit("退出(&x)"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(QString::fromLocal8Bit("退出应用程序"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    cutAct = new QAction(QIcon(":/images/cut.png"), QString::fromLocal8Bit("剪切(&t)"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(QString::fromLocal8Bit("将当前选中内容放到剪切板"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), QString::fromLocal8Bit("拷贝(&C)"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(QString::fromLocal8Bit("将当前选中内容复制到剪切板"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), QString::fromLocal8Bit("粘贴"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(QString::fromLocal8Bit("将剪切板内容放到当前位置"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    saveAct = new QAction(QIcon(":/images/save.png"), QString::fromLocal8Bit("保存"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(QString::fromLocal8Bit("保存当前文件"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QString::fromLocal8Bit("另存为..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(QString::fromLocal8Bit("另存当前文档"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    colorAct = new QAction(QString::fromLocal8Bit("颜色..."), this);
    colorAct->setStatusTip(QString::fromLocal8Bit("报警颜色定义"));
    connect(colorAct, SIGNAL(triggered()), this, SLOT(colorSetting()));

    optAct = new QAction(QString::fromLocal8Bit("选项..."), this);
    optAct->setStatusTip(QString::fromLocal8Bit("有关该系统的选项"));
    connect(optAct, SIGNAL(triggered()), this, SLOT(opt()));

    closeAct = new QAction(QString::fromLocal8Bit("关闭(&o)"), this);
    closeAct->setStatusTip(QString::fromLocal8Bit("关闭当前活动窗口"));
    connect(closeAct, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(QString::fromLocal8Bit("关闭所有(&A)"), this);
    closeAllAct->setStatusTip(QString::fromLocal8Bit("关闭所有窗口"));
    connect(closeAllAct, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(QString::fromLocal8Bit("平铺(&T)"), this);
    tileAct->setStatusTip(QString::fromLocal8Bit("平铺所有窗口"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(QString::fromLocal8Bit("层叠(&C)"), this);
    cascadeAct->setStatusTip(QString::fromLocal8Bit("层叠所有窗口"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(QString::fromLocal8Bit("下一个(&x)"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(QString::fromLocal8Bit("移至下一窗口"));
    connect(nextAct, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(QString::fromLocal8Bit("上一个(&v)"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(QString::fromLocal8Bit("移至上一窗口"));
    connect(previousAct, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(QString::fromLocal8Bit("关于(&A)"), this);
    aboutAct->setStatusTip(QString::fromLocal8Bit("关于该应用"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    testAutoAct = new QAction(QIcon(":/images/alarm.png"), QString::fromLocal8Bit("测试"), this);
    testAutoAct->setCheckable(true);
    testAutoAct->setStatusTip(QString::fromLocal8Bit("模拟生成一个报警"));
    connect(testAutoAct, SIGNAL(triggered()), this, SLOT(testAuto()));

    testAct = new QAction(QIcon(":/images/alarm.png"), QString::fromLocal8Bit("报警模拟器"), this);
    testAct->setStatusTip(QString::fromLocal8Bit("报警模拟器"));
    connect(testAct, SIGNAL(triggered()), this, SLOT(test()));
}

void MainWin::createMenus()
{
    fileMenu = menuBar()->addMenu(QString::fromLocal8Bit("文件(&F)"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    textMenu = menuBar()->addMenu(QString::fromLocal8Bit("编辑(&E)"));
    textMenu->addAction(cutAct);
    textMenu->addAction(copyAct);
    textMenu->addAction(pasteAct);

    toolsMenu = menuBar()->addMenu(QString::fromLocal8Bit("工具(&T)"));
    toolsMenu->addAction(colorAct);
    toolsMenu->addAction(optAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(testAct);

    windowMenu = menuBar()->addMenu(QString::fromLocal8Bit("窗口(&W)"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(QString::fromLocal8Bit("帮助(&H)"));
    helpMenu->addAction(aboutAct);
}

void MainWin::createToolBars()
{
    fileToolBar = addToolBar(QString::fromLocal8Bit("标准"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(testAutoAct);
}

void MainWin::createStatusBar()
{
    statusBar()->showMessage(QString::fromLocal8Bit("就绪"));
}

void MainWin::readSettings()
{
    DisplaySetting* settings = DisplaySetting::instance();
    QPoint pos = settings->value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings->value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWin::writeSettings()
{
    DisplaySetting* settings = DisplaySetting::instance();
    settings->setValue("pos", pos());
    settings->setValue("size", size());
}

MdiChild *MainWin::activeMdiChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return dynamic_cast<MdiChild*>(activeSubWindow->widget());

    return NULL;
}

QMdiSubWindow *MainWin::findMdiChild(const QString &titleName)
{
    foreach (QMdiSubWindow *window, mdiArea->subWindowList())
    {
        MdiChild *mdiChild = dynamic_cast<MdiChild*>(window->widget());
        if (mdiChild && mdiChild->friendlyTitle() == titleName)
            return window;
    }

    return NULL;
}

void MainWin::setActiveSubWindow(QWidget *window)
{
    if (window)
        mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWin::openText()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName.isEmpty())  return;

    QMdiSubWindow *existing = findMdiChild(QFileInfo(fileName).fileName());
    if (existing)
    {
        mdiArea->setActiveSubWindow(existing);
        return;
    }

    TextChild* child = new TextChild(this);
    mdiArea->addSubWindow(child);

    if (child->loadFile(fileName))
    {
        statusBar()->showMessage(QString::fromLocal8Bit("文件已加载"), 2000);
        child->show();
    }
    else
        child->close();
}

void MainWin::save()
{
    MdiChild* child = activeMdiChild();
    if (child == NULL) return;

    if (child->mdiType() == MdiChild::Text)
    {
        TextChild* win = (TextChild*)child;
        if (win->save())
            statusBar()->showMessage(QString::fromLocal8Bit("文件被保存"), 2000);
    }
}

void MainWin::saveAs()
{
    MdiChild* child = activeMdiChild();
    if (child == NULL) return;

    if (child->mdiType() == MdiChild::Text)
    {
        TextChild* win = (TextChild*)child;
        if (win->saveAs())
            statusBar()->showMessage(QString::fromLocal8Bit("文件被保存"), 2000);
    }
}

void MainWin::cut()
{
    MdiChild* child = activeMdiChild();
    if (child == NULL) return;

    if (child->mdiType() == MdiChild::Text)
    {
        TextChild* win = (TextChild*)child;
        win->cut();
    }
}

void MainWin::copy()
{
    MdiChild* child = activeMdiChild();
    if (child == NULL) return;

    if (child->mdiType() == MdiChild::Text)
    {
        TextChild* win = (TextChild*)child;
        win->copy();
    }
}

void MainWin::paste()
{
    MdiChild* child = activeMdiChild();
    if (child == NULL) return;

    if (child->mdiType() == MdiChild::Text)
    {
        TextChild* win = (TextChild*)child;
        win->paste();
    }
}

void MainWin::colorSetting()
{
}

void MainWin::opt()
{
}

void MainWin::test()
{
    static TestDlg* dlg = NULL;
    if (dlg == NULL)
        dlg = new TestDlg(this);

    dlg->init();
    dlg->show();
}

void MainWin::testAuto()
{
    int type = AlarmEvent::NEW;
    QSharedPointer<AlarmData> ptr(AlarmData::createAlarmData());

    foreach (QMdiSubWindow *window, mdiArea->subWindowList())
    {
        MdiChild *mdiChild = dynamic_cast<MdiChild*>(window->widget());
        if (mdiChild && mdiChild->mdiType() != MdiChild::Text)
            QCoreApplication::postEvent(window->widget(), new AlarmEvent(*ptr.data(), (QEvent::Type)type));
    }

    /// 连续触发
    if (testAutoAct->isChecked())
        QTimer::singleShot(1000, this, SLOT(testAuto()));
}

