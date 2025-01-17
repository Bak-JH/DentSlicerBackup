/*
This class was originally implemented in qq23:

http://doc.qt.io/archives/qq/qq23-bonjour.html

Copyright (c) 2007, Trenton Schulz

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <QtCore/QSocketNotifier>
#include <QtNetwork/QHostInfo>

#include "bonjourrecord.h"
#include "bonjourserviceresolver.h"

BonjourServiceResolver::BonjourServiceResolver(QObject *parent)
    : QObject(parent), dnssref(0), bonjourSocket(0), bonjourPort(-1)
{
}

BonjourServiceResolver::~BonjourServiceResolver()
{
    cleanupResolve();
}

void BonjourServiceResolver::cleanupResolve()
{
    if (dnssref) {
        DNSServiceRefDeallocate(dnssref);
        dnssref = 0;
        delete bonjourSocket;
        bonjourPort = -1;
    }
    emit cleanedUp();
}

void BonjourServiceResolver::resolveBonjourRecord(const BonjourRecord &record)
{
    if (dnssref) {
        qWarning("resolve in process, aborting");
        return;
    }
    qDebug() << "record: " << record.serviceName << record.registeredType << record.replyDomain;


    DNSServiceErrorType err = DNSServiceResolve(&dnssref, 0, 0,
                                                record.serviceName.toUtf8().constData(),
                                                record.registeredType.toUtf8().constData(),
                                                record.replyDomain.toUtf8().constData(),
                                                (DNSServiceResolveReply)bonjourResolveReply, this);
    if (err != kDNSServiceErr_NoError) {
        qDebug() << "kDNSServiceErr: " << err;
        emit error(err);
    } else {
        int sockfd = DNSServiceRefSockFD(dnssref);
        if (sockfd == -1) {
            emit error(kDNSServiceErr_Invalid);
        } else {
            bonjourSocket = new QSocketNotifier(sockfd, QSocketNotifier::Read, this);
            connect(bonjourSocket, SIGNAL(activated(int)), this, SLOT(bonjourSocketReadyRead()));
        }
    }
}



void BonjourServiceResolver::bonjourSocketReadyRead()
{
    DNSServiceErrorType err = DNSServiceProcessResult(dnssref);
    if (err != kDNSServiceErr_NoError)
        emit error(err);
}


void BonjourServiceResolver::bonjourResolveReply(DNSServiceRef, DNSServiceFlags ,
                                    quint32 , DNSServiceErrorType errorCode,
                                    const char *, const char *hosttarget, quint16 port,
                                    quint16 , const char *, void *context)
{
    BonjourServiceResolver *serviceResolver = static_cast<BonjourServiceResolver *>(context);
    if (errorCode != kDNSServiceErr_NoError) {
        emit serviceResolver->error(errorCode);
        return;
    }
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        {
            port =  0 | ((port & 0x00ff) << 8) | ((port & 0xff00) >> 8);
        }
#endif
    serviceResolver->bonjourPort = port;
    QHostInfo::lookupHost(QString::fromUtf8(hosttarget),
                          serviceResolver, SLOT(finishConnect(const QHostInfo &)));
}


void BonjourServiceResolver::bonjourGetAddrReply(DNSServiceRef sdRef, 
    DNSServiceFlags flags, quint32 interfaceIndex, DNSServiceErrorType errorCode,
    const char* hostname, const sockaddr* address, uint32_t ttl, void* context)
{
    BonjourServiceResolver* serviceResolver = static_cast<BonjourServiceResolver*>(context);
    sockaddr_in* casted = (sockaddr_in*)address;
    char* ip = inet_ntoa(casted->sin_addr);
    emit serviceResolver->foundHostIP(QString(ip));
}

void BonjourServiceResolver::lookupAddress(const QString& hostName)
{
    DNSServiceErrorType err = DNSServiceGetAddrInfo(&dnssref, 0, 0,
        kDNSServiceProtocol_IPv4,
        hostName.toUtf8().constData(),
        (DNSServiceGetAddrInfoReply)bonjourGetAddrReply, this);

    if (err != kDNSServiceErr_NoError) {
        qDebug() << "kDNSServiceErr: " << err;
        emit error(err);
    }
    else {
        int sockfd = DNSServiceRefSockFD(dnssref);
        if (sockfd == -1) {
            emit error(kDNSServiceErr_Invalid);
        }
        else {
            bonjourSocket = new QSocketNotifier(sockfd, QSocketNotifier::Read, this);
            connect(bonjourSocket, SIGNAL(activated(int)), this, SLOT(bonjourSocketReadyRead()));
        }
    }
}


void BonjourServiceResolver::finishConnect(const QHostInfo &hostInfo)
{

    emit bonjourRecordResolved(hostInfo);
    QMetaObject::invokeMethod(this, "cleanupResolve", Qt::QueuedConnection);
}

void BonjourServiceResolver::handleError(DNSServiceErrorType err)
{
    qDebug() << "Resolver -- handling following error: " << err;
    cleanupResolve();
}
