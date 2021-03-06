/***************************************************************************
 *            netcomm.cpp
 *
 *  Wed Jun 7 12:00:00 CEST 2017
 *  Copyright 2017 Lars Muldjord
 *  muldjordlars@gmail.com
 ****************************************************************************/
/*
 *  This file is part of skyscraper.
 *
 *  skyscraper is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  skyscraper is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with skyscraper; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#include "netcomm.h"

#include <QUrl>
#include <QNetworkRequest>

NetComm::NetComm()
{
  //connect(this, &NetComm::finished, this, &NetComm::replyFinished);
  requestTimer.setSingleShot(true);
  requestTimer.setInterval(60000);
  connect(&requestTimer, &QTimer::timeout, this, &NetComm::cancelRequest);
}

void NetComm::request(QString query, QString postData, QString headerKey, QString headerValue)
{
  clearAll();
  QUrl url(query);
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/57.0.2987.133 Safari/537.36");
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  if(!headerKey.isEmpty() && !headerValue.isEmpty()) {
    request.setRawHeader(headerKey.toUtf8(), headerValue.toUtf8());
  }
  
  if(postData.isEmpty()) {
    reply = get(request);
  } else {
    reply = post(request, postData.toUtf8());
  }
  connect(reply, &QNetworkReply::finished, this, &NetComm::replyReady);
  requestTimer.start();
}

void NetComm::replyReady()
{
  disconnect(reply, &QNetworkReply::finished, this, &NetComm::replyReady);
  requestTimer.stop();
  contentType = reply->rawHeader("Content-Type");
  data = reply->readAll();
  reply->deleteLater();

  emit dataReady();
}

QByteArray NetComm::getData()
{
  return data;
}

QByteArray NetComm::getContentType()
{
  return contentType;
}

void NetComm::cancelRequest()
{
  disconnect(reply, &QNetworkReply::finished, this, &NetComm::replyReady);
  reply->abort();
  reply->deleteLater();
  clearAll();

  emit dataReady();
}

void NetComm::clearAll()
{
  //clearAccessCache();
  contentType.clear();
  data.clear();
}
