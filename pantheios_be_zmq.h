/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>          // Pantheios C++ main header
#include <pantheios/backend.h>
#include <pantheios/init_codes.h>

/* zmq Header Files */
#include <zmq.hpp>

/* Standard C/C++ Header Files */
#include <exception>                        // for std::exception
#include <new>                              // for std::bad_alloc
#include <string>                           // for std::string
#include <stdlib.h>                         // for exit codes

/*
PANTHEIOS_CALL(int) pantheios_fe_init(
  int    
, void**  
);

PANTHEIOS_CALL(void) pantheios_fe_uninit(void* );

PANTHEIOS_CALL(char const*) pantheios_fe_getProcessIdentity(void*); 

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(
  void* 
, int severity
, int 
);
*/

int pantheios_be_zmq_set_params(zmq::context_t* ctx, const char* addr);

PANTHEIOS_CALL(int) pantheios_be_zmq_init(
  char const* processIdentity
, void*       reserved 
, void**      ptoken 
);


PANTHEIOS_CALL(void) pantheios_be_zmq_uninit(void*);

PANTHEIOS_CALL(int) pantheios_be_zmq_logEntry(
  void*       feToken
, void*       beToken 
, int         severity
, char const* entry
, size_t      cchEntry
);


