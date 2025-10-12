/*
// Copyright (c) 2021 Phoenix Technologies Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/
/**
 * @file    smbus.hpp
 *
 * @brief   SMBus API
 *
 * @details This file contains SMBus access functions for using in Phoenix sensor
 *          customization layer.
 */

#pragma once

#include <cstdint>

/**
 * @Name api_smbus_quick_command()
 *
 * @Description This function do SMBus Quick Command protocol transaction,
 *              sends a single bit (Rd/Wr) to the device. The Rd/Wr bit may be
 *              used to simply turn a device function on or off, or 
 *              enable/disable a low power standby mode. There is no data sent
 *              or received.
 * \verbatim
   Quick Command Protocol:

     +---+------+-------+-----+---+
     | S | Addr | Rd/Wr | [A] | P |
     +---+------+-------+-----+---+  

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *
 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  rw             - Send Rd or Wr bit. On Rd, set to 0, other
 *                              values mean send Wr bit. 
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_quick_command() returns 0;
 *         on error, it returns a nonzero value.
 *
 * @code Example code:
 *
 *  // Access device 14-0050 using Quick Command protocol, with Wr bit.
 *  api_smbus_quick_command (14, 0x50, 1, 0, 0);
 *
 * @endcode
 */
int api_smbus_quick_command(uint8_t bus, uint8_t addr, int retry, uint8_t rw,
                            int retry_delay_ms);


/**
 * @Name api_smbus_send_byte()
 *
 * @Description This function do SMBus Send Byte protocol transaction.
 *              Sends a single byte to the device.
 * \verbatim
   Send Byte Protocol:

     +---+------+----+-----+------+-----+---+
     | S | Addr | Wr | [A] | Data | [A] | P |
     +---+------+----+-----+------+-----+---+  

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *
 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  data           - A plain data byte. 
 * @param[in]  pec            - The Packet Error Code for packet error checking
 *                              mechanism. If the slave device supports to append
 *                              PEC byte, please set this to true.
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_send_byte() returns 0;
 *         on error, it returns a nonzero value.
 *
 * @code Example code:
 *
 *  // Access device 14-0050 using Send Byte protocol, with data 0x72 
 *  api_smbus_send_byte (14, 0x50, 0x72, false, 0, 0);
 *
 * @endcode
 */
int api_smbus_send_byte(uint8_t bus, uint8_t addr, uint8_t data, bool pec,
                        int retry, int retry_delay_ms);

/**
 * @Name api_smbus_receive_byte()
 *
 * @Description This function do SMBus Receive Byte protocol transaction.
 *              Reads a single byte from a device, without specifying a 
 *              device command code or register.
 * \verbatim
   Receive Byte Protocol:

     +---+------+----+-----+--------+----+---+
     | S | Addr | Rd | [A] | [Data] | NA | P |
     +---+------+----+-----+--------+----+---+

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *
 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[out] *data          - Pointer to buffer into which data will be read.
 * @param[in]  pec            - The Packet Error Code for packet error checking
 *                              mechanism. If the slave device supports to append
 *                              PEC byte, please set this to true.
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_receive_byte() returns 0;
 *         on error, it returns a nonzero value.
 *
 * @code Example code:
 *
 *  // Access device 12-0054 using Receive Byte protocol
 *  uint8_t data;
 *  api_smbus_receive_byte (12, 0x54, &data, false, 0, 0);
 *
 * @endcode
 */
int api_smbus_receive_byte(uint8_t bus, uint8_t addr, uint8_t* data, bool pec,
                           int retry, int retry_delay_ms);

/**
 * @Name api_smbus_read_byte()
 *
 * @Description This function do SMBus Read Byte protocol transaction.
 *              Reads a single byte from a device, from a designated device
 *              command code or register.
 * \verbatim
   Read Byte Protocol:

     +---+------+----+-----+------+-----+---+------+----+-----+--------+----+---+
     | S | Addr | Wr | [A] | Comm | [A] | S | Addr | Rd | [A] | [Data] | NA | P |
     +---+------+----+-----+------+-----+---+------+----+-----+--------+----+---+

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *
 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  command_code   - The SMBus device command code or register.
 * @param[out] *data          - Pointer to buffer into which data will be read.
 * @param[in]  pec            - The Packet Error Code for packet error checking
 *                              mechanism. If the slave device supports to append
 *                              PEC byte, please set this to true.
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_read_byte() returns 0;
 *         on error, it returns a nonzero value.
 *
 * @code Example code:
 *
 *  // Access device 12-0054 registr 0x01 and read response using Read Byte protocol.
 *  uint8_t data;
 *  api_smbus_read_byte (12, 0x54, 0x01, &data, false, 0, 0);
 *
 * @endcode
 */
int api_smbus_read_byte(uint8_t bus, uint8_t addr, uint8_t command_code,
                        uint8_t* data, bool pec, int retry, int retry_delay_ms);

/**
 * @Name api_smbus_read_word()
 *
 * @Description This function do SMBus Read Word protocol transaction.
 *              Reads a word (16 bits) from a device, from a designated device
 *              command code or register.
 * \verbatim
   Read Word Protocol:

     +---+------+----+-----+------+-----+---+------+----+-----+-----------+---+------------+----+---+
     | S | Addr | Wr | [A] | Comm | [A] | S | Addr | Rd | [A] | [DataLow] | A | [DataHigh] | NA | P |
     +---+------+----+-----+------+-----+---+------+----+-----+-----------+---+------------+----+---+

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *
 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  command_code   - The SMBus device command code or register.
 * @param[out] *data          - Pointer to buffer into which data will be read.
 * @param[in]  pec            - The Packet Error Code for packet error checking
 *                              mechanism. If the slave device supports to append
 *                              PEC byte, please set this to true.
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_read_word() returns 0;
 *         on error, it returns a nonzero value.
 *
 * @code Example code:
 *
 *  // Access device 12-0054 registr 0x01 and read response using Read Word protocol.
 *  uint16_t data;
 *  api_smbus_read_word (12, 0x54, 0x01, &data, false, 0, 0);
 *
 * @endcode
 */
int api_smbus_read_word(uint8_t bus, uint8_t addr, uint8_t command_code,
                        uint16_t* data, bool pec, int retry,
                        int retry_delay_ms);

/**
 * @Name api_smbus_write_byte()
 *
 * @Description This function do SMBus Write Byte protocol transaction.
 *              This writes a single byte to a device, to a designated register.
 * \verbatim
   Write Byte Protocol:

     +---+------+----+-----+------+-----+------+-----+---+
     | S | Addr | Wr | [A] | Comm | [A] | Data | [A] | P |
     +---+------+----+-----+------+-----+------+-----+---+

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *
 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  command_code   - The SMBus device command code or register.
 * @param[in]  data           - A plain data byte. 
 * @param[in]  pec            - The Packet Error Code for packet error checking
 *                              mechanism. If the slave device supports to append
 *                              PEC byte, please set this to true.
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_write_byte() returns 0;
 *         on error, it returns a nonzero value.
 */
int api_smbus_write_byte(uint8_t bus, uint8_t addr, uint8_t command_code,
                         uint8_t data, bool pec, int retry, int retry_delay_ms);

/**
 * @Name api_smbus_write_word()
 *
 * @Description This function do SMBus Write Word protocol transaction.
 *              This writes a word (16 bits) to a device, to a designated
 *              register.
 * \verbatim
   Write Word Protocol:

     +---+------+----+-----+------+-----+---------+-----+----------+-----+---+
     | S | Addr | Wr | [A] | Comm | [A] | DataLow | [A] | DataHigh | [A] | P |
     +---+------+----+-----+------+-----+---------+-----+----------+-----+---+

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *
 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  command_code   - The SMBus device command code or register.
 * @param[in]  data           - A plain data word. 
 * @param[in]  pec            - The Packet Error Code for packet error checking
 *                              mechanism. If the slave device supports to append
 *                              PEC byte, please set this to true.
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_write_word() returns 0;
 *         on error, it returns a nonzero value.
 */
int api_smbus_write_word(uint8_t bus, uint8_t addr, uint8_t command_code,
                         uint16_t data, bool pec, int retry,
                         int retry_delay_ms);

/**
 * @Name api_smbus_process_call()
 *
 * @Description This function do SMBus Process Call protocol transaction.
 *              Selects a device command_code byte, sends 16 bits of data to
 *              it, and reads 16 bits of data in return.
 * \verbatim
   Process Call Protocol:

     +---+------+----+-----+------+-----+---------+-----+----------+-----+---+------+----+-----+-----------+---+------------+----+---+
     | S | Addr | Wr | [A] | Comm | [A] | DataLow | [A] | DataHigh | [A] | S | Addr | Rd | [A] | [DataLow] | A | [DataHigh] | NA | P |
     +---+------+----+-----+------+-----+---------+-----+----------+-----+---+------+----+-----+-----------+---+------------+----+---+

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *
 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  command_code   - The SMBus device command code or register.
 * @param[in]  write_data     - A plain data word, which data will be written
 *                              to slave.
 * @param[out] *read_data     - Pointer to buffer into which data will be read.
 * @param[in]  pec            - The Packet Error Code for packet error checking
 *                              mechanism. If the slave device supports to append
 *                              PEC byte, please set this to true.
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_process_call() returns 0;
 *         on error, it returns a nonzero value.
 */
int api_smbus_process_call(uint8_t bus, uint8_t addr, uint8_t command_code,
                           uint16_t write_data, uint16_t* read_data, bool pec,
                           int retry, int retry_delay_ms);

