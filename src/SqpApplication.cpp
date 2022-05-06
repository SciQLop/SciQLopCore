#include "SciQLopCore/SqpApplication.h"

#include <SciQLopCore/Data/IDataProvider.h>
#include <SciQLopCore/DataSource/datasources.h>
//#include <DragAndDrop/DragDropGuiController.h>
//#include <Network/NetworkController.h>
#include <QThread>
//#include <Time/TimeController.h>


Q_LOGGING_CATEGORY(LOG_SqpApplication, "SqpApplication")

class SqpApplication::SqpApplicationPrivate
{
public:
    SqpApplicationPrivate()
    {
        // /////////////////////////////// //
        // Connections between controllers //
        // /////////////////////////////// //

    }

    virtual ~SqpApplicationPrivate()
    {

    }

    DataSources m_DataSources;

};


SqpApplication::SqpApplication(int& argc, char** argv)
        : QApplication { argc, argv }, impl { std::make_unique<SqpApplicationPrivate>() }
{
    this->setStyle(new MyProxyStyle(this->style()));
    qCDebug(LOG_SqpApplication()) << tr("SqpApplication construction") << QThread::currentThread();

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

}

SqpApplication::~SqpApplication() {}

void SqpApplication::initialize() {}


DataSources& SqpApplication::dataSources() noexcept
{
    return impl->m_DataSources;
}


