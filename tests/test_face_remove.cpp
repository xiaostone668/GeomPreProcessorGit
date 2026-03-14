#include <QtTest>
#include "core/FaceRemoveEngine.h"
#include "geometry/PsBody.h"
#include "geometry/PsFace.h"

using namespace PS;

class TestFaceRemove : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testFaceRemoveOptions();
};

void TestFaceRemove::initTestCase()
{
}

void TestFaceRemove::testFaceRemoveOptions()
{
    FaceRemoveOptions options;
    QVERIFY(options.fillHoles);
    QCOMPARE(options.fillingStrategy, HoleFillingStrategy::Auto);
}

QTEST_MAIN(TestFaceRemove)
#include "test_face_remove.moc"
