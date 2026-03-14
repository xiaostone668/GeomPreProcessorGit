#include "io/PsSTEPImporter.h"
#include "core/PsKernel.h"
#include "geometry/PsBody.h"
#include <QFile>
#include <QDebug>

namespace PS {

STEPImporter::STEPImporter()
{
}

STEPImporter::~STEPImporter() = default;

ImportResult STEPImporter::import(const QString& filePath, const ImportOptions& options)
{
    ImportResult result;
    
    qDebug() << "Importing STEP file:" << filePath;
    reportProgress(0, "Opening file...");
    
    if (!QFile::exists(filePath)) {
        result.errorMessage = QObject::tr("文件不存在: %1").arg(filePath);
        return result;
    }
    
    // 在实际环境中调用: PK_SESSION_set_transmit_format(PK_FORMAT_STEP)
    // PK_PART_receive(filePath.toUtf8().constData(), ...)
    
    reportProgress(50, "Reading entities...");
    
    // 创建测试体
    auto body = std::make_shared<Body>();
    result.bodies.push_back(body);
    
    reportProgress(100, "Done");
    result.success = true;
    result.entityCount = 1;
    
    return result;
}

QStringList STEPImporter::supportedFormats() const
{
    return QStringList() << "stp" << "step";
}

QString STEPImporter::formatDescription(const QString& format) const
{
    if (format == "stp" || format == "step") {
        return QObject::tr("STEP AP214/203");
    }
    return QString();
}

} // namespace PS
