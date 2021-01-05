#include <iostream>
#include <QString>
#include <QObject>
#include <QDebug>
#include <QTime>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

typedef std::shared_ptr<std::vector<std::string>> vecStrPtr;

QT_BEGIN_NAMESPACE

class JxLog
{
  private:
    vecStrPtr _values;
    bool last = false;

  public:
    JxLog() : _values(std::make_shared<std::vector<std::string>>(std::vector<std::string>()))
    {
        last = true;
    }

    JxLog(const JxLog &other)
    {
        this->_values = other._values;
    }

    template <typename T>
    JxLog operator<<(T var)
    {
        _values->push_back(std::to_string(var));
        return *this;
    }

    JxLog operator<<(QString var)
    {
        _values->push_back(var.toStdString());
        return *this;
    }

    JxLog operator<<(std::string var)
    {
        _values->push_back(var);
        return *this;
    }

    JxLog operator<<(const char *var)
    {
        _values->push_back(std::string(var));
        return *this;
    }

    // JxLog operator<<(void *data)
    // JxLog operator<<(void *data)
    // {
    //     // (void)data; Does nothing gets rid of warning
    //     return *this;
    // }
    void printAll()
    {
        for (auto value : *_values)
        {
            std::cout << value;
        }
    }
    std::vector<std::string> getvalues()
    {
        return *_values;
    }
    std::string getValuesJoined()
    {
        std::stringstream ss;
        for (auto value : *_values)
        {
            ss << value << " ";
        }
        return ss.str();
    }

    ~JxLog()
    {
        if (last)
        {
            std::string time(QTime::currentTime().toString().toStdString().c_str());
            std::cout << '[' << time << "] " << getValuesJoined() << '\n';
        }
    }
};
QT_END_NAMESPACE