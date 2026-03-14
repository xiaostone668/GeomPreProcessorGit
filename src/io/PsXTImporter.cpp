#include "io/PsXTImporter.h"
#include "core/PsKernel.h"
#include "geometry/PsBody.h"
#include <QFile>
#include <QDebug>

namespace PS {

XTImporter::XTImporter()
{
}

XTImporter::~XTImporter() = default;

ImportResult XTImporter::import(const QString& filePath, const ImportOptions& options)
{
    ImportResult result;
    
    qDebug() << "Importing XT file:" << filePath;
    reportProgress(0, "Opening file...");
    
    if (!QFile::exists(filePath)) {
        result.errorMessage = QObject::tr("文件不存在: %1").arg(filePath);
        return result;
    }
    
    // 在实际环境中调用: PK_PART_receive
    // Parasolid原生格式支持
    
    reportProgress(50, "Reading bodies...");
    
    // 创建测试体
    auto body = std::make_shared<Body>();
    result.bodies.push_back(body);
    
    reportProgress(100, "Done");
    result.success = true;
    result.entityCount = 1;
    
    return result;
}

ImportResult XTImporter::importFromMemory(const QByteArray& data, const ImportOptions& options)
{
    ImportResult result;
    
    qDebug() << "Importing XT from memory, size:" << data.size();
    
    // 在实际环境中调用: PK_PART_receive_from_memory
    
    auto body = std::make_shared<Body>();
    result.bodies.push_back(body);
    result.success = true;
    
    return result;
}

QStringList XTImporter::supportedFormats() const
{
    return QStringList() << "x_t" << "x_b" << "xt";
}

QString XTImporter::formatDescription(const QString& format) const
{
    if (format == "x_t" || format == "xt") {
        return QObject::tr("Parasolid XT (文本)");
    } else if (format == "x_b") {
        return QObject::tr("Parasolid XB (二进制)");
    }
    return QString();
}

} // namespace PS
