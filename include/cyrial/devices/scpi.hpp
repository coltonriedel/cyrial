#ifndef CYRIAL_DEVICES_SCPI_HPP
#define CYRIAL_DEVICES_SCPI_HPP

#include <string>

#include "base.hpp"

namespace cyrial
{

/* @class scpi_device
 *
 * @brief Class to represent a generic device which supports the SCPI protocol
 */
class scpi_device : virtual public base_device
{
public:
  /* @brief Constructor for scpi device
   *
   * @param A shared_ptr to a communication interface
   */
  scpi_device(std::shared_ptr<interface> port)
    : base_device(port)
  { }

  /* @brief Function to retreive identifying information about the device
   *
   * Format:
   *   <manufacturer>, <model>, <serial number>, <firmware>
   *
   * @return std::string Identifying information about the device
   */
  std::string idn()
  {
    return comm->query("*IDN?");
  }
};

} // namespace cyrial

#endif // CYRIAL_DEVICES_SCPI_HPP
