#ifndef CYRIAL_MANAGER_HPP
#define CYRIAL_MANAGER_HPP

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include <Python.h>

#include "interface.hpp"

namespace cyrial
{

/* @class manager
 * @brief Class to represent a communication interface
 *
 * Several objects exist in the Python interpreter space as an artifact of not
 * being able to successfully call resource methods otherwise. If possible it
 * would be more consistent to have everything exist as a PyObject and to use
 * the PyObject_* interfaces for all operations.
 */
class manager
{
  bool finalize;

  PyObject* py_main;
  PyObject* py_context;

  PyObject* py_resource_manager;
  PyObject* py_device_list;

  std::vector<std::shared_ptr<interface>> port;

public:
  manager()
  {
    Py_Initialize();
    finalize = true;

    py_main = PyImport_AddModule("__main__");

    if (py_main == NULL)
      throw std::runtime_error("Python failed to get main module");

    py_context = PyModule_GetDict(py_main);

    if (py_context == NULL)
      throw std::runtime_error("Python failed to get context");

    PyObject* py_sys_module    = PyImport_ImportModule("sys"   );
    PyObject* py_serial_module = PyImport_ImportModule("serial");
    PyObject* py_visa_module   = PyImport_ImportModule("visa"  );

    if (py_sys_module == NULL || py_serial_module == NULL
        || py_visa_module == NULL)
      throw std::runtime_error("Python failed to load required module");

    PyModule_AddObject(py_main, "sys"   , py_sys_module   );
    PyModule_AddObject(py_main, "serial", py_serial_module);
    PyModule_AddObject(py_main, "visa"  , py_visa_module  );

    PyObject* py_system_path = PySys_GetObject((char*)"path");
    PyObject* py_here = PyUnicode_FromString(".");
    PyList_Append(py_system_path, py_here);
    Py_DECREF(py_here);

    if (PyObject_HasAttrString(py_main, "c_dev"))
      throw std::runtime_error("Python var 'c_dev' clobbered");

    PyRun_SimpleString("c_dev = []");
    py_device_list = PyObject_GetAttrString(py_main, "c_dev");

    if (PyObject_HasAttrString(py_main, "c_rm"))
      throw std::runtime_error("Python var 'c_rm' clobbered");

    PyRun_SimpleString("c_rm = visa.ResourceManager('@py')");
    py_resource_manager = PyObject_GetAttrString(py_main, "c_rm");

    PyObject* py_rm_list_res = PyObject_GetAttrString(py_resource_manager,
                                                      "list_resources");

    PyObject* py_available_list = PyObject_CallFunction(py_rm_list_res, NULL);
    Py_ssize_t py_available_devices = PyTuple_Size(py_available_list);

    if (py_available_devices == 0)
      throw std::runtime_error("No connected devices found");

    for (Py_ssize_t i = 0; i < py_available_devices; ++i)
    {
      std::string command =
        "c_dev.append(c_rm.open_resource(c_rm.list_resources()["
          + std::to_string(i) + "]))";
      PyRun_SimpleString(command.c_str());

      PyObject* py_device = PyList_GetItem(py_device_list, i);

      port.push_back(std::make_shared<interface>(i, py_device, py_context,
                                                                      py_main));
    }
  }

  manager(PyObject* py_mn, PyObject* py_cxt=NULL)
    : py_main(py_mn), py_context(py_cxt)
  {
    finalize = false;

    if (!py_cxt)
      py_context = PyModule_GetDict(py_main);

    if (py_context == NULL)
      throw std::runtime_error("Python failed to get context");

    PyObject* py_sys_module    = PyImport_ImportModule("sys"   );
    PyObject* py_serial_module = PyImport_ImportModule("serial");
    PyObject* py_visa_module   = PyImport_ImportModule("visa"  );

    if (py_sys_module == NULL || py_serial_module == NULL
        || py_visa_module == NULL)
      throw std::runtime_error("Python failed to load required module");

    PyModule_AddObject(py_main, "sys"   , py_sys_module   );
    PyModule_AddObject(py_main, "serial", py_serial_module);
    PyModule_AddObject(py_main, "visa"  , py_visa_module  );

    PyObject* py_system_path = PySys_GetObject((char*)"path");
    PyObject* py_here = PyUnicode_FromString(".");
    PyList_Append(py_system_path, py_here);
    Py_DECREF(py_here);

    if (PyObject_HasAttrString(py_main, "c_dev"))
      throw std::runtime_error("Python var 'c_dev' clobbered");

    PyRun_SimpleString("c_dev = []");
    py_device_list = PyObject_GetAttrString(py_main, "c_dev");

    if (PyObject_HasAttrString(py_main, "c_rm"))
      throw std::runtime_error("Python var 'c_rm' clobbered");

    PyRun_SimpleString("c_rm = visa.ResourceManager('@py')");
    py_resource_manager = PyObject_GetAttrString(py_main, "c_rm");

    PyObject* py_rm_list_res = PyObject_GetAttrString(py_resource_manager,
                                                      "list_resources");

    PyObject* py_available_list = PyObject_CallFunction(py_rm_list_res, NULL);
    Py_ssize_t py_available_devices = PyTuple_Size(py_available_list);

    if (py_available_devices == 0)
      throw std::runtime_error("No connected devices found");

    for (Py_ssize_t i = 0; i < py_available_devices; ++i)
    {
      std::string command =
        "c_dev.append(c_rm.open_resource(c_rm.list_resources()["
          + std::to_string(i) + "]))";
      PyRun_SimpleString(command.c_str());

      PyObject* py_device = PyList_GetItem(py_device_list, i);

      port.push_back(std::make_shared<interface>(i, py_device, py_context,
                                                                      py_main));
    }
  }

  /* @brief Function to return the total number of connected devices
   *
   * @return size_t The number of connected devices
   */
  size_t num_dev()
  {
    return port.size();
  }

  /* @brief Function to return a shared_ptr to a connected port
   *
   * @return shared_ptr Pointer to a connected port
   */
  std::shared_ptr<interface> dev(size_t number)
  {
    return port[number];
  }

  ~manager()
  {
    if (finalize)
      Py_Finalize();
  }
};

} // namespace cyrial

#endif // CYRIAL_MANAGER_HPP
