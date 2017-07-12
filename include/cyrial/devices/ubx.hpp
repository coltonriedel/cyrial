#ifndef CYRIAL_DEVICES_UBX_HPP
#define CYRIAL_DEVICES_UBX_HPP

#include <array>
#include <string>

#include "../serial_device.hpp"

namespace cyrial
{

/* @class ubx_device
 *
 * @brief Class to represent a generic device which supports the UBX (u-blox)
 *        communication protocol
 */
class ubx_device
{
  size_t calculate_checksum(const char* msg)
  {
    size_t cksm = 0;

    for (size_t i = 0; msg[i]; ++i)
      cksm ^= (unsigned char)msg[i];

    return cksm;
  }

  std::string encode_message(std::string msg)
  {
    // convert to hex representation
    return "";
  }

  std::string build_message(std::string msg)
  {
    return { "$" + encode_message(msg) + (std::string)"*"
                                             + calculate_checksum(msg.c_str()) }

public:
  // UBX functions
};

} // namespace cyrial

#endif // CYRIAL_DEVICES_UBX_HPP
