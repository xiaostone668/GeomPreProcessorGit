#include "core/PsKernel.h"
#include <QDebug>
#include <cstring>

namespace PS {

// ========== PsException ==========

PsException::PsException(const QString& message)
    : m_message(message)
{
}

PsException::PsException(const char* message)
    : m_message(QString::fromUtf8(message))
{
}

const char* PsException::what() const noexcept
{
    m_whatBuffer = m_message.toStdString();
    return m_whatBuffer.c_str();
}

// ========== Kernel ==========

Kernel& Kernel::instance()
{
    static Kernel instance;
    return instance;
}

Kernel::Kernel()
{
}

Kernel::~Kernel()
{
    if (m_initialized) {
        terminate();
    }
}

bool Kernel::initialize(const KernelOptions& options)
{
    if (m_initialized) {
        return true;
    }
    
    m_options = options;
    
    // 在实际环境中调用: PK_KERNEL_initialise()
    // 这里模拟初始化
    qDebug() << "Initializing Parasolid kernel...";
    qDebug() << "  Undo enabled:" << options.enableUndo;
    qDebug() << "  Undo stack size:" << options.undoStackSize;
    qDebug() << "  Default tolerance:" << options.defaultTolerance;
    
    m_initialized = true;
    m_lastError = ErrorCode::NoError;
    
    return true;
}

bool Kernel::terminate()
{
    if (!m_initialized) {
        return true;
    }
    
    // 在实际环境中调用: PK_KERNEL_terminate()
    qDebug() << "Terminating Parasolid kernel...";
    
    m_initialized = false;
    return true;
}

QString Kernel::getLastErrorString() const
{
    switch (m_lastError) {
        case ErrorCode::NoError: return "No error";
        case ErrorCode::NullEntity: return "Null entity";
        case ErrorCode::InvalidEntity: return "Invalid entity";
        case ErrorCode::OutOfMemory: return "Out of memory";
        case ErrorCode::DivisionByZero: return "Division by zero";
        case ErrorCode::SingularTransformation: return "Singular transformation";
        case ErrorCode::BadParameter: return "Bad parameter";
        case ErrorCode::UnknownError: return "Unknown error";
        default: return "Unknown error code";
    }
}

void Kernel::clearError()
{
    m_lastError = ErrorCode::NoError;
}

bool Kernel::isValidEntity(PK_ENTITY_t entity) const
{
    if (entity == PK_ENTITY_null) {
        return false;
    }
    // 在实际环境中调用: PK_ENTITY_is_valid(entity)
    return entity > 0;
}

EntityType Kernel::getEntityType(PK_ENTITY_t entity) const
{
    if (!isValidEntity(entity)) {
        return EntityType::UNKNOWN;
    }
    // 在实际环境中调用: PK_ENTITY_ask_type(entity)
    // 这里简化处理
    return EntityType::BODY;
}

void Kernel::deleteEntity(PK_ENTITY_t entity)
{
    if (isValidEntity(entity)) {
        // 在实际环境中调用: PK_ENTITY_delete(entity)
        qDebug() << "Deleting entity:" << entity;
    }
}

void Kernel::deleteEntities(const std::vector<PK_ENTITY_t>& entities)
{
    for (PK_ENTITY_t entity : entities) {
        deleteEntity(entity);
    }
}

bool Kernel::canUndo() const
{
    return m_initialized && m_options.enableUndo;
}

bool Kernel::canRedo() const
{
    return m_initialized && m_options.enableUndo;
}

bool Kernel::undo()
{
    if (!canUndo()) {
        return false;
    }
    // 在实际环境中调用: PK_UNDO_undo()
    qDebug() << "Undo";
    return true;
}

bool Kernel::redo()
{
    if (!canRedo()) {
        return false;
    }
    // 在实际环境中调用: PK_UNDO_redo()
    qDebug() << "Redo";
    return true;
}

void Kernel::clearUndoStack()
{
    if (m_initialized) {
        // 在实际环境中调用: PK_UNDO_clear()
        qDebug() << "Clearing undo stack";
    }
}

void Kernel::setOptions(const KernelOptions& options)
{
    m_options = options;
}

QString Kernel::versionString()
{
    return "31.1.0";
}

int Kernel::versionMajor()
{
    return 31;
}

int Kernel::versionMinor()
{
    return 1;
}

int Kernel::versionPatch()
{
    return 0;
}

void Kernel::setError(ErrorCode code)
{
    m_lastError = code;
}

// ========== KernelGuard ==========

KernelGuard::KernelGuard(const KernelOptions& options)
    : m_initialized(false)
{
    m_initialized = Kernel::instance().initialize(options);
}

KernelGuard::~KernelGuard()
{
    Kernel::instance().terminate();
}

// ========== EntityGuard ==========

EntityGuard::EntityGuard(PK_ENTITY_t entity)
    : m_entity(entity)
    , m_owned(true)
{
}

EntityGuard::~EntityGuard()
{
    if (m_owned && m_entity != PK_ENTITY_null) {
        Kernel::instance().deleteEntity(m_entity);
    }
}

void EntityGuard::release()
{
    m_owned = false;
}

void EntityGuard::reset(PK_ENTITY_t entity)
{
    if (m_owned && m_entity != PK_ENTITY_null) {
        Kernel::instance().deleteEntity(m_entity);
    }
    m_entity = entity;
    m_owned = true;
}

} // namespace PS
