#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "geometry/PsBody.h"
#include <QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QToolBar;
class QDockWidget;
class QLabel;
class QProgressBar;
QT_END_NAMESPACE

namespace PS {

class Viewport3D;
class ModelTree;
class PropertyPanel;
class CheckResultDialog;
class SewingDialog;
class PatchingDialog;
class FaceRemoveDialog;
class BeamExtractDialog;

/**
 * @brief 主窗口
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
    static MainWindow* instance();
    
    // 文档操作
    void loadFile(const QString& filePath);
    void saveFile(const QString& filePath);
    
    // 体操作
    void setBody(const std::shared_ptr<Body>& body);
    std::shared_ptr<Body> body() const { return m_body; }
    bool hasBody() const { return m_body != nullptr && !m_body->isNull(); }
    
    // 选择
    void setSelectedFaces(const std::vector<Face>& faces);
    void setSelectedEdges(const std::vector<Edge>& edges);
    std::vector<Face> selectedFaces() const { return m_selectedFaces; }
    std::vector<Edge> selectedEdges() const { return m_selectedEdges; }
    
public slots:
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onImport();
    void onExport();
    void onExit();
    
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onSelectAll();
    void onDeselectAll();
    
    void onViewFit();
    void onViewFront();
    void onViewTop();
    void onViewRight();
    void onViewIso();
    void onViewWireframe();
    void onViewShaded();
    void onViewHiddenLine();
    
    // 五大功能
    void onSewing();
    void onCheckRepair();
    void onPatching();
    void onRemoveFace();
    void onExtractBeam();
    
    void onPreferences();
    void onAbout();
    
    void onSelectionChanged();
    void onBodyModified();
    
private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupDockWidgets();
    void setupStatusBar();
    void setupConnections();
    
    void updateWindowTitle();
    void updateUIState();
    void updateStatusMessage(const QString& message);
    
    // 子窗口
    void createDialogs();
    
    static MainWindow* s_instance;
    
    // 当前体
    std::shared_ptr<Body> m_body;
    std::vector<Face> m_selectedFaces;
    std::vector<Edge> m_selectedEdges;
    
    // UI组件
    Viewport3D* m_viewport = nullptr;
    ModelTree* m_modelTree = nullptr;
    PropertyPanel* m_propertyPanel = nullptr;
    
    // 对话框
    CheckResultDialog* m_checkDialog = nullptr;
    SewingDialog* m_sewingDialog = nullptr;
    PatchingDialog* m_patchingDialog = nullptr;
    FaceRemoveDialog* m_faceRemoveDialog = nullptr;
    BeamExtractDialog* m_beamExtractDialog = nullptr;
    
    // 菜单和工具栏
    QMenu* m_fileMenu = nullptr;
    QMenu* m_editMenu = nullptr;
    QMenu* m_viewMenu = nullptr;
    QMenu* m_geometryMenu = nullptr;
    QMenu* m_toolsMenu = nullptr;
    QMenu* m_helpMenu = nullptr;
    
    QToolBar* m_mainToolBar = nullptr;
    QToolBar* m_viewToolBar = nullptr;
    QToolBar* m_geometryToolBar = nullptr;
    
    // 状态栏
    QLabel* m_statusLabel = nullptr;
    QProgressBar* m_progressBar = nullptr;
    
    // 文件路径
    QString m_currentFilePath;
    bool m_isModified = false;
};

} // namespace PS

#endif // MAINWINDOW_H
