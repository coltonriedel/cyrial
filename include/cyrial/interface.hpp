#ifndef CYRIAL_INTERFACE_HPP
#define CYRIAL_INTERFACE_HPP

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

/* @class interface
 *
 * @brief Class to represent a interface which supports serial
 *        communication
 *
 * Several objects exist in the Python interpreter space as an artifact of not
 * being able to successfully call resource methods otherwise. If possible it
 * would be more consistent to have everything exist as a PyObject and to use
 * the PyObject_* interfaces for all operations.
 */
class interface
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
  /* @brief Constructor for interface
   *
   * @param i The index of the device in the manager class' storage
   * @param py_dev PyObject pointer to the resource/device
   * @param py_cxt PyObject pointer to the context of the Python Interpreter
   * @param py_mn  PyObject pointer to the main module
   */
  interface(Py_ssize_t i, PyObject* py_dev, PyObject* py_cxt,
      PyObject* py_mn)
    : idx(i), py_device(py_dev), py_context(py_cxt), py_main(py_mn)
  {
    // Set default timeout in ms
    PyObject* py_default_timeout = PyInt_FromLong(200);
    PyObject_SetAttrString(py_device, "timeout", py_default_timeout);
    Py_DECREF(py_default_timeout);

    // Get and store the interface on which the device is connected
    PyObject* py_dev_loc = PyObject_GetAttrString(py_device, "resource_name");
    location = PyString_AsString(py_dev_loc);
  }

  /* @brief Function to return the index of the device in the manager class'
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

  /* @brief Function to write raw data to device
   *
   * @param data Data to send to device
   */
  void write_raw(std::string data)
  {
    std::string command = "c_dev[" + std::to_string(idx) + "].write_raw('"
      + data + "')";

    PyRun_SimpleString(command.c_str());
  }

  /* @brief Function to write commands to device
   *
   * @param cmd Command to send to device
   */
  void write(std::string cmd)
  {
    std::string command = "c_dev[" + std::to_string(idx) + "].write('" + cmd
                                                                         + "')";

    PyRun_SimpleString(command.c_str());
  }

  /* @brief Function to read raw data from buffer of device
   *
   * @return String containing raw device buffer contents
   */
  std::string read_raw()
  {
    std::string temp = "";
    std::string response;
    std::string command = "temp = repr(c_dev[" + std::to_string(idx)
                                                        + "].read_raw())[1:-1]";

    PyObject* py_resp = PyRun_String(command.c_str(), Py_single_input,
                                     py_context, py_context);
    PyObject* py_temp = PyObject_GetAttrString(py_main, "temp");

    response = PyString_AsString(PyObject_Str(py_temp));

    do
    {
      response += temp;

      py_resp = PyRun_String(command.c_str(), Py_single_input,
                             py_context, py_context);
      py_temp = PyObject_GetAttrString(py_main, "temp");

      temp = PyString_AsString(PyObject_Str(py_temp));

    } while (!temp.empty());

    return response;
  }

  /* @brief Function to read from buffer of device
   *
   * @return String containing raw device buffer contents
   */
  std::string read()
  {
    std::string temp = "";
    std::string response;
    std::string command = "temp = c_dev[" + std::to_string(idx)
                                                          + "].read().rstrip()";

    PyObject* py_resp = PyRun_String(command.c_str(), Py_single_input,
                                     py_context, py_context);
    PyObject* py_temp = PyObject_GetAttrString(py_main, "temp");

    response = PyString_AsString(PyObject_Str(py_temp));

    do
    {
      response += (std::string)"\n" + temp;

      py_resp = PyRun_String(command.c_str(), Py_single_input,
                             py_context, py_context);
      py_temp = PyObject_GetAttrString(py_main, "temp");

      temp = PyString_AsString(PyObject_Str(py_temp));

    } while (!temp.empty());

    return response;
  }

  /* @brief Convenience function to write a command and read the result without
   *        attempting to decode
   *
   * @param cmd Command to send to device
   * @return String containing raw device buffer contents
   */
  std::string query_raw(std::string command)
  {
    write_raw(command);

    return read_raw();
  }

  /* @brief Convenience function to write a command and read the result
   *
   * @param cmd Command to send to device
   * @return String containing device buffer contents
   */
  std::string query(std::string command)
  {
    write(command);

    return read();
  }

  /* @brief Function to eat lines off the buffer of the device, useful when
   *        issuing commands which will be echoed but do not produce a response
   *
   * TODO: could be much more intelligent
   *
   * @param lines The number of lines to eat, defaults to 2
   */
  void eat(size_t lines=2)
  {
    std::string command = "temp = c_dev[" + std::to_string(idx) + "].read()";

    for (size_t i = 0; i < lines; ++i)
      PyRun_SimpleString(command.c_str());
  }

};

} // namespace cyrial

#endif // CYRIAL_INTERFACE_HPP
