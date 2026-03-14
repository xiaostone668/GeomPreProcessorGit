#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QProgressBar;
class QLabel;
class QPushButton;
QT_END_NAMESPACE

namespace PS {

/**
 * @brief 进度对话框
 */
class ProgressDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit ProgressDialog(QWidget* parent = nullptr);
    ~ProgressDialog();
    
    void setLabelText(const QString& text);
    void setRange(int minimum, int maximum);
    void setValue(int value);
    int value() const;
    
    void setCancelButtonText(const QString& text);
    void setCancelButtonVisible(bool visible);
    
    bool wasCanceled() const { return m_wasCanceled; }
    
public slots:
    void onCancel();
    void onProgress(int percent, const QString& message);
    
signals:
    void canceled();
    
private:
    void setupUI();
    
    QLabel* m_label = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QPushButton* m_cancelButton = nullptr;
    
    bool m_wasCanceled = false;
};

} // namespace PS

#endif // PROGRESSDIALOG_H
