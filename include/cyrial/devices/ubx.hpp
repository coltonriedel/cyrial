#ifndef CYRIAL_DEVICES_UBX_HPP
#define CYRIAL_DEVICES_UBX_HPP

#include <array>
#include <string>

#include "../interface.hpp"

namespace cyrial
{

/* @class ubx_device
 *
 * @brief Class to represent a generic device which supports the UBX (u-blox)
 *        communication protocol
 */
class ubx_device
{
  std::shared_ptr<interface> comm;

  uint8_t s_mu = 0xb5; // μ sync character
  uint8_t s_b  = 0x62; // b sync character

  uint8_t c_mon = 0x0a; // MON message class

  /* @brief Function to compute the Fletcher checksums of an UBX message and
   *        append them to the message before returning
   *
   * @param A vector of bytes (including sync bytes) representing an UBX message
   * @return The UBX message with A and B checksums appended
   */
  std::vector<uint8_t> add_ubx_checksum(std::vector<uint8_t> msg)
  {
    uint8_t check_a = 0;
    uint8_t check_b = 0;

    // Start at 2 to skip sync characters
    for (size_t i = 2; i < msg.size(); ++i)
      check_b += (check_a += msg[i]);

    msg.push_back(check_a);
    msg.push_back(check_b);

    return msg;
  }

  /* @brief Function to convert a vector of bytes representing an UBX message
   *        into a string containing hex escape characters
   *
   * @param A vector of bytes representing the contents of an UBX message
   * @return A string of escaped hex characters representing the message
   */
  std::string escape_ubx_message(std::vector<uint8_t> msg)
  {
    std::ostringstream result;

    for (size_t i = 0; i < msg.size(); ++i)
      result << "\\x"
        << std::setw(2) << std::setfill('0') << std::hex << (int)msg[i];

    return result.str();
  }

public:
  /* @brief Constructor for UBX device
   *
   * @param A shared_ptr to a communication interface
   */
  ubx_device(std::shared_ptr<interface> port)
    : comm(port)
  {
    comm->set_timeout(100);
    comm->set_baud(9600);
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

    return comm->query_raw(escape_ubx_message(add_ubx_checksum(packet)));
  }
};

} // namespace cyrial

#endif // CYRIAL_DEVICES_UBX_HPP
