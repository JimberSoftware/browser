
#include "jxwindowcontroller.h"
#include "jimberx264browser.h"
#include "jpopupwindow.h"

void JxWindowController::addTabWindow(JTabWindowPtr window)
{
    this->m_tabWindows.insert(window->winId(), window);
}

JxWindowController &JxWindowController::instance()
{
    static JxWindowController instance;
    return instance;
}
void JxWindowController::addPopupWindow(JPopupWindowPtr window)
{
    this->m_popupWindows.insert(window->winId(), window);
}

void JxWindowController::removeWindow(int winId)
{
    this->m_popupWindows.remove(winId);
    this->m_tabWindows.remove(winId);
}

void JxWindowController::closeWindow(int winId)
{
    JxWindowController::instance().removeWindow(winId);
}
void closeWindow(int winId){
    JxWindowController::instance().closeWindow(winId);
}