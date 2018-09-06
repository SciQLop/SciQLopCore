#include <Network/Downloader.h>
#include <memory>

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QAuthenticator>
#include <QVariant>
#include <QHash>
#include <QCoreApplication>
#include <QReadWriteLock>
#include <QMutex>
#include <QThread>
#include <QObject>
#include <QtConcurrent/QtConcurrent>

class Downloader::p_Downloader : public QObject
{
    Q_OBJECT
    using login_pair=QPair<QString,QString>;
    QNetworkAccessManager manager;
    QMutex manager_lock;
    QReadWriteLock auth_lock;
    QHash<QString,login_pair> auth;
    QReadWriteLock pending_requests_lock;
    QHash<QUuid,QNetworkReply*> pending_requests;

    QNetworkRequest buildRequest(const QString& url, const QString &user="", const QString &passwd="")
    {
        QNetworkRequest request;
        request.setUrl(QUrl(url));
        request.setRawHeader("User-Agent", "SciQLop 1.0");
        if(user!="" and passwd!="")
        {
            //might grow quickly since we can have tons of URLs for the same host
            auth[url]=login_pair(user,passwd);
            QString login = "Basic "+user+":"+passwd;
            request.setRawHeader("Authorization",login.toLocal8Bit());
        }
        return request;
    }
signals:
    void _get_sig(const QNetworkRequest& request,std::function<void (QNetworkReply* reply)> callback);
private slots:
    void _get(const QNetworkRequest& request,std::function<void (QNetworkReply* reply)> callback)
    {
        QNetworkReply *reply = manager.get(request);
        QObject::connect(reply, &QNetworkReply::finished, [reply=reply,callback=callback](){callback(reply);});
    }

public:
    explicit p_Downloader()
    {
        auto login_bambda = [this](QNetworkReply * reply, QAuthenticator * authenticator)
        {
            QWriteLocker loc{&auth_lock};
            if(auth.contains(reply->url().toString()))
            {
                auto login = auth[reply->url().toString()];
                authenticator->setUser(login.first);
                authenticator->setPassword(login.second);
            }
        };

        QObject::connect(&manager, &QNetworkAccessManager::authenticationRequired, login_bambda);
        QObject::connect(this, &p_Downloader::_get_sig, this,  &p_Downloader::_get, Qt::QueuedConnection);
    }

    Response get(const QString& url, const QString &user="", const QString &passwd="")
    {
        QNetworkAccessManager manager;
        auto login_bambda = [user=user,passwd=passwd](QNetworkReply * reply, QAuthenticator * authenticator)
        {
            authenticator->setUser(user);
            authenticator->setPassword(passwd);
        };
        QObject::connect(&manager, &QNetworkAccessManager::authenticationRequired, login_bambda);
        Response resp;
        QNetworkRequest request = buildRequest(url, user, passwd);
        auto reply = manager.get(request);
        while (reply->isRunning())
        {
            QCoreApplication::processEvents();
            QThread::usleep(1000);
        }
        QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        resp = Response(reply->readAll(), status_code.toInt());
        return resp;
    }

    QUuid getAsync(const QString &url, std::function<void (QUuid ,Response)> callback, const QString &user, const QString &passwd)
    {
        auto uuid = QUuid::createUuid();
        QNetworkRequest request = buildRequest(url, user, passwd);
        manager_lock.lock();
        QNetworkReply *reply = manager.get(request);
        manager_lock.unlock();
        auto callback_wrapper = [url, uuid, callback, this](){
            QNetworkReply* reply;
            {
                QWriteLocker locker(&pending_requests_lock);
                reply = pending_requests.take(uuid);
            }
            QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            Response resp = Response(reply->readAll(), status_code.toInt());
            {
                QWriteLocker loc{&auth_lock};
                auth.remove(url);
            }
            delete reply;
            callback(uuid, resp);
        };
        QObject::connect(reply, &QNetworkReply::finished, callback_wrapper);
        {
            QWriteLocker locker(&pending_requests_lock);
            pending_requests[uuid] = reply;
        }
        return uuid;
    }
    bool downloadFinished(QUuid uuid)
    {
        QReadLocker locker(&pending_requests_lock);
        if(pending_requests.contains(uuid))
        {
            auto req = pending_requests[uuid];
            return req->isFinished();
        }
        return true;
    }
};

Response Downloader::get(const QString &url, const QString &user, const QString &passwd)
{
    return  Downloader::instance().impl->get(url, user, passwd);
}

QUuid Downloader::getAsync(const QString &url, std::function<void (QUuid ,Response)> callback, const QString &user, const QString &passwd)
{
    return  Downloader::instance().impl->getAsync(url, callback, user, passwd);
}

bool Downloader::downloadFinished(QUuid uuid)
{
    return Downloader::instance().impl->downloadFinished(uuid);
}

Downloader::Downloader()
    :impl(spimpl::make_unique_impl<p_Downloader>())
{
}

#include "Downloader.moc"
