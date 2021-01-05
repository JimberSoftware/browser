
#pragma once

#include <QtGui/qpa/qplatforminputcontext.h>
#include <QtGui/qpa/qplatforminputcontextfactory_p.h>

QT_BEGIN_NAMESPACE

class JxInputContext : public QPlatformInputContext
{
  Q_OBJECT
public:
  JxInputContext();
  virtual void showInputPanel() override;
  virtual void hideInputPanel() override;
  virtual bool isInputPanelVisible() const override;

private:
  bool visible = false;
};

QT_END_NAMESPACE