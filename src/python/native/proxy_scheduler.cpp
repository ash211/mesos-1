#include <iostream>

#include "proxy_scheduler.hpp"
#include "module.hpp"
#include "mesos_scheduler_driver_impl.hpp"

using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::map;
using namespace mesos;

namespace mesos { namespace python {

string ProxyScheduler::getFrameworkName(SchedulerDriver* driver) {
  InterpreterLock lock;
  PyObject* res = PyObject_CallMethod(impl->pythonScheduler,
                                      (char*) "getFrameworkName",
                                      (char*) "O",
                                      impl);
  if (res == NULL) {
    cerr << "Failed to call scheduler's getFrameworkName" << endl;
    PyErr_Print();
    driver->stop();
    return "";
  }
  if (res == Py_None) {
    cerr << "Scheduler's getFrameworkName returned None" << endl;
    driver->stop();
    return "";
  }
  char* chars = PyString_AsString(res);
  if (chars == NULL) {
    cerr << "Scheduler's getFrameworkName did not return a string" << endl;
    PyErr_Print();
    driver->stop();
    Py_DECREF(res);
    return "";
  }
  string str(chars);
  Py_DECREF(res);
  return str;
};


ExecutorInfo ProxyScheduler::getExecutorInfo(SchedulerDriver* driver) {
  InterpreterLock lock;
  ExecutorInfo info;
  PyObject* res = PyObject_CallMethod(impl->pythonScheduler,
                                      (char*) "getExecutorInfo",
                                      (char*) "O",
                                      impl);
  if (res == NULL) {
    cerr << "Failed to call scheduler's getExecutorInfo" << endl;
    goto cleanup;
  }
  if (res == Py_None) {
    PyErr_Format(PyExc_Exception, "Scheduler's getExecutorInfo returned None");
    goto cleanup;
  }
  if (!readPythonProtobuf(res, &info)) {
    PyErr_Format(PyExc_Exception, "Could not deserialize Python ExecutorInfo");
    goto cleanup;
  }
cleanup:
  if (PyErr_Occurred()) {
    PyErr_Print();
    driver->stop();
  }
  Py_XDECREF(res);
  return info;
};


void ProxyScheduler::registered(SchedulerDriver* driver,
                                const FrameworkID& frameworkId)
{
  InterpreterLock lock;
  
  PyObject* fid = NULL;
  PyObject* res = NULL;
  
  fid = createPythonProtobuf(frameworkId, "FrameworkID");
  if (fid == NULL) {
    goto cleanup; // createPythonProtobuf will have set an exception
  }

  res = PyObject_CallMethod(impl->pythonScheduler,
                            (char*) "registered",
                            (char*) "OO",
                            impl,
                            fid);
  if (res == NULL) {
    cerr << "Failed to call scheduler's registered" << endl;
    goto cleanup;
  }

cleanup:
  if (PyErr_Occurred()) {
    PyErr_Print();
    driver->stop();
  }
  Py_XDECREF(fid);
  Py_XDECREF(res);
}


void ProxyScheduler::resourceOffer(SchedulerDriver* driver,
                                   const OfferID& offerId,
                                   const vector<SlaveOffer>& offers)
{
  InterpreterLock lock;

  PyObject* oid = NULL;
  PyObject* list = NULL;
  PyObject* res = NULL;

  oid = createPythonProtobuf(offerId, "OfferID");
  if (oid == NULL) {
    goto cleanup; // createPythonProtobuf will have set an exception
  }

  list = PyList_New(offers.size());
  if (list == NULL) {
    goto cleanup;
  }
  for (int i = 0; i < offers.size(); i++) {
    PyObject* offer = createPythonProtobuf(offers[i], "SlaveOffer");
    if (offer == NULL) {
      goto cleanup;
    }
    PyList_SetItem(list, i, offer); // Steals the reference to offer
  }

  res = PyObject_CallMethod(impl->pythonScheduler,
                            (char*) "resourceOffer",
                            (char*) "OOO",
                            impl,
                            oid,
                            list);
  if (res == NULL) {
    cerr << "Failed to call scheduler's resourceOffer" << endl;
    goto cleanup;
  }

cleanup:
  if (PyErr_Occurred()) {
    PyErr_Print();
    driver->stop();
  }
  Py_XDECREF(oid);
  Py_XDECREF(list);
  Py_XDECREF(res);
}


void ProxyScheduler::offerRescinded(SchedulerDriver* driver,
                                    const OfferID& offerId)
{
  InterpreterLock lock;
  
  PyObject* oid = NULL;
  PyObject* res = NULL;
  
  oid = createPythonProtobuf(offerId, "OfferID");
  if (oid == NULL) {
    goto cleanup; // createPythonProtobuf will have set an exception
  }

  res = PyObject_CallMethod(impl->pythonScheduler,
                            (char*) "offerRescinded",
                            (char*) "OO",
                            impl,
                            oid);
  if (res == NULL) {
    cerr << "Failed to call scheduler's offerRescinded" << endl;
    goto cleanup;
  }

cleanup:
  if (PyErr_Occurred()) {
    PyErr_Print();
    driver->stop();
  }
  Py_XDECREF(oid);
  Py_XDECREF(res);
}


void ProxyScheduler::statusUpdate(SchedulerDriver* driver,
                                  const TaskStatus& status)
{
  InterpreterLock lock;
  
  PyObject* stat = NULL;
  PyObject* res = NULL;
  
  stat = createPythonProtobuf(status, "TaskStatus");
  if (stat == NULL) {
    goto cleanup; // createPythonProtobuf will have set an exception
  }

  res = PyObject_CallMethod(impl->pythonScheduler,
                            (char*) "statusUpdate",
                            (char*) "OO",
                            impl,
                            stat);
  if (res == NULL) {
    cerr << "Failed to call scheduler's statusUpdate" << endl;
    goto cleanup;
  }

cleanup:
  if (PyErr_Occurred()) {
    PyErr_Print();
    driver->stop();
  }
  Py_XDECREF(stat);
  Py_XDECREF(res);
}


void ProxyScheduler::frameworkMessage(SchedulerDriver* driver,
                                      const FrameworkMessage& message)
{
  InterpreterLock lock;
  
  PyObject* msg = NULL;
  PyObject* res = NULL;
  
  msg = createPythonProtobuf(message, "FrameworkMessage");
  if (msg == NULL) {
    goto cleanup; // createPythonProtobuf will have set an exception
  }

  res = PyObject_CallMethod(impl->pythonScheduler,
                            (char*) "frameworkMessage",
                            (char*) "OO",
                            impl,
                            msg);
  if (res == NULL) {
    cerr << "Failed to call scheduler's frameworkMessage" << endl;
    goto cleanup;
  }

cleanup:
  if (PyErr_Occurred()) {
    PyErr_Print();
    driver->stop();
  }
  Py_XDECREF(msg);
  Py_XDECREF(res);
}


void ProxyScheduler::slaveLost(SchedulerDriver* driver,
                               const SlaveID& slaveId)
{
  InterpreterLock lock;
  
  PyObject* sid = NULL;
  PyObject* res = NULL;
  
  sid = createPythonProtobuf(slaveId, "SlaveID");
  if (sid == NULL) {
    goto cleanup; // createPythonProtobuf will have set an exception
  }

  res = PyObject_CallMethod(impl->pythonScheduler,
                            (char*) "slaveLost",
                            (char*) "OO",
                            impl,
                            sid);
  if (res == NULL) {
    cerr << "Failed to call scheduler's slaveLost" << endl;
    goto cleanup;
  }

cleanup:
  if (PyErr_Occurred()) {
    PyErr_Print();
    driver->stop();
  }
  Py_XDECREF(sid);
  Py_XDECREF(res);
}


void ProxyScheduler::error(SchedulerDriver* driver,
                           int code,
                           const string& message)
{
  InterpreterLock lock;
  PyObject* res = PyObject_CallMethod(impl->pythonScheduler,
                                      (char*) "error",
                                      (char*) "Ois",
                                      impl,
                                      code,
                                      message.c_str());
  if (res == NULL) {
    cerr << "Failed to call scheduler's error" << endl;
    goto cleanup;
  }
cleanup:
  if (PyErr_Occurred()) {
    PyErr_Print();
    // No need for driver.stop(); it should stop itself
  }
  Py_XDECREF(res);
}

}} /* namespace mesos { namespace python { */