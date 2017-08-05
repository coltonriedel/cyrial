#ifndef CYRIAL_DEVICES_BASE_HPP
#define CYRIAL_DEVICES_BASE_HPP

#include <string>

#include "../interface.hpp"

namespace cyrial
{

/* @class base_device
 *
 * @brief Class to represent a generic device
 */
class base_device
{
protected:
  std::shared_ptr<interface> comm;

public:
  /*
   * @brief Default constructor for base device
   */
  base_device() { }

  /* @brief Constructor for base device
   *
   * @param A shared_ptr to a communication interface
   */
  base_device(std::shared_ptr<interface> port)
    : comm(port)
  { }
};

} // namespace cyrial

#endif // CYRIAL_DEVICES_BASE_HPP
