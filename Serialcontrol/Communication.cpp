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

#include "Communication.h"
#include <iostream>
#include <QDebug>

Communication::Communication(QSerialPort& serial_)
{
    serial = &serial_;

}

//void Communication::set_serial(SettingsDialog::SerialSettings p)
//{
//
//}

void Communication::SendData(QByteArray package){
    
    QByteArray data = send_rcv_data(package, 6);
    
    queue.enqueue(data);
    
    if (!queue.isEmpty())
        emit PackageReady(queue.dequeue());
}

bool Communication::checkPackage(QByteArray package, QByteArray response){

}

bool Communication::send_data(QByteArray data, uint8_t ackSize)
{
    bool ret = false;

    /* Check if device is open */
    if (!serial->isOpen())
        return false;
    
    /* Discard buffered data */
    serial->readAll();
        
    /* Send and flush */    
    serial->write(data);
    serial->flush();
    
    /* ACK check */
    serial->waitForReadyRead(10);

    QByteArray requestData = serial->readAll();
        
    /* Receive 6 bytes as reply */
    while (serial->waitForReadyRead(100)){
       requestData += serial->readAll();
    
        if (requestData.size() == ackSize){
            ret = true;
            break;
        }
    } 
        
//    if (!ret){
//       std::cout << "-------------------" << std::endl;

//         for (int i=0; i < requestData.size(); i++){
//              std::cout << i << "  :   " << std::hex << (uint)requestData[i] << std::endl;
//         }
//    }

    return ret;
}

QByteArray Communication::send_rcv_data(QByteArray data, uint8_t ackSize){
    QByteArray requestData;
    
    /* Check if device is open */
     if (!serial->isOpen())
         return requestData;

    /* Discard buffered data */
    serial->readAll();
        
    /* Send and flush */    
    serial->write(data);
    serial->flush();
      
    /* ACK check */
    serial->waitForReadyRead(50);

    requestData = serial->readAll();
        
    /* Receive 6 bytes as reply */
    while (serial->waitForReadyRead(100)){
       requestData += serial->readAll();
    
        if (requestData.size() == ackSize){
            break;
        }
    } 
    
    return requestData;   
}


QByteArray Communication::make_pgk(QByteArray data)
{
    QByteArray package;
    uint8_t checksum = 0xff;
    
    package += 0x7e;                                     //Inicializador - ST
    package += data.size();                               //Tamanho	- SZ - Tamanho do pacote em bytes (ID e DT)
    
    for (int i=0; i < data.size(); i++){
        package += data[i];                                    //Identificador de comando - ID - 01 Manda Porcentagem Servo
        checksum -= data[i];    
    }
    
    package += checksum;     //Checksum
       
    return package;    
}
