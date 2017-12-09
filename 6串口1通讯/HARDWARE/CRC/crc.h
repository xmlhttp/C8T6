/* 
 * File:   crc.h
 * Author: eduard.soltuzu
 *
 * Created on October 2, 2015, 10:48 AM
 */

#ifndef CRC_H
#define CRC_H

#include <stdint.h>
uint16_t CRC16( uint8_t * pucFrame, uint16_t usLen );
#endif
