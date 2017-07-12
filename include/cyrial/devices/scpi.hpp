#ifndef CYRIAL_DEVICES_SCPI_HPP
#define CYRIAL_DEVICES_SCPI_HPP

#include <string>

#include "../serial_device.hpp"

namespace cyrial
{

/* @class scpi_device
 *
 * @brief Class to represent a generic device which supports the SCPI protocol
 */
class scpi_device : public serial_device
{
public:
  /* @brief Function to retreive identifying information about the device
   *
   * Format:
   *   <manufacturer>, <model>, <serial number>, <firmware>
   *
   * @return std::string Identifying information about the device
   */
  std::string idn()
  {
    return query("*IDN?");
  }
};

} // namespace cyrial

#endif // CYRIAL_DEVICES_SCPI_HPP
