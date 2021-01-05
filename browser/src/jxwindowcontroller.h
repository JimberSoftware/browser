#pragma once

#include <QMap>
#include <QSharedMemory>

class JPopupWindow;
class JimberX264Browser;

typedef QSharedPointer<JPopupWindow> JPopupWindowPtr;
typedef QSharedPointer<JimberX264Browser> JTabWindowPtr;

class JxWindowController
{
private:
    QMap<int, JPopupWindowPtr> m_popupWindows;
    QMap<int, JTabWindowPtr> m_tabWindows;
    JxWindowController() {}

public:
    JxWindowController(JxWindowController const &) = delete;
    void operator=(JxWindowController const &) = delete;

    static JxWindowController &instance();
    void addPopupWindow(JPopupWindowPtr window);

    void addTabWindow(JTabWindowPtr window);
    void removeWindow(int winId);
    void closeWindow(int winId);
};

