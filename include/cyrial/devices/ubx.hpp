#ifndef CYRIAL_DEVICES_UBX_HPP
#define CYRIAL_DEVICES_UBX_HPP

#include <array>
#include <string>

#include "nmea.hpp"

namespace cyrial
{

/* @class ubx_device
 *
 * @brief Class to represent a generic device which supports the UBX (u-blox)
 *        communication protocol
 */
class ubx_device : public nmea_device
{
  uint8_t s_mu = 0xb5; // μ sync character
  uint8_t s_b  = 0x62; // b sync character

  uint8_t c_mon = 0x0a; // MON message class

  /* @bring Function to compute the checksum (XOR) of NMEA messages
   *
   * @param The message to compute the checksum on, containing both '$' and '*'
   */
  void add_pubx_checksum(std::string &msg)
  {
    uint8_t check = 0x00;
    std::ostringstream checksum;

    // Start at 1 to skip '$', stop at (n - 1) to skip '*'
    for (size_t i = 1; i < msg.length() - 1; ++i)
      check ^= (uint8_t)msg[i];

    checksum << std::setw(2) << std::setfill('0') << std::hex << std::uppercase
      << (int)check;

    msg += checksum.str();
  }

  /* @brief Function to compute the Fletcher checksums of an UBX message and
   *        append them to the message before returning
   *
   * @param A vector of bytes (including sync bytes) representing an UBX message
   */
  void add_ubx_checksum(std::vector<uint8_t> &msg)
  {
    uint8_t check_a = 0;
    uint8_t check_b = 0;

    // Start at 2 to skip sync characters
    for (size_t i = 2; i < msg.size(); ++i)
      check_b += (check_a += msg[i]);

    msg.push_back(check_a);
    msg.push_back(check_b);
  }

  /* @brief Function to convert a vector of bytes representing an UBX message
   *        into a string containing hex escape characters
   *
   * @param A vector of bytes representing the contents of an UBX message
   * @return A string of escaped hex characters representing the message
   */
  std::string escape_ubx_message(std::vector<uint8_t> &msg)
  {
    std::ostringstream result;

    for (size_t i = 0; i < msg.size(); ++i)
      result << "\\x"
        << std::setw(2) << std::setfill('0') << std::hex << (int)msg[i];

    std::cout << result.str() << std::endl;
    return result.str();
  }

public:
  /* @brief Constructor for UBX device
   *
   * @param A shared_ptr to a communication interface
   */
  ubx_device(std::shared_ptr<interface> port)
    : nmea_device(port), base_device(port)
  {
    comm->set_timeout(1000);
    comm->set_baud(9600);
  }

  // PUBX Messages

  /* @brief Function to control the output of NMEA messages on each interface.
   *        0=off, 1=every epoch
   *
   * @param nmea_type A string indicating which message type rate should be
   *        modified
   * @param i2c_rate The rate of message transmission on the i2c interface
   * @param uart_rate The rate of message transmission on the uart interface
   * @param usb_rate The rate of message transmission on the usb interface
   * @param spi_rate The rate of message transmission on the spi interface
   */
  void pubx_rate(std::string nmea_type, size_t i2c_rate=0, size_t uart_rate=0,
      size_t usb_rate=0, size_t spi_rate=0)
  {
    std::string command = "$PUBX,40," + nmea_type + ","
      + std::to_string(i2c_rate) + "," + std::to_string(uart_rate) + ","
      + std::to_string(usb_rate) + "," + std::to_string(spi_rate) + ",0,0*";

    add_pubx_checksum(command);

    comm->write(command);
  }

  // UBX Messages

  /* TODO: Eventually make a general solution

  possibly create a map<map<string>>  so I can index ubx[mon][hw] or something
  and use it as a template parameter where arguments are handled by a variadic

  template <msg_class m_class, msg_id m_id>
  std::string ubx_message(size_t m_length)
  {

  }
  template <msg_class m_class, msg_id m_id, typename Arg>
  std::string ubx_message(size_t m_length, Arg arg)
  {

  }

  template <msg_class m_class, msg_id m_id, typename... Args>
  std::string ubx_message(size_t m_length, Args... args)
  {

  }
  */

  /* UBX-MON
   *
   * Monitoring messages, i.e. communication status, CPU load, stack usage, I/O
   * subsystem statistics.
   */

  /* @brief Function to get the results of the UBX-MON-HW command
   *
   * @return The output of the UBX-MON-HW command
   */
  std::string ubx_mon_hw()
  {
    uint8_t length_a = 0x00;  // UBX_MON_HW passes no parameters
    uint8_t length_b = 0x00;  //    so the length is always 0

    std::vector<uint8_t> packet = { s_mu, s_b, c_mon, 0x09 /* ID */,
                                    length_a, length_b };

    add_ubx_checksum(packet);

    return comm->query_hex(escape_ubx_message(packet));
  }

  /* @brief Function to get the results of the UBX-MON-VER command
   *
   * UBX-MON-VER returns the currently running firmware version, hardware
   * version, and any extensions to the firmware
   *
   * @return The output of the UBX-MON-VER command
   */
  std::string ubx_mon_ver()
  {
    uint8_t length_a = 0x00;  // UBX_MON_VER passes no parameters
    uint8_t length_b = 0x00;  //    so the length is always 0

    std::vector<uint8_t> packet = { s_mu, s_b, c_mon, 0x04 /* ID */,
                                    length_a, length_b };

    add_ubx_checksum(packet);

    return comm->query_raw(escape_ubx_message(packet));
  }
};

} // namespace cyrial

#endif // CYRIAL_DEVICES_UBX_HPP
