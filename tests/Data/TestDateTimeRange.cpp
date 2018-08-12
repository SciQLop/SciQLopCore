#include <QObject>
#include <QtTest>
#include <QUuid>
#include <limits>


#include <Data/DateTimeRange.h>
#include <Common/Numeric.h>

Q_DECLARE_METATYPE(Seconds<double>);

DateTimeRange computeZoom(QDateTime start, QDateTime stop, double zoom)
{
    double start_epoch = start.toMSecsSinceEpoch();
    double stop_epoch = stop.toMSecsSinceEpoch();
    auto delta = stop_epoch - start_epoch;
    auto dt_ms = (zoom-1.)*(double(delta))/2.;
    return DateTimeRange{(start_epoch - dt_ms)/1000., (stop_epoch + dt_ms)/1000.};
}

class TestDateTimeRange: public QObject {
    Q_OBJECT

private slots:
    void testRangeDelta_data()
    {
        QTest::addColumn<QDateTime>("tstart");
        QTest::addColumn<QDateTime>("tend");
        QTest::addColumn<double>("expected");
        auto now = QDateTime::currentDateTime();
        auto yesterday = QDateTime::currentDateTime().addDays(-1);
        QTest::newRow("No delta") << now << now << 0.;
        QTest::newRow("One day delta") << yesterday << now << 60.*60.*24.;
        QTest::newRow("Minus one day delta") << now << yesterday << -60.*60.*24.;
    }

    void testRangeDelta()
    {
        QFETCH(QDateTime,tstart);
        QFETCH(QDateTime,tend);
        QFETCH(double,expected);
        auto range = DateTimeRange::fromDateTime(tstart, tend);
        double delta = range.delta();
        // Since it is built from QDateTime don't expect better resolution
        QVERIFY((delta-expected) <= 0.002);
    }

    void testRangeShift_data()
    {
        QTest::addColumn<DateTimeRange>("initial");
        QTest::addColumn<Seconds<double>>("shift");
        QTest::addColumn<DateTimeRange>("expected");
        auto now = QDateTime::currentDateTime();
        auto yestd = QDateTime::currentDateTime().addDays(-1);
        auto range = DateTimeRange::fromDateTime(yestd, now);
        QTest::newRow("No shift") << range << Seconds<double>{0.} << range;

        QTest::newRow("One milisecond left") << range << Seconds<double>{-.001} <<
                                            DateTimeRange::fromDateTime(yestd.addMSecs(-1.), now.addMSecs(-1.));
        QTest::newRow("One milisecond right") << range << Seconds<double>{.001} <<
                                             DateTimeRange::fromDateTime(yestd.addMSecs(1.), now.addMSecs(1.));
        QTest::newRow("One second left") << range << Seconds<double>{-1.} <<
                                            DateTimeRange::fromDateTime(yestd.addSecs(-1.), now.addSecs(-1.));
        QTest::newRow("One second right") << range << Seconds<double>{1.} <<
                                             DateTimeRange::fromDateTime(yestd.addSecs(1.), now.addSecs(1.));
        QTest::newRow("One year left") << range << Seconds<double>{-365.*24.*60.*60.} <<
                                             DateTimeRange::fromDateTime(yestd.addYears(-1.), now.addYears(-1.));
        QTest::newRow("One year right") << range << Seconds<double>{365.*24.*60.*60.} <<
                                             DateTimeRange::fromDateTime(yestd.addYears(1.), now.addYears(1.));
    }
    void testRangeShift()
    {
        QFETCH(DateTimeRange,initial);
        QFETCH(Seconds<double>,shift);
        QFETCH(DateTimeRange,expected);
        QCOMPARE(initial+shift, expected);
    }

    void testRangeZoom_data()
    {
        QTest::addColumn<DateTimeRange>("initial");
        QTest::addColumn<double>("zoom");
        QTest::addColumn<DateTimeRange>("expected");
        auto now = QDateTime::currentDateTime();
        auto yestd = QDateTime::currentDateTime().addDays(-1);
        auto range = DateTimeRange::fromDateTime(yestd, now);
        QTest::newRow("No zoom") << range << 1. << range;

        QTest::newRow("Zoom IN 0.001") << range << 1.001 <<
                                            computeZoom(yestd, now, 1.001);
        QTest::newRow("Zoom OUT 0.001") << range << 0.999 <<
                                            computeZoom(yestd, now, 0.999);
    }
    void testRangeZoom()
    {
        QFETCH(DateTimeRange,initial);
        QFETCH(double,zoom);
        QFETCH(DateTimeRange,expected);
        QCOMPARE(initial*zoom, expected);
    }

    void testRangeContains_data()
    {
        QTest::addColumn<DateTimeRange>("range");
        QTest::addColumn<DateTimeRange>("range2");
        QTest::addColumn<bool>("contains");
        auto now = QDateTime::currentDateTime();
        auto yestd = QDateTime::currentDateTime().addDays(-1);
        auto range = DateTimeRange::fromDateTime(yestd, now);
        QTest::newRow("Same range") << range << range << true;
        QTest::newRow("Smaller range") << range << range * 0.8 << true;
        QTest::newRow("Bigger range") << range << range * 1.2 << false;
        QTest::newRow("Shifted range with overlap") << range << range + Seconds<double>{1000.} << false;
        QTest::newRow("Shifted range without overlap") << range << range + Seconds<double>{24.*60.*60.*10} << false;

    }
    void testRangeContains()
    {
        QFETCH(DateTimeRange,range);
        QFETCH(DateTimeRange,range2);
        QFETCH(bool,contains);
        QCOMPARE(range.contains(range2), contains);
    }

    void testRangeIntersect_data()
    {
        QTest::addColumn<DateTimeRange>("range");
        QTest::addColumn<DateTimeRange>("range2");
        QTest::addColumn<bool>("contains");
        auto now = QDateTime::currentDateTime();
        auto yestd = QDateTime::currentDateTime().addDays(-1);
        auto tomorrow = QDateTime::currentDateTime().addDays(1);
        auto range = DateTimeRange::fromDateTime(yestd, now);
        auto range2 = DateTimeRange::fromDateTime(now, tomorrow);
        QTest::newRow("Same range") << range << range << true;
        QTest::newRow("Smaller range") << range << range * 0.8 << true;
        QTest::newRow("Bigger range") << range << range * 1.2 << true;
        QTest::newRow("Shifted range with overlap") << range << range + Seconds<double>{1000.} << true;
        QTest::newRow("Shifted range with overlaping boundary") << range << range2 << true;
        QTest::newRow("Shifted range without overlap") << range << range + Seconds<double>{24.*60.*60.*10} << false;

    }
    void testRangeIntersect()
    {
        QFETCH(DateTimeRange,range);
        QFETCH(DateTimeRange,range2);
        QFETCH(bool,contains);
        QCOMPARE(range.intersect(range2), contains);
    }
};
QTEST_MAIN(TestDateTimeRange)


#include "TestDateTimeRange.moc"
