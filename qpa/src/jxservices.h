
#pragma once


#include <QtGui/qpa/qplatformservices.h>

QT_BEGIN_NAMESPACE

class JxServices : public QPlatformServices {
    public:
    
    JxServices();
    
    bool openUrl(const QUrl &url);
    
    ~JxServices(){};
};

 QT_END_NAMESPACE