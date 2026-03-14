#include "ui/MainWindow.h"
#include "ui/Viewport3D.h"
#include "ui/ModelTree.h"
#include "ui/PropertyPanel.h"
#include "ui/CheckResultDialog.h"
#include "ui/SewingDialog.h"
#include "ui/PatchingDialog.h"
#include "ui/FaceRemoveDialog.h"
#include "ui/BeamExtractDialog.h"
#include "geometry/PsBody.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QDebug>

namespace PS {

MainWindow* MainWindow::s_instance = nullptr;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    s_instance = this;
    
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupDockWidgets();
    setupStatusBar();
    setupConnections();
    createDialogs();
    
    updateWindowTitle();
    updateUIState();
    
    statusBar()->showMessage(tr("就绪"));
}

MainWindow::~MainWindow()
{
    s_instance = nullptr;
}

MainWindow* MainWindow::instance()
{
    return s_instance;
}

void MainWindow::setupUI()
{
    setWindowIcon(QIcon(":/icons/app.png"));
    resize(1600, 1000);
    
    m_viewport = new Viewport3D(this);
    setCentralWidget(m_viewport);
}

void MainWindow::setupMenuBar()
{
    // 文件菜单
    m_fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    m_fileMenu->addAction(tr("新建(&N)"), this, &MainWindow::onNew, QKeySequence::New);
    m_fileMenu->addAction(tr("打开(&O)..."), this, &MainWindow::onOpen, QKeySequence::Open);
    m_fileMenu->addAction(tr("保存(&S)"), this, &MainWindow::onSave, QKeySequence::Save);
    m_fileMenu->addAction(tr("另存为(&A)..."), this, &MainWindow::onSaveAs);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("导入(&I)..."), this, &MainWindow::onImport);
    m_fileMenu->addAction(tr("导出(&E)..."), this, &MainWindow::onExport);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("退出(&X)"), this, &MainWindow::onExit);
    
    // 编辑菜单
    m_editMenu = menuBar()->addMenu(tr("编辑(&E)"));
    m_editMenu->addAction(tr("撤销(&U)"), this, &MainWindow::onUndo, QKeySequence::Undo);
    m_editMenu->addAction(tr("恢复(&R)"), this, &MainWindow::onRedo, QKeySequence::Redo);
    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("剪切(&T)"), this, &MainWindow::onCut, QKeySequence::Cut);
    m_editMenu->addAction(tr("复制(&C)"), this, &MainWindow::onCopy, QKeySequence::Copy);
    m_editMenu->addAction(tr("粘贴(&P)"), this, &MainWindow::onPaste, QKeySequence::Paste);
    m_editMenu->addAction(tr("删除(&D)"), this, &MainWindow::onDelete, QKeySequence::Delete);
    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("全选(&A)"), this, &MainWindow::onSelectAll);
    m_editMenu->addAction(tr("取消选择"), this, &MainWindow::onDeselectAll);
    
    // 视图菜单
    m_viewMenu = menuBar()->addMenu(tr("视图(&V)"));
    m_viewMenu->addAction(tr("适应窗口(&F)"), this, &MainWindow::onViewFit);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(tr("前视图(&F)"), this, &MainWindow::onViewFront);
    m_viewMenu->addAction(tr("顶视图(&T)"), this, &MainWindow::onViewTop);
    m_viewMenu->addAction(tr("右视图(&R)"), this, &MainWindow::onViewRight);
    m_viewMenu->addAction(tr("等轴测(&I)"), this, &MainWindow::onViewIso);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(tr("线框模式(&W)"), this, &MainWindow::onViewWireframe);
    m_viewMenu->addAction(tr("着色模式(&S)"), this, &MainWindow::onViewShaded);
    
    // 几何菜单 - 五大功能
    m_geometryMenu = menuBar()->addMenu(tr("几何(&G)"));
    m_geometryMenu->addAction(tr("缝合(&S)..."), this, &MainWindow::onSewing);
    m_geometryMenu->addAction(tr("检查修复(&C)..."), this, &MainWindow::onCheckRepair);
    m_geometryMenu->addSeparator();
    m_geometryMenu->addAction(tr("N边补面(&P)..."), this, &MainWindow::onPatching);
    m_geometryMenu->addAction(tr("删除面(&R)..."), this, &MainWindow::onRemoveFace);
    m_geometryMenu->addAction(tr("抽梁(&B)..."), this, &MainWindow::onExtractBeam);
    
    // 工具菜单
    m_toolsMenu = menuBar()->addMenu(tr("工具(&T)"));
    m_toolsMenu->addAction(tr("选项(&O)..."), this, &MainWindow::onPreferences);
    
    // 帮助菜单
    m_helpMenu = menuBar()->addMenu(tr("帮助(&H)"));
    m_helpMenu->addAction(tr("关于(&A)..."), this, &MainWindow::onAbout);
}

