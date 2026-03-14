#include <QtTest>
#include "core/CheckRepairEngine.h"
#include "geometry/PsBody.h"

using namespace PS;

class TestCheckRepair : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testCheckOptions();
    void testRepairOptions();
    void testIssueCreation();
};

void TestCheckRepair::initTestCase()
{
}

void TestCheckRepair::testCheckOptions()
{
    CheckOptions options;
    QVERIFY(options.checkSmallFace);
    QVERIFY(options.checkSmallEdge);
    QCOMPARE(options.smallFaceAreaThreshold, 1e-6);
}

void TestCheckRepair::testRepairOptions()
{
    RepairOptions options;
    QVERIFY(options.fixSmallFaces);
    QVERIFY(options.fixSmallEdges);
}

void TestCheckRepair::testIssueCreation()
{
    GeometryIssue issue;
    issue.type = CheckType::SmallFace;
    issue.severity = Severity::Warning;
    
    QVERIFY(issue.isFixable());
}

QTEST_MAIN(TestCheckRepair)
#include "test_check_repair.moc"
