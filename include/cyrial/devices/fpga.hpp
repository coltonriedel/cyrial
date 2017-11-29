#ifndef CYRIAL_DEVICES_FPGA_HPP
#define CYRIAL_DEVICES_FPGA_HPP

#include <string>

#include "../interface.hpp"

namespace cyrial
{

/* @class fpga_device
 *
 * @brief Class to represent an FPGA (RTDS proprietary protocol)
 */
class fpga_device
{
protected:
  std::shared_ptr<interface> comm;

public:
  /* @brief Constructor for fpga device
   *
   * @param A shared_ptr to a communication interface
   */
  fpga_device(std::shared_ptr<interface> port)
    : comm(port)
  {
    comm->set_baud(57600);
    comm->set_timeout(100);
  }
};

} // namespace cyrial

#endif // CYRIAL_DEVICES_FPGA_HPP