void MainWindow::setupToolBar()
{
    m_mainToolBar = addToolBar(tr("主工具栏"));
    m_mainToolBar->setMovable(false);
    
    m_mainToolBar->addAction(tr("新建"), this, &MainWindow::onNew);
    m_mainToolBar->addAction(tr("打开"), this, &MainWindow::onOpen);
    m_mainToolBar->addAction(tr("保存"), this, &MainWindow::onSave);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(tr("撤销"), this, &MainWindow::onUndo);
    m_mainToolBar->addAction(tr("恢复"), this, &MainWindow::onRedo);
    
    m_geometryToolBar = addToolBar(tr("几何工具"));
    m_geometryToolBar->setMovable(false);
    
    m_geometryToolBar->addAction(tr("缝合"), this, &MainWindow::onSewing);
    m_geometryToolBar->addAction(tr("检查修复"), this, &MainWindow::onCheckRepair);
    m_geometryToolBar->addSeparator();
    m_geometryToolBar->addAction(tr("补面"), this, &MainWindow::onPatching);
    m_geometryToolBar->addAction(tr("删除面"), this, &MainWindow::onRemoveFace);
    m_geometryToolBar->addAction(tr("抽梁"), this, &MainWindow::onExtractBeam);
}

void MainWindow::setupDockWidgets()
{
    // 模型树
    QDockWidget* treeDock = new QDockWidget(tr("模型树"), this);
    m_modelTree = new ModelTree(treeDock);
    treeDock->setWidget(m_modelTree);
    addDockWidget(Qt::LeftDockWidgetArea, treeDock);
    
    // 属性面板
    QDockWidget* propDock = new QDockWidget(tr("属性"), this);
    m_propertyPanel = new PropertyPanel(propDock);
    propDock->setWidget(m_propertyPanel);
    addDockWidget(Qt::RightDockWidgetArea, propDock);
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(tr("就绪"));
    statusBar()->addWidget(m_statusLabel);
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMaximumWidth(200);
    m_progressBar->setVisible(false);
    statusBar()->addPermanentWidget(m_progressBar);
}

void MainWindow::setupConnections()
{
    connect(m_viewport, &Viewport3D::selectionChanged, this, &MainWindow::onSelectionChanged);
}

void MainWindow::createDialogs()
{
    m_checkDialog = new CheckResultDialog(this);
    m_sewingDialog = new SewingDialog(this);
    m_patchingDialog = new PatchingDialog(this);
    m_faceRemoveDialog = new FaceRemoveDialog(this);
    m_beamExtractDialog = new BeamExtractDialog(this);
}

void MainWindow::setBody(const std::shared_ptr<Body>& body)
{
    m_body = body;
    m_viewport->setBody(body);
    m_modelTree->setBody(body);
    
    if (body && !body->isNull()) {
        updateStatusMessage(tr("已加载: %1面, %2边, %3顶点")
                          .arg(body->faceCount())
                          .arg(body->edgeCount())
                          .arg(body->vertexCount()));
    }
    
    updateUIState();
}

void MainWindow::loadFile(const QString& filePath)
{
    // TODO: 实现文件加载
    updateStatusMessage(tr("加载文件: %1").arg(filePath));
}

void MainWindow::saveFile(const QString& filePath)
{
    // TODO: 实现文件保存
    updateStatusMessage(tr("保存文件: %1").arg(filePath));
}

void MainWindow::updateWindowTitle()
{
    QString title = tr("Parasolid几何修复工具 v1.0.0");
    
    if (hasBody()) {
        title = tr("[未命名]") + " - " + title;
    }
    
    if (m_isModified) {
        title = "*" + title;
    }
    
    setWindowTitle(title);
}

void MainWindow::updateUIState()
{
    bool hasBody = this->hasBody();
    
    // 更新菜单状态
    // 根据是否有体来启用/禁用功能
    
    updateWindowTitle();
}

void MainWindow::updateStatusMessage(const QString& message)
{
    m_statusLabel->setText(message);
    qDebug() << "Status:" << message;
}

// ========== 槽函数 ==========

void MainWindow::onNew()
{
    // 创建新体
    setBody(std::make_shared<Body>());
}

