/**
 * Parasolid Geometry Repair Tool
 * 基于Parasolid内核的几何修复工具软件
 */

#include "core/PsKernel.h"
#include "ui/MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    // 创建Qt应用
    QApplication app(argc, argv);
    
    // 设置应用信息
    app.setOrganizationName("ParasolidRepair");
    app.setApplicationName("ParasolidGeometryRepair");
    app.setApplicationDisplayName(QObject::tr("Parasolid几何修复工具"));
    app.setApplicationVersion("1.0.0");
    
    // 设置样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 设置调色板
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(palette);
    
    // 初始化Parasolid内核
    PS::KernelOptions kernelOptions;
    kernelOptions.enableUndo = true;
    kernelOptions.undoStackSize = 50;
    kernelOptions.enableLogging = true;
    
    PS::KernelGuard kernelGuard(kernelOptions);
    
    if (!kernelGuard.isInitialized()) {
        std::cerr << "Failed to initialize Parasolid kernel!" << std::endl;
        QMessageBox::critical(nullptr, QObject::tr("错误"), 
                             QObject::tr("无法初始化Parasolid内核,请检查许可证和配置。"));
        return 1;
    }
    
    qDebug() << "Parasolid kernel initialized successfully";
    qDebug() << "Version:" << PS::Kernel::versionString();
    
    // 创建并显示主窗口
    PS::MainWindow window;
    window.show();
    
    // 处理命令行参数
    if (argc > 1) {
        QString filePath = QString::fromLocal8Bit(argv[1]);
        window.loadFile(filePath);
    }
    
    // 运行应用
    return app.exec();
}
