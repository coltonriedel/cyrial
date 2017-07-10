#ifndef CYRIAL_SERIAL_DEVICE_HPP
#define CYRIAL_SERIAL_DEVICE_HPP

#include <array>
#include <string>

#include <Python.h>

namespace cyrial
{

std::array<size_t, 31> baud_rates{ 50,      75,      110,     134,     150,
                                   200,     300,     600,     1200,    1800,
                                   2400,    4800,    9600,    19200,   38400,
                                   57600,   115200,  230400,  460800,  500000,
                                   576000,  921600,  1000000, 1152000, 1500000,
                                   2000000, 2500000, 3000000, 3500000, 4000000,
                                   0 };

/* @class serial_device
 *
 * @brief Class to represent a generic device which supports serial
 *        communication
 *
 * Several objects exist in the Python interpreter space as an artifact of not
 * being able to successfully call resource methods otherwise. If possible it
 * would be more consistent to have everything exist as a PyObject and to use
 * the PyObject_* interfaces for all operations.
 */
class serial_device
{
  Py_ssize_t idx;
  Py_ssize_t timeout;
  Py_ssize_t baud_rate;

  std::string location;
  std::string name;

  PyObject* py_device;
  PyObject* py_context;
  PyObject* py_main;

public:
  /* @brief Constructor for serial_device
   *
   * @param i The index of the device in the serial class' storage
   * @param py_dev PyObject pointer to the resource/device
   * @param py_cxt PyObject pointer to the context of the Python Interpreter
   * @param py_mn  PyObject pointer to the main module
   */
  serial_device(Py_ssize_t i, PyObject* py_dev, PyObject* py_cxt,
      PyObject* py_mn)
    : idx(i), py_device(py_dev), py_context(py_cxt), py_main(py_mn)
  {
    // Set default timeout in ms
    PyObject* py_default_timeout = PyInt_FromLong(200);
    PyObject_SetAttrString(py_device, "timeout", py_default_timeout);
    Py_DECREF(py_default_timeout);

    /*
     * This should be moved to SCPI devices
     *
    // Set baud rate
    PyObject* py_default_baud_rate = PyInt_FromLong(115200);
    PyObject_SetAttrString(py_device, "baud_rate", py_default_baud_rate);
    Py_DECREF(py_default_baud_rate);
     */

    /*
     * This is a SCPI specific command and needs to be reorganized to work
     * with UBX protocol devices
     *
    // Get and store the device's common name
    name = query("*IDN?", true);
     */

    // Get and store the interface on which the device is connected
    PyObject* py_dev_loc = PyObject_GetAttrString(py_device, "resource_name");
    location = PyString_AsString(py_dev_loc);
  }

  /* @brief Function to return the index of the device in the serial class'
   *        storage
   *
   * @return The index
   */
  const size_t get_idx()
  {
    return idx;
  }

  /* @brief Function to get device baud rate
   *
   * @return The current setting for baud rate
   */
  size_t get_baud_rate()
  {
    return baud_rate;
  }

  /* @brief Function to set device baud rate
   *
   * @param proposed The proposed new baud rate
   * @return The devices baud rate after attempting to apply proposed change
   */
  size_t set_baud(size_t proposed)
  {
    for (size_t i = 0; proposed != baud_rate && i < baud_rates.size(); ++i)
      if (proposed == baud_rates[i])
      {
        PyObject* py_new_baud_rate = PyInt_FromLong(proposed);
        PyObject_SetAttrString(py_device, "baud_rate", py_new_baud_rate);
        Py_DECREF(py_new_baud_rate);

        baud_rate = proposed;
      }

    return baud_rate;
  }

  /* @brief Function to get device timeout
   *
   * @return The current setting for communication timeout in milliseconds
   */
  size_t get_timeout()
  {
    return timeout;
  }

  /* @brief Function to set device timeout
   *
   * @param t New communication timeout in milliseconds
   */
  void set_timeout(size_t t)
  {
    if (t != timeout)
    {
      PyObject* py_new_timeout = PyInt_FromLong(t);
      PyObject_SetAttrString(py_device, "timeout", py_new_timeout);
      Py_DECREF(py_new_timeout);
    }
  }

  /* @brief Function to write serial commands to device
   *
   * @param cmd Command to send to device
   */
  void write(std::string cmd)
  {
    std::string command = "c_dev[" + std::to_string(idx) + "].write('" + cmd
                                                                         + "')";

    PyRun_SimpleString(command.c_str());
  }

  /* @brief Function to read from serial buffer of device
   *
   * @param discard Boolean indicating whether or not the the first line, which
   *                typically contains the instruction, should be discarded.
   *                Defaults to false.
   * @return String containing raw device buffer contents
   */
  std::string read(bool discard=false)
  {
    std::string temp = "";
    std::string response;
    std::string command = "temp = c_dev[" + std::to_string(idx)
                                                          + "].read().rstrip()";

    PyObject* py_resp = PyRun_String(command.c_str(), Py_single_input,
                                     py_context, py_context);
    PyObject* py_temp = PyObject_GetAttrString(py_main, "temp");

    response = discard ? "" : PyString_AsString(PyObject_Str(py_temp));

    do
    {
      response += response.empty() ? temp : (std::string)"\n" + temp;

      py_resp = PyRun_String(command.c_str(), Py_single_input,
                             py_context, py_context);
      py_temp = PyObject_GetAttrString(py_main, "temp");

      temp = PyString_AsString(PyObject_Str(py_temp));

    } while (!temp.empty());

    return response;
  }

  /* @brief Convenience function to write a serial command and read the result
   *
   * @param cmd Command to send to device
   * @param discard Boolean indicating whether or not the the first line, which
   *                typically contains the instruction, should be discarded.
   *                Defaults to true for queries.
   * @return String containing device buffer contents
   */
  std::string query(std::string command, bool discard=true)
  {
    write(command);

    return read(discard);
  }

  // TODO: implement other encodings and *_raw functions to enable different
  // protocols
};

} // namespace cyrial

#endif // CYRIAL_SERIAL_DEVICE_HPP