void MainWindow::onOpen()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("打开文件"), QString(),
        tr("STEP文件 (*.stp *.step);;IGES文件 (*.igs *.iges);;Parasolid文件 (*.x_t *.x_b);;所有文件 (*.*)"));
    
    if (!filePath.isEmpty()) {
        loadFile(filePath);
    }
}

void MainWindow::onSave()
{
    if (m_currentFilePath.isEmpty()) {
        onSaveAs();
    } else {
        saveFile(m_currentFilePath);
    }
}

void MainWindow::onSaveAs()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("保存文件"), QString(),
        tr("STEP文件 (*.stp);;Parasolid文件 (*.x_t);;所有文件 (*.*)"));
    
    if (!filePath.isEmpty()) {
        m_currentFilePath = filePath;
        saveFile(filePath);
    }
}

void MainWindow::onImport() { onOpen(); }
void MainWindow::onExport() { onSaveAs(); }
void MainWindow::onExit() { close(); }

void MainWindow::onUndo()
{
    // TODO: 实现撤销
}

void MainWindow::onRedo()
{
    // TODO: 实现重做
}

void MainWindow::onCut() {}
void MainWindow::onCopy() {}
void MainWindow::onPaste() {}
void MainWindow::onDelete() {}
void MainWindow::onSelectAll() {}
void MainWindow::onDeselectAll() { m_viewport->clearSelection(); }

void MainWindow::onViewFit() { m_viewport->viewFitAll(); }
void MainWindow::onViewFront() { m_viewport->viewFront(); }
void MainWindow::onViewTop() { m_viewport->viewTop(); }
void MainWindow::onViewRight() { m_viewport->viewRight(); }
void MainWindow::onViewIso() { m_viewport->viewIso(); }
void MainWindow::onViewWireframe() { m_viewport->setViewMode(Viewport3D::Wireframe); }
void MainWindow::onViewShaded() { m_viewport->setViewMode(Viewport3D::Shaded); }

void MainWindow::onSewing()
{
    if (!hasBody()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载或创建一个体。"));
        return;
    }
    
    m_sewingDialog->setBody(m_body);
    m_sewingDialog->exec();
}

void MainWindow::onCheckRepair()
{
    if (!hasBody()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载或创建一个体。"));
        return;
    }
    
    m_checkDialog->setBody(m_body);
    m_checkDialog->runCheck();
    m_checkDialog->exec();
}

void MainWindow::onPatching()
{
    if (!hasBody()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载或创建一个体。"));
        return;
    }
    
    m_patchingDialog->setBody(m_body);
    m_patchingDialog->exec();
}

void MainWindow::onRemoveFace()
{
    if (!hasBody()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载或创建一个体。"));
        return;
    }
    
    m_faceRemoveDialog->setBody(m_body);
    m_faceRemoveDialog->exec();
}

void MainWindow::onExtractBeam()
{
    if (!hasBody()) {
        QMessageBox::warning(this, tr("警告"), tr("请先加载或创建一个体。"));
        return;
    }
    
    m_beamExtractDialog->setBody(m_body);
    m_beamExtractDialog->exec();
}

void MainWindow::onPreferences()
{
    // TODO: 选项对话框
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("关于 Parasolid几何修复工具"),
        tr("<h2>Parasolid几何修复工具 v1.0.0</h2>"
           "<p>基于Parasolid内核的专业几何修复工具</p>"
           "<p><b>功能特点:</b></p>"
           "<ul>"
           "<li>缝合 - 将多个面缝合为连续壳体</li>"
           "<li>检查修复 - 9种几何问题检测与修复</li>"
           "<li>N边补面 - 自动填充N边形孔洞</li>"
           "<li>删除面 - 智能删除并填补孔洞</li>"
           "<li>抽梁 - 从实体提取梁结构</li>"
           "</ul>"
           "<p>Parasolid Version: %1</p>")
        .arg(Kernel::versionString()));
}

void MainWindow::onSelectionChanged()
{
    m_selectedFaces = m_viewport->selectedFaces();
    m_selectedEdges = m_viewport->selectedEdges();
    
    updateStatusMessage(tr("已选择: %1面, %2边").arg(m_selectedFaces.size()).arg(m_selectedEdges.size()));
}

void MainWindow::onBodyModified()
{
    m_isModified = true;
    updateWindowTitle();
    m_modelTree->refresh();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_isModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("确认退出"),
            tr("当前文档已修改,是否保存?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Save) {
            onSave();
            event->accept();
        } else if (reply == QMessageBox::Discard) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

} // namespace PS
