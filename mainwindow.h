#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QTextEdit>
#include <QTimer>
#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>
#include <qmessagemanager.h>
#include <qmessageservice.h>
#ifdef Q_OS_SYMBIAN
//#include <mproengengine.h>
#elif defined (Q_WS_MAEMO_5)
#include <QDBusMessage>
#endif

QTM_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void positionUpdated(const QGeoPositionInfo &pos);
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &list);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &list);

    void messageReceived(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &filterId);
    void answerFindmeMessage(bool geoLocationSucess);

    void locationTimeout();
#ifdef Q_WS_MAEMO_5
    void maemoProfileSetSucess();
    void maemoProfileSetError();
#endif

private:
    QGeoPositionInfoSource *m_location;
    QGeoSatelliteInfoSource *m_satellite;
    QGeoPositionInfo position;
    QTextEdit *textEdit;

    QMessageManager *m_manager;
    QMessageService *m_service;
    QMessageManager::NotificationFilterIdSet m_notificationFilterSet;
    QMessageId m_messageId;

    QTimer *locationTimer;

    void processSMS();
    void startGPS();

    QString replyMessage;
/*#ifdef Q_OS_SYMBIAN
    MProEngEngine *m_profileEngine;
#endif*/

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initLocationMode();
    void initMessagingMode();
};

#endif // MAINWINDOW_H
