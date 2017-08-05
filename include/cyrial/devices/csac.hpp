#ifndef CYRIAL_DEVICES_CSAC_HPP
#define CYRIAL_DEVICES_CSAC_HPP

#include <string>

#include "../interface.hpp"

namespace cyrial
{

/* @class csac_device
 *
 * This implementation targets the Microsemi/Symmetricom SA.45 CSAC
 *
 * @brief Class to represent a CSAC
 */
class csac_device
{
protected:
  std::shared_ptr<interface> comm;

public:
  /* @brief Constructor for csac device
   *
   * @param A shared_ptr to a communication interface
   */
  csac_device(std::shared_ptr<interface> port)
    : comm(port)
  {
    comm->set_baud(57600);
    comm->set_timeout(100);
  }

  /* @brief Function to get telemetry headers
   *
   * @return std::string Telemetry headers
   */
  std::string telemetry_header()
  {
    return comm->query("!6");
  }

  /* @brief Function to get telemetry data
   *
   * @return std::string Telemetry data in CSV format
   */
  std::string telemetry_data()
  {
    return comm->query("!^");
  }

  /* @brief Function to adjust the absolute operating frequency
   *
   * @param Frequency adjustment value in pp10^15
   * @return Unit response steer
   */
  std::string steer_freq_abs(int value)
  {
    return (value < -20000000 || value > 20000000) ? ""
      : comm->query("!FD" + std::to_string(value));
  }

  /* @brief Function to adjust the relative operating frequency
   *
   * @param Frequency adjustment value in pp10^15
   * @return Unit response steer
   */
  std::string steer_freq_rel(int value)
  {
    return (value < -20000000 || value > 20000000) ? ""
      : comm->query("!FD" + std::to_string(value));
  }

  /* @brief Function to lock the frequency steering value
   *
   * WARNING: hardware lifecycles provide for a finite number of steering lock
   * writes, so this command should be used sparingly
   */
  void STEER_FREQ_LOCK()
  {
    comm->write("!FL");
    comm->eat();
  }

//  /* @brief Function to
//   *
//   * @return std::string
//   */
//  std::string idn()
//  {
//    return comm->query("*IDN?");
//  }
//
//  /* @brief Function to
//   *
//   * @return std::string
//   */
//  std::string idn()
//  {
//    return comm->query("*IDN?");
//  }
//
//  /* @brief Function to
//   *
//   * @return std::string
//   */
//  std::string idn()
//  {
//    return comm->query("*IDN?");
//  }
//
//  /* @brief Function to
//   *
//   * @return std::string
//   */
//  std::string idn()
//  {
//    return comm->query("*IDN?");
//  }
//
//  /* @brief Function to
//   *
//   * @return std::string
//   */
//  std::string idn()
//  {
//    return comm->query("*IDN?");
//  }
};

} // namespace cyrial

#endif // CYRIAL_DEVICES_CSAC_HPP
