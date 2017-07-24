#include <iostream>
#include <signal.h>
#include <QtGui>
#include <QRegExp>
#include <QApplication>
#include <mainwin.h>

using namespace std;

typedef void (*sighandler)(int); // 函数指针，定义信号操作回调
static sighandler MySignal(int nSigNum, sighandler handler)
{
    struct sigaction saNew, saOld;
    saNew.sa_handler = handler;
    sigemptyset(&saNew.sa_mask);
    if (SIG_DFL != handler && SIG_IGN != handler)
        sigaddset(&saNew.sa_mask, nSigNum);

    saNew.sa_flags = 0;
    if (SIGALRM == nSigNum)
    {
#ifdef SA_INTERRUPT
        saNew.sa_flags |= SA_INTERRUPT;
#endif
    }
    else
    {
#ifdef SA_RESTART
        saNew.sa_flags |= SA_RESTART;
#endif
    }

    if (sigaction(nSigNum, &saNew, &saOld) < 0)
        return SIG_ERR;

    return saOld.sa_handler;
}

/// 屏蔽信号，确保程序脱离终端运行
static void initDaemon()
{
    MySignal(SIGTTOU, SIG_IGN);
    MySignal(SIGTTIN, SIG_IGN);
    MySignal(SIGTSTP, SIG_IGN);
    MySignal(SIGHUP, SIG_IGN);

#ifdef IGN_SIGCHLD
    signal(SIGCHLD, SIG_IGN);
#endif

    pid_t pid = fork();
    if (pid > 0)
        exit(0);
}

int main(int argc, char** argv)
{
    initDaemon();
    Q_INIT_RESOURCE(xalm);

    QApplication app(argc, argv);
    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    MainWin* win = new MainWin();
    win->show();

    return app.exec();
}
