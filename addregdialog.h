#ifndef ADDREGDIALOG_H
#define ADDREGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>

class AddRegDialog : public QDialog {
public:
    AddRegDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Добавить регистр");

        auto *layout = new QFormLayout(this);
        addrEdit = new QLineEdit(this);
        nameEdit = new QLineEdit(this);

        // Можно добавить маску для ввода Hex адресов (например, 0x1000)
        addrEdit->setPlaceholderText("1000");

        layout->addRow("Адрес (Hex):", addrEdit);
        layout->addRow("Название:", nameEdit);

        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        layout->addWidget(buttons);

        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }

    QString getAddress() const { return addrEdit->text(); }
    QString getName() const { return nameEdit->text(); }

private:
    QLineEdit *addrEdit;
    QLineEdit *nameEdit;
};
#endif // ADDREGDIALOG_H
