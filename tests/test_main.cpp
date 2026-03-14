#include <QtTest>
#include <QCoreApplication>

// 主函数
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    int status = 0;
    
    // 运行所有测试
    // status |= QTest::qExec(new TestSewing, argc, argv);
    // status |= QTest::qExec(new TestCheckRepair, argc, argv);
    // status |= QTest::qExec(new TestPatching, argc, argv);
    
    return status;
}
