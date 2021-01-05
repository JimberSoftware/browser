#ifndef JIMBERX264CLIPBOARD_H
#define JIMBERX264CLIPBOARD_H

#include <QtGui/qpa/qplatformclipboard.h>
#include "JCommandHandler.h"
#include <QMimeData>
#include <QBuffer>

QT_BEGIN_NAMESPACE

typedef QSharedPointer<QMimeData> QMimeDataPtr;

class JxClipboard : public QPlatformClipboard
{
public:
    QMimeDataPtr _data;
    JxClipboard()
    {
        _data = QMimeDataPtr::create();
    }
    virtual ~JxClipboard(){};

    bool ownsMode(QClipboard::Mode mode) const override
    {
        Q_UNUSED(mode);
        return true;
    }

    bool supportsMode(QClipboard::Mode mode) const override
    {
        Q_UNUSED(mode);
        return true;
    }

    QMimeData *mimeData(QClipboard::Mode mode) override
    {
        //we know its clipboard
        Q_UNUSED(mode);
        return _data.data();
    }

    void setMimeData(QMimeData *data, QClipboard::Mode mode) override
    {
        if (mode != QClipboard::Mode::Clipboard)
            return;
        _data = QMimeDataPtr(data);

        if (data->hasImage())
        {
            QImage image = qvariant_cast<QImage>(data->imageData());
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            image.save(&buffer, "PNG");
            QString imageBase64 = QString::fromLatin1(byteArray.toBase64().data());
            JCommand(OutgoingCommand::CLIPBOARD, QGuiApplication::focusWindow()->winId()) << "image" << imageBase64;
        }
        if (data->hasText())
        {
            JCommand(OutgoingCommand::CLIPBOARD, QGuiApplication::focusWindow()->winId()) << "text" << data->text().toUtf8().toBase64();
        }

        emitChanged(mode);
    }
};

QT_END_NAMESPACE

#endif