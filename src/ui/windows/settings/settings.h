#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class Settings; }
QT_END_NAMESPACE

class Settings : public QWidget {
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings() override;

private slots:
    void onSaveButtonClicked();
    void onCancelButtonClicked();
    void validatePassword();
    void onReceivedButtonClicked();
    void onSentButtonClicked();
    void onSendFileButtonClicked();

private:
    Ui::Settings *ui;
    void setupConnections();
    bool validatePasswordRequirements(const QString& password);
};

#endif // SETTINGS_H