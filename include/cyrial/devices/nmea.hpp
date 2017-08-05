#ifndef CYRIAL_DEVICES_NMEA_HPP
#define CYRIAL_DEVICES_NMEA_HPP

#include <string>

#include "base.hpp"

namespace cyrial
{

/* @class nmea_device
 *
 * @brief Class to represent a generic device which supports sending NMEA
 *        messages
 */
class nmea_device : virtual public base_device
{
protected:
  std::vector<std::string> messages;

  std::string check_NMEA(std::string input)
  {
    std::string result;

    if (input[0] == '$')
    {
      messages.push_back(input);

      result = comm->read();

      // Idea is to continue reading until we receive a non-NMEA message
      // It may be required that we split strings on newlines, because I'm not
      // certain if there will be a situation where the sought reply will be
      // at the tail of the string
      while (result[0] == '$')
      {
        messages.push_back(input);

        result = comm->read();
      }
    }
    else
      return input;
  }

public:
  /* @brief Constructor for nmea device
   *
   * @param A shared_ptr to a communication interface
   */
  nmea_device(std::shared_ptr<interface> port)
    : base_device(port)
  { }

  std::string get_NMEA()
  {
    std::string result = "";

    for (auto msg : messages)
      result += msg;

    messages.clear();

    return result;
  }
};

} // namespace cyrial

#endif // CYRIAL_DEVICES_NMEA_HPP
