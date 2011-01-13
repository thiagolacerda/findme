#include "mainwindow.h"
#ifdef Q_OS_SYMBIAN
#include <proengfactory.h>
#include <profile.hrh>
#include <e32base.h>
#include <e32def.h>
#include <e32svr.h>
#include <mproengengine.h>
#elif defined(Q_WS_MAEMO_5)
#include <QtDBus>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);
    m_location = 0;
    m_satellite = 0;

    m_manager = new QMessageManager(this);
    if (!m_manager)
        textEdit->append("Could not initialize QMessageManager");

    m_service = new QMessageService(this);
    if (!m_service)
        textEdit->append("Could not initialize QMessageService");

    locationTimer = new QTimer(this);
    connect(locationTimer, SIGNAL(timeout()), this, SLOT(locationTimeout()));

    replyMessage.clear();
}

MainWindow::~MainWindow()
{
    if (m_location != 0) {
        m_location->stopUpdates();
        delete m_location;
    }
    if (m_satellite != 0) {
        m_satellite->stopUpdates();
        delete m_satellite;
    }
}

void MainWindow::positionUpdated(const QGeoPositionInfo &pos)
{
    textEdit->append(QString("lat: %1, long: %2").arg(pos.coordinate().latitude()).arg(pos.coordinate().longitude()));
    position = pos;
    answerFindmeMessage(true);
    m_location->stopUpdates();
    m_satellite->stopUpdates();
    locationTimer->stop();
}

void MainWindow::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &list)
{
    /*for (int i = 0; i < list.size(); ++i) {
        textEdit->append(QString("Satellite in use %1, signal: %2").arg(i).arg(list.at(i).signalStrength()));
    }*/
}

void MainWindow::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &list)
{
    /*for (int i = 0; i < list.size(); ++i) {
        textEdit->append(QString("Satellite in view %1, signal: %2").arg(i).arg(list.at(i).signalStrength()));
    }*/
}

void MainWindow::initLocationMode()
{
    textEdit->append("initLocationMode");
    m_location = QGeoPositionInfoSource::createDefaultSource(0);
    if (m_location) {
        m_location->setUpdateInterval(5000);
        connect(m_location, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(positionUpdated(QGeoPositionInfo)));
    }

    m_satellite = QGeoSatelliteInfoSource::createDefaultSource(0);
    if (m_satellite) {
        connect(m_satellite, SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)), this, SLOT(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
        connect(m_satellite, SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)), this, SLOT(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    }

    if ((m_location == 0) || (m_satellite == 0)) {
        textEdit->append("This examples requires a valid location source and no valid location sources are available on this platform.");
    }
}

void MainWindow::initMessagingMode()
{
    textEdit->append("initMessagingMode!!");
    if (m_manager) {
        textEdit->append("initMessagingMode valid m_manager");
        m_notificationFilterSet.insert(m_manager->registerNotificationFilter(QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::InboxFolder)));
        connect(m_manager, SIGNAL(messageAdded(QMessageId,QMessageManager::NotificationFilterIdSet)), this, SLOT(messageReceived(QMessageId,QMessageManager::NotificationFilterIdSet)));
    }
}

void MainWindow::startGPS()
{
    m_location->startUpdates();
    m_satellite->startUpdates();
}

void MainWindow::messageReceived(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &filterId)
{
    textEdit->append("message!!");
    if (filterId.contains(m_notificationFilterSet)) {
        m_messageId = id;
        textEdit->append("message received");
        processSMS();
    }
}

void MainWindow::processSMS()
{
    QMessage message = m_manager->message(m_messageId);
    // SMS message body
    QString messageString = message.textContent();
    textEdit->append(QString("message: %1").arg(messageString));
    if (messageString == "#findme") {
        textEdit->append("here, request position");
/*#ifdef Q_OS_SYMBIAN
        if (m_profileEngine) {
            textEdit->append("setting profile");
            m_profileEngine->SetActiveProfileL(0);
            textEdit->append("profile set");
        } else {
            TInt err = 0;
            m_profileEngine = ProEngFactory::NewEngineL();
            CleanupReleasePushL(*m_profileEngine);
            TRAP(err, m_profileEngine->SetActiveProfileL(EProfileGeneralId));
            if (err)
                textEdit->append(QString("error: %1").arg(err));
            textEdit->append(QString("new profile: %1").arg(m_profileEngine->ActiveProfileId()));//activeProfile->ProfileName()->Name()));
            CleanupStack::PopAndDestroy(1);
        }
#endif*/
#ifdef Q_OS_SYMBIAN
/*        TInt err = 0;
        m_profileEngine = ProEngFactory::NewEngineL();
        CleanupReleasePushL(*m_profileEngine);*/
        textEdit->append("setting profile");
        //MProEngProfile* activeProfile = m_profileEngine->ActiveProfileL();
/*        textEdit->append(QString("profile: %1").arg(m_profileEngine->ActiveProfileId()));//activeProfile->ProfileName()->Name()));
        TRAP(err, m_profileEngine->SetActiveProfileL(EProfileGeneralId));
        if (err)
            textEdit->append(QString("error: %1").arg(err));
        textEdit->append(QString("new profile: %1").arg(m_profileEngine->ActiveProfileId()));//activeProfile->ProfileName()->Name()));
        CleanupStack::PopAndDestroy(1);
        textEdit->append("profile set!!");*/
#elif defined(Q_WS_MAEMO_5)
        QDBusMessage message = QDBusMessage::createMethodCall("com.nokia.profiled", "/com/nokia/profiled", "com.nokia.profiled", "set_profile");
        QList<QVariant> args;
        args.append("general");
        message.setArguments(args);
        QDBusConnection con = QDBusConnection::sessionBus();
        con.callWithCallback(message, this, SLOT(maemoProfileSetSucess()), SLOT(maemoProfileSetError()));
#endif
        //answerFindmeMessage(false);
        initLocationMode();
        startGPS();
        locationTimer->start(1800000);
    }
}

void MainWindow::answerFindmeMessage(bool geoLocationSucess)
{
    textEdit->append("answerFindMessage");
    QString geoLocation = "Unable to get phone's location. ";
    if (geoLocationSucess) {
        geoLocation = QString("At %1 Lat: %2, Long: %3. ").arg(position.timestamp().toString()).arg(position.coordinate().latitude()).arg(position.coordinate().longitude());
    }

    QMessageAccountId smsAccountId = QMessageAccount::defaultAccount(QMessage::Sms);
    QMessage original(m_messageId);
    QMessage reply(original.createResponseMessage(QMessage::ReplyToSender));
    replyMessage.append(geoLocation);
    reply.setBody(replyMessage);
    reply.setParentAccountId(smsAccountId);
    m_service->send(reply);
    textEdit->append(QString("message sent to: %1, body: %2").arg(reply.to().at(0).addressee()).arg(reply.textContent()));
    replyMessage.clear();
}

void MainWindow::locationTimeout()
{
    locationTimer->stop();
    answerFindmeMessage(false);
    m_location->stopUpdates();
    m_satellite->stopUpdates();
}

#ifdef Q_WS_MAEMO_5
void MainWindow::maemoProfileSetSucess()
{
    textEdit->append("maemoProfileSetSucess");
    replyMessage.append("Phone profile was set to General. ");
}

void MainWindow::maemoProfileSetError()
{
    textEdit->append("maemoProfileSetError");
    replyMessage.append("Unable to set phone profile to General. ");
}
#endif
