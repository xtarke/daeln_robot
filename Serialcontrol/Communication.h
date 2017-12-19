/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QMutex>
#include <iostream>
#include <QtSerialPort/QtSerialPort>
#include "serialsettingsdialog.h"

class Communication : public QObject
{
    Q_OBJECT
    
    
private:
    QSerialPort *serial;
    QQueue<QByteArray> queue;

    bool checkPackage(QByteArray package, QByteArray response);
        
signals:
    void PackageReady(QByteArray package);
    
private slots:
    void SendData(QByteArray package);
    
public:
    Communication(QSerialPort &serial_);
    
    bool send_data(QByteArray data, uint8_t ackSize);
    
    QByteArray send_rcv_data(QByteArray data, uint8_t ackSize);
    static QByteArray make_pgk(QByteArray data);
    
    bool isReady() {return serial->isOpen(); }
    void SerialClose() {serial->close(); }
    bool SerialOpen() { return serial->open(QIODevice::ReadWrite); }
        
};

#endif // COMMUNICATION_H
