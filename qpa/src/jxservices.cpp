#include "jxservices.h"
#include <QDebug>
#include "JCommandHandler.h"
#include <QUrl>
#include <QGuiApplication>
#include <QWindow>
typedef OutgoingCommand OC;
JxServices::JxServices()
{
}

bool JxServices::openUrl(const QUrl &url)
{
    JCommand(OC::OPENSERVICE, QGuiApplication::focusWindow()->winId()) << url.toDisplayString();
    return true;
};