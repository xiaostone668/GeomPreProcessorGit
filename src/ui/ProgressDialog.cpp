#include "ui/ProgressDialog.h"
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace PS {

ProgressDialog::ProgressDialog(QWidget* parent)
    : QDialog(parent)
    , m_wasCanceled(false)
{
    setupUI();
    setWindowTitle(tr("处理中"));
    setModal(true);
}

ProgressDialog::~ProgressDialog() = default;

void ProgressDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    m_label = new QLabel(tr("请稍候..."), this);
    mainLayout->addWidget(m_label);
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    mainLayout->addWidget(m_progressBar);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_cancelButton = new QPushButton(tr("取消"), this);
    connect(m_cancelButton, &QPushButton::clicked, this, &ProgressDialog::onCancel);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void ProgressDialog::setLabelText(const QString& text)
{
    m_label->setText(text);
}

void ProgressDialog::setRange(int minimum, int maximum)
{
    m_progressBar->setRange(minimum, maximum);
}

void ProgressDialog::setValue(int value)
{
    m_progressBar->setValue(value);
}

int ProgressDialog::value() const
{
    return m_progressBar->value();
}

void ProgressDialog::setCancelButtonText(const QString& text)
{
    m_cancelButton->setText(text);
}

void ProgressDialog::setCancelButtonVisible(bool visible)
{
    m_cancelButton->setVisible(visible);
}

void ProgressDialog::onCancel()
{
    m_wasCanceled = true;
    emit canceled();
}

void ProgressDialog::onProgress(int percent, const QString& message)
{
    setValue(percent);
    if (!message.isEmpty()) {
        setLabelText(message);
    }
}

} // namespace PS
