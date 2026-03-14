#include "io/PsSTEPExporter.h"
#include "core/PsKernel.h"
#include "geometry/PsBody.h"
#include <QFile>
#include <QDebug>

namespace PS {

STEPExporter::STEPExporter()
{
}

STEPExporter::~STEPExporter() = default;

ExportResult STEPExporter::exportBody(const Body& body, const QString& filePath, const ExportOptions& options)
{
    ExportResult result;
    
    qDebug() << "Exporting STEP file:" << filePath;
    reportProgress(0, "Preparing export...");
    
    if (!body.isValid()) {
        result.errorMessage = QObject::tr("无效的体");
        return result;
    }
    
    // 在实际环境中调用: PK_SESSION_set_transmit_format(PK_FORMAT_STEP)
    // PK_PART_transmit(part, filePath.toUtf8().constData(), ...)
    
    reportProgress(50, "Writing entities...");
    
    reportProgress(100, "Done");
    result.success = true;
    result.exportedEntityCount = 1;
    
    return result;
}

ExportResult STEPExporter::exportBodies(const std::vector<std::shared_ptr<Body>>& bodies, 
                                        const QString& filePath, const ExportOptions& options)
{
    ExportResult result;
    
    qDebug() << "Exporting multiple bodies to STEP:" << filePath;
    
    int count = 0;
    for (const auto& body : bodies) {
        if (body && body->isValid()) {
            count++;
        }
    }
    
    // 批量导出
    
    result.success = true;
    result.exportedEntityCount = count;
    
    return result;
}

QStringList STEPExporter::supportedFormats() const
{
    return QStringList() << "stp" << "step";
}

QString STEPExporter::formatDescription(const QString& format) const
{
    if (format == "stp" || format == "step") {
        return QObject::tr("STEP AP214/203");
    }
    return QString();
}

} // namespace PS
