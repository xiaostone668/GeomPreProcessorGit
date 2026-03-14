#ifndef PATCHINGDIALOG_H
#define PATCHINGDIALOG_H

#include "geometry/PsTypes.h"
#include <QDialog>

QT_BEGIN_NAMESPACE
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;
class QProgressBar;
class QLabel;
class QListWidget;
QT_END_NAMESPACE

namespace PS {

class Body;

/**
 * @brief 补面对话框
 */
class PatchingDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit PatchingDialog(QWidget* parent = nullptr);
    ~PatchingDialog();
    
    void setBody(const std::shared_ptr<Body>& body);
    void setSelectedEdges(const std::vector<Edge>& edges);
    
    PatchingOptions options() const;
    
public slots:
    void onPatch();
    void onAutoPatch();
    void onPreview();
    void onSelectBoundary();
    void onClearSelection();
    
private:
    void setupUI();
    void setupConnections();
    void updateBoundaryList();
    void showProgress(const QString& message);
    void hideProgress();
    
    QComboBox* m_methodCombo = nullptr;
    QSpinBox* m_degreeSpin = nullptr;
    QSpinBox* m_uPatchesSpin = nullptr;
    QSpinBox* m_vPatchesSpin = nullptr;
    QDoubleSpinBox* m_toleranceSpin = nullptr;
    QDoubleSpinBox* m_maxErrorSpin = nullptr;
    QCheckBox* m_tangencyCheck = nullptr;
    QCheckBox* m_enforceBoundsCheck = nullptr;
    
    QListWidget* m_boundaryList = nullptr;
    
    QPushButton* m_patchButton = nullptr;
    QPushButton* m_autoPatchButton = nullptr;
    QPushButton* m_selectBoundaryButton = nullptr;
    QPushButton* m_clearButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
    
    QProgressBar* m_progressBar = nullptr;
    QLabel* m_statusLabel = nullptr;
    
    std::shared_ptr<Body> m_body;
    std::vector<Edge> m_selectedEdges;
    std::shared_ptr<Body> m_resultBody;
    
    bool m_isProcessing = false;
};

} // namespace PS

#endif // PATCHINGDIALOG_H
