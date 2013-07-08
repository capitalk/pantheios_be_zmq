
#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS // Faster compilation

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

/* STLSoft Header files */
#include <stlsoft/stlsoft.h>
#if _STLSOFT_VER < 0x01096bff
# error This file requires STLSoft 1.9.107 or later
#endif /* _STLSOFT_VER */

#include <stlsoft/conversion/char_conversions.hpp>
#include <stlsoft/conversion/integer_to_string.hpp>
#include <stlsoft/memory/auto_destructor.hpp>
#include <stlsoft/memory/malloc_allocator.hpp>
#include <stlsoft/synch/lock_scope.hpp>



#ifdef PLATFORMSTL_HAS_ATOMIC_INTEGER_OPERATIONS
# include <platformstl/synch/spin_mutex.hpp>
#endif /* PLATFORMSTL_HAS_ATOMIC_INTEGER_OPERATIONS */
#ifdef PANTHEIOS_MT
# include <platformstl/synch/thread_mutex.hpp>
#else /* ? PANTHEIOS_MT */
# include <stlsoft/synch/null_mutex.hpp>
#endif /* PANTHEIOS_MT */

#ifdef PANTHEIOS_MT
    typedef platformstl::thread_mutex           mutex_type;
#else /* ? PANTHEIOS_MT */
    typedef stlsoft::null_mutex                 mutex_type;
#endif /* PANTHEIOS_MT */



/*
PANTHEIOS_CALL(int) pantheios_fe_init(
  int     
, void** 
)
{
#ifdef DEBUG
    fprintf(stderr, "pantheios_fe_init()\n");
#endif
    return PANTHEIOS_INIT_RC_SUCCESS; // Successful initialisation
}

PANTHEIOS_CALL(void) pantheios_fe_uninit(void*)
{
#ifdef DEBUG
    fprintf(stderr, "pantheios_fe_UNinit()\n");
#endif
}

PANTHEIOS_CALL(char const*) pantheios_fe_getProcessIdentity(void*)
{
  return "pantheios_zmq_be.cpp";
}

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(
  void* 
, int severity
, int 
)
{
    using namespace std;
    // log all levels
    return 1;
}

*/


class be_zmq_context
{
    public:
        be_zmq_context() {
            ctx = NULL;
            addr = "";
            error_sock = NULL;
        }

        ~be_zmq_context() {
#ifdef DEBUG
            fprintf(stderr, "~be_zmq_context()\n");
#endif
            if (error_sock) {
                error_sock->close();
                delete error_sock;
            }
        }

        zmq::context_t* ctx;
        std::string addr;
        zmq::socket_t* error_sock;

        mutex_type m_mx;

};


class zmq_param
{
    public:
        zmq_param(zmq::context_t* ctx, 
                std::string& addr) {
            this->ctx = ctx;
            this->addr = addr;
        }

        zmq::context_t* ctx;
        std::string addr;
};

static be_zmq_context* s_zctx = NULL;

PANTHEIOS_CALL(int) pantheios_be_zmq_init(
  char const* processIdentity
, int         backEndId
, void*       wtf
, void*       reserved 
, void**      ptoken 
)
{
  using namespace std;
#ifdef DEBUG
  fprintf(stderr, "pantheios_be_zmq_init()\n");
#endif
  try
  {
    s_zctx = new be_zmq_context();
    *ptoken = static_cast<void*>(s_zctx);
  }
  catch(std::bad_alloc&)
  {
    pantheios::util::onBailOut(PANTHEIOS_SEV_ALERT, "failed to initialise back-end", processIdentity, "out of memory");
    return PANTHEIOS_INIT_RC_OUT_OF_MEMORY;
  }
  catch(std::exception& x)
  {
    pantheios::util::onBailOut(PANTHEIOS_SEV_ALERT, "failed to initialise back-end", processIdentity, x.what());

    return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
  }

  return PANTHEIOS_INIT_RC_SUCCESS; // Successful initialisation
}




int
pantheios_be_zmq_set_params(zmq::context_t* ctx, const char* addr)
{
#ifdef DEBUG
    fprintf(stderr, "pantheios_be_set_params()\n");
#endif
    stlsoft::lock_scope<mutex_type> lock(s_zctx->m_mx);
    if (NULL == ctx) {
        return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
    }    
    assert(ctx);
    int zero = 0;
    s_zctx->ctx = ctx;
    s_zctx->addr = addr;
    s_zctx->error_sock = new zmq::socket_t(*ctx, ZMQ_PUB);
    assert(s_zctx->error_sock);
    s_zctx->error_sock->bind(addr);
    s_zctx->error_sock->setsockopt(ZMQ_LINGER, &zero, sizeof(zero));

    return PANTHEIOS_INIT_RC_SUCCESS;
}

PANTHEIOS_CALL(void) pantheios_be_zmq_uninit(void* /* token */)
{
#ifdef DEBUG
    fprintf(stderr, "pantheios_be_zmq_UNinit()\n");
#endif
    s_zctx->error_sock->close();
    if (s_zctx != NULL) delete s_zctx;
}


PANTHEIOS_CALL(int) pantheios_be_zmq_logEntry(
  void*       feToken
, void*       beToken 
, int         severity
, char const* entry
, size_t      cchEntry
)
{
    using namespace std;

    be_zmq_context* zctx = static_cast<be_zmq_context*>(beToken);
#ifdef DEBUG
    assert(zctx);
    assert(zctx->error_sock);
#endif
    if (!zctx || !zctx->error_sock) return 0;
    stlsoft::lock_scope<mutex_type> lock(zctx->m_mx);

    zmq::message_t level_msg(20);
    memset(level_msg.data(), 0, 20);
    switch(severity)
    {
    case  PANTHEIOS_SEV_EMERGENCY:
        snprintf((char*)level_msg.data(), 20, "%s", "EMERGENCY"); 
        break;
    case  PANTHEIOS_SEV_ALERT:
        snprintf((char*)level_msg.data(), 20, "%s", "ALERT"); 
        break;
    case  PANTHEIOS_SEV_CRITICAL:
        snprintf((char*)level_msg.data(), 20, "%s", "CRITICAL"); 
        break;
    case  PANTHEIOS_SEV_ERROR:
        snprintf((char*)level_msg.data(), 20, "%s", "ERROR"); 
        break;
    case  PANTHEIOS_SEV_WARNING:
        snprintf((char*)level_msg.data(), 20, "%s", "WARNING"); 
        break;
    case  PANTHEIOS_SEV_NOTICE:
        snprintf((char*)level_msg.data(), 20, "%s", "NOTICE"); 
        break;
    case  PANTHEIOS_SEV_INFORMATIONAL:
        snprintf((char*)level_msg.data(), 20, "%s", "INFORMATIONAL"); 
        break;
    case  PANTHEIOS_SEV_DEBUG:
        snprintf((char*)level_msg.data(), 20, "%s", "DEBUG"); 
        break;
    default:
        snprintf((char*)level_msg.data(), 20, "%s", "XXX"); 
        break;
    }

    zctx->error_sock->send(level_msg, ZMQ_SNDMORE);
    zmq::message_t error_txt_msg(cchEntry); 
    memcpy(error_txt_msg.data(), entry, cchEntry);
    zctx->error_sock->send(error_txt_msg, 0);
    return cchEntry;
}