/**
 * @Name api_smbus_block_write()
 *
 * @Description This function do SMBus Block Write protocol transaction.
 *              This writes a block of up to 32 bytes to a device, from a
 *              designated register that is specified through the Comm byte.
 *              The amount of data is specified by the device in the Count byte.
 * \verbatim
   Block Write Protocol:

     +---+------+----+-----+------+-----+-------+-----+------+-----+------+-----+     +-----+------+-----+---+
     | S | Addr | Wr | [A] | Comm | [A] | Count | [A] | Data | [A] | Data | [A] | ... | [A] | Data | [A] | P |
     +---+------+----+-----+------+-----+-------+-----+------+-----+------+-----+     +-----+------+-----+---+

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *
 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  command_code   - The SMBus device command code or register.
 * @param[in]  write_count    - The count of data bytes to be written.
 * @param[in]  *write_data    - Pointer to buffer from which data will be written
 *                              to slave.
 * @param[in]  pec            - The Packet Error Code for packet error checking
 *                              mechanism. If the slave device supports to append
 *                              PEC byte, please set this to true.
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_block_write() returns 0;
 *         on error, it returns a nonzero value.
 */
int api_smbus_block_write(uint8_t bus, uint8_t addr, uint8_t command_code,
                          uint8_t write_count, uint8_t* write_data, bool pec,
                          int retry, int retry_delay_ms);

/**
 * @Name api_smbus_block_read()
 *
 * @Description This function do SMBus Block Read protocol transaction.
 *              This reads a block of up to 32 bytes to from device, from a
 *              designated register that is specified through the Comm byte.
 *              The amount of data is specified by the device in the Count byte.
 * \verbatim
   Block Read Protocol:

     +---+------+----+-----+------+-----+---+------+----+-----+---------+---+--------+---+--------+---+     +---+--------+----+---+
     | S | Addr | Wr | [A] | Comm | [A] | S | Addr | Rd | [A] | [Count] | A | [Data] | A | [Data] | A | ... | A | [Data] | NA | P |
     +---+------+----+-----+------+-----+---+------+----+-----+---------+---+--------+---+--------+---+     +---+--------+----+---+

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *
 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  command_code   - The SMBus device command code or register.
 * @param[out] *read_count    - Pointer to buffer into which is the data byte 
 *                              containing the length of this block operation.
 * @param[out] *read_data     - Pointer to buffer into which data will be read.
 * @param[in]  pec            - The Packet Error Code for packet error checking
 *                              mechanism. If the slave device supports to append
 *                              PEC byte, please set this to true.
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_block_read() returns 0;
 *         on error, it returns a nonzero value.
 */
int api_smbus_block_read(uint8_t bus, uint8_t addr, uint8_t command_code,
                         uint8_t *read_count, uint8_t* read_data, bool pec,
                         int retry, int retry_delay_ms);

/**
 * @Name api_smbus_block_process_call()
 *
 * @Description This function do SMBus Block Process Call protocol transaction.
 *              This selects a device register (through the command_code byte),
 *              sends 1 to 31 bytes of data to it, and reads 1 to 31 bytes of
 *              data in return
 * \verbatim
   Block Process Call Protocol:

     +---+------+----+-----+------+-----+-------+-----+------+-----+     +---+------+----+-----+---------+---+--------+     +---+---+
     | S | Addr | Wr | [A] | Comm | [A] | Count | [A] | Data | [A] | ... | S | Addr | Rd | [A] | [Count] | A | [Data] | ... | A | P |
     +---+------+----+-----+------+-----+-------+-----+------+-----+     +---+------+----+-----+---------+---+--------+     +---+---+

   =============== =============================================================
   S               - Start condition
   P               - Stop condition
   Rd/Wr (1 bit)   - Read/Write bit. Rd equals 1, Wr equals 0.
   A, NA (1 bit)   - Acknowledge (ACK) and Not Acknowledge (NACK) bit
   Addr  (7 bits)  - I2C 7 bit address. Note that this can be expanded as usual to
                     get a 10 bit I2C address.
   Comm  (8 bits)  - Command byte, a data byte which often selects a register on
                     the device.
   Data  (8 bits)  - A plain data byte. Sometimes, I write DataLow, DataHigh
                     for 16 bit data.
   Count (8 bits)  - A data byte containing the length of a block operation.

   [..]            - Data sent by I2C device, as opposed to data sent by the host
                     adapter.
   =============== =============================================================
   \endverbatim
 *

 * @param[in]  bus            - The SMBus channel on which the slave is connected.
 * @param[in]  addr           - The SMBus address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  command_code   - The SMBus device command code or register.
 * @param[in]  write_count    - The count of data bytes to be written.
 * @param[in]  *write_data    - Pointer to buffer from which data will be written
 *                              to slave.
 * @param[out] *read_count    - Pointer to buffer into which is the data byte 
 *                              containing the length of this block operation.
 * @param[out] *read_data     - Pointer to buffer into which data will be read.
 * @param[in]  pec            - The Packet Error Code for packet error checking
 *                              mechanism. If the slave device supports to append
 *                              PEC byte, please set this to true.
 * @param[in]  retry          - The retry count when SMBus transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return SMBus transaction status.
 *         On success, api_smbus_block_process_call() returns 0;
 *         on error, it returns a nonzero value.
 */
int api_smbus_block_process_call(uint8_t bus, uint8_t addr,
                                 uint8_t command_code, uint8_t write_count,
                                 uint8_t* write_data, uint8_t* read_count,
                                 uint8_t* read_data, bool pec, int retry,
                                 int retry_delay_ms);

