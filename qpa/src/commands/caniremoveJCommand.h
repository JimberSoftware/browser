#pragma once
#include "../jxwsserver.h"

#include <QObject>
#include <sstream>

class JCommand : QObject
{
private:
    QStringList m_values;

public:
    QString toQString()
    {
        QString str;
        for (auto value : m_values)
        {
            str.append(value);
            str.append(";");
        }
        return str;
    }

    void send()
    {
        // std::string time(QTime::currentTime().toString().toStdString().c_str());

        //    std::cout << "Your command: " << getValuesJoined() << '\n'
        //               << std::endl;
        JxWsServer::instance().sendToAllClients(toQString());
    }
};
