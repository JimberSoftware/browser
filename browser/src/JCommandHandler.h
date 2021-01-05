#pragma once
#include <iostream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
//#include "backtrace.h"
#include "proto.h"
#include "socketcommunication.h"

#define SPLITSIGN ";"

QT_BEGIN_NAMESPACE


typedef ServerToClientCommand OutgoingCommand;
typedef ClientToServerCommand IncomingCommand;

class JCommand
{
protected:
    enum class CommandCategory
    {
        Incoming,
        Outgoing
    };

    CommandCategory _category;
    int _windowId;

    QSharedPointer<QStringList> _values;
    JCommand() : _values(QSharedPointer<QStringList>::create())
    {
    }

public:
    CommandCategory category()
    {
        return _category;
    }
    QString arg(int index) const
    {
        return (*_values)[index];
    }

    JCommand(OutgoingCommand command, int windowId) : _values(QSharedPointer<QStringList>::create())
    {
        _windowId = windowId;
        *_values << QString::fromStdString(std::to_string(static_cast<int>(command)));
        _category = CommandCategory::Outgoing;
    }


    JCommand(QString commandMessage)
    {
        *_values = commandMessage.split(SPLITSIGN);
    }

    JCommand(const JCommand &other)
    {
        this->_values = other._values;
    }

    template <typename T>
    JCommand operator<<(T var)
    {
        *_values << QString::fromStdString(std::to_string(var));
        return *this;
    }

    JCommand operator<<(QString var)
    {
        *_values << var;
        return *this;
    }
    JCommand operator<<(QByteArray var)
    {
        *_values << QString(var);
        return *this;
    }

    JCommand operator<<(std::string var)
    {
        *_values << QString::fromStdString(var);
        return *this;
    }

    JCommand operator<<(const char *var)
    {
        *_values << var;
        return *this;
    }

   /* JCommand operator<<(void *data)
    {
        return *this;
    }*/
    void printAll()
    {
        for (auto value : *_values)
        {
            qInfo() << value;
        }
    }

    QStringList getvalues()
    {
        return *_values;
    }
    QString getValuesJoined()
    {
        return _values->join(";");
    }

    virtual ~JCommand()
    {
        //   qInfo() << " Command from QPA " << getValuesJoined();
        if (_category == CommandCategory::Outgoing)
        {
            QPACommunication::instance().sendToAllClients(getValuesJoined(), _windowId);
        }
    }
};

class JIncomingCommand : public JCommand
{
public:
    IncomingCommand getCommand()
    {
      
        return static_cast<IncomingCommand>((*_values)[0].toInt());
    }
    JIncomingCommand(IncomingCommand command) : JCommand()
    {
          
        *_values << QString::fromStdString(std::to_string(static_cast<int>(command)));
    }
    JIncomingCommand(QString commandMessage)
    {
        // commandMessage = commandMessage.replace("\\u", "");

        *_values = commandMessage.split(SPLITSIGN);

    }
    virtual ~JIncomingCommand() override
    {
    }
};

typedef OutgoingCommand OC;

QT_END_NAMESPACE