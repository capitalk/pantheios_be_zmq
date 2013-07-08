#include "pantheios_be_zmq.h"

// Backends
#include <pantheios/backends/bec.file.h>
#include <pantheios/backends/bec.fprintf.h>
#include <pantheios/backends/be.N.h>

// Frontends
#include <pantheios/frontends/fe.N.h>

const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = "pantheios_be_zmq_test_harness";

pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[] =
{
    {1, PANTHEIOS_SEV_DEBUG},
    {2, PANTHEIOS_SEV_DEBUG},
    {3, PANTHEIOS_SEV_ERROR},
    {0, PANTHEIOS_SEV_DEBUG}
};

pan_be_N_t PAN_BE_N_BACKEND_LIST[] =
{
    PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_fprintf, 0),
    PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_file, PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID),
    PANTHEIOS_BE_N_STDFORM_ENTRY(3, pantheios_be_zmq, 0),
    PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

int main()
{
  try
  {
    using namespace std;

    zmq::context_t ctx(1);
    pantheios_be_zmq_set_params(&ctx, "tcp://127.0.0.1:5556");
    pantheios_be_file_setFilePath("test_pantheios_file_log.out");
    sleep(1);

    // A normal log statement
    pantheios::log_DEBUG("debug stmt");
    pantheios::log_INFORMATIONAL("informational stmt");
    pantheios::log_NOTICE("notice stmt");
    pantheios::log_WARNING("warning stmt");
    pantheios::log_ERROR("error stmt");
    pantheios::log_CRITICAL("critical stmt");
    pantheios::log_ALERT("alert stmt");
    pantheios::log_EMERGENCY("emergency stmt");


    pantheios::log_INFORMATIONAL("stmt#2");

    pantheios::log_INFORMATIONAL("stmt#3");

    return EXIT_SUCCESS;
  }
  catch(std::bad_alloc&)
  {
    pantheios::log(pantheios::alert, "out of memory");
  }
  catch(std::exception& x)
  {
    pantheios::log_CRITICAL("Exception: ", x);
  }
  catch(...)
  {
    pantheios::logputs(pantheios::emergency, "Unexpected unknown error");
  }

  return EXIT_FAILURE;
}


