#include "abl/environment.hpp"
#include "abl/exception.hpp"

#include "thirdparty/predef/predef.h"

#if !defined(_WIN32)
#include "abl/mt/fast_mutex.hpp"
#include "abl/mt/guard.hpp"
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/param.h>
#include <cstring>
#include <map> 
#if defined(PREDEF_PLATFORM_BSD)
#include <sys/sysctl.h>
#elif defined(PREDEF_OS_HPUX)
#include <pthread.h>
#endif
#else
#include <windows.h>
#include <sstream>
#include <cstring>
#include <iphlpapi.h> 
#include "abl/buffer.hpp"
#include "abl/string/unicode_converter.hpp"
#pragma comment(lib, "IPHLPAPI.lib")
#endif

#include <cstdlib>
#include <cstdio> // sprintf()


#if !defined(_WIN32)
namespace abl
{
  class environment_impl_c
  {
  public:
    typedef UInt8 node_id_t[6]; /// Ethernet address.

    static std::string getImpl(const std::string& name);	
    static bool hasImpl(const std::string& name);	
    static void setImpl(const std::string& name, const std::string& value);
    static std::string osNameImpl();
    static std::string osVersionImpl();
    static std::string osArchitectureImpl();
    static std::string nodeNameImpl();
    static void nodeIdImpl(node_id_t& id);
    static unsigned processorCountImpl();

  private:
    typedef std::map<std::string, std::string> StringMap;
	
    static StringMap _map;
    static fast_mutex_c _mutex;
  }; 

  environment_impl_c::StringMap environment_impl_c::_map;
  fast_mutex_c environment_impl_c::_mutex;

  std::string environment_impl_c::getImpl(const std::string& name)
  {
    guard_c<fast_mutex_c> lock(_mutex);
	
    const char* val = getenv(name.c_str());
    if (val)
      return std::string(val);
    else
      throw not_found_exception_c(name);
  }


  bool environment_impl_c::hasImpl(const std::string& name)
  {
    
    guard_c<fast_mutex_c> lock(_mutex);
    return getenv(name.c_str()) != 0;
  }


  void environment_impl_c::setImpl(const std::string& name, const std::string& value)
  {
    guard_c<fast_mutex_c> lock(_mutex);
    std::string var = name;
    var.append("=");
    var.append(value);
    _map[name] = var;
    if (putenv((char*) _map[name].c_str()))
      {
	std::string msg = "cannot set environment variable: ";
	msg.append(name);
	throw system_exception_c(msg);
      }
  }


  std::string environment_impl_c::osNameImpl()
  {
    struct utsname uts;
    uname(&uts);
    return uts.sysname;
  }


  std::string environment_impl_c::osVersionImpl()
  {
    struct utsname uts;
    uname(&uts);
    return uts.release;
  }


  std::string environment_impl_c::osArchitectureImpl()
  {
    struct utsname uts;
    uname(&uts);
    return uts.machine;
  }


  std::string environment_impl_c::nodeNameImpl()
  {
    struct utsname uts;
    uname(&uts);
    return uts.nodename;
  }


  unsigned environment_impl_c::processorCountImpl()
  {
#if defined(PREDEF_PLATFORM_BSD)
    unsigned count;
    std::size_t size = sizeof(count);
    if (sysctlbyname("hw.ncpu", &count, &size, 0, 0))
      return 1;
    else
      return count;
#elif defined(PREDEF_OS_HPUX)
    return pthread_num_processors_np();
#elif defined(_SC_NPROCESSORS_ONLN)
    int count = sysconf(_SC_NPROCESSORS_ONLN);
    if (count <= 0) count = 1;
    return static_cast<int>(count);
#else
    return 1;
#endif
  }


} // namespace abl


//
// nodeIdImpl
//
#if defined(PREDEF_PLATFORM_BSD) || defined(PREDEF_OS_QNX)
//
// BSD variants
//
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if_dl.h>


namespace abl 
{


  void environment_impl_c::nodeIdImpl(node_id_t& id)
  {
    struct ifaddrs* ifaphead;
    int rc = getifaddrs(&ifaphead);
    if (rc) throw system_exception_c("cannot get network adapter list");

    bool foundAdapter = false;
    for (struct ifaddrs* ifap = ifaphead; ifap; ifap = ifap->ifa_next) 
      {
	if (ifap->ifa_addr && ifap->ifa_addr->sa_family == AF_LINK) 
	  {
	    struct sockaddr_dl* sdl = reinterpret_cast<struct sockaddr_dl*>(ifap->ifa_addr);
	    caddr_t ap = (caddr_t) (sdl->sdl_data + sdl->sdl_nlen);
	    int alen = sdl->sdl_alen;
	    if (ap && alen > 0) 
	      {
		std::memcpy(&id, ap, sizeof(id));
		foundAdapter = true;
		break;
	      }
	  }
      }
    freeifaddrs(ifaphead);
    if (!foundAdapter) throw system_exception_c("cannot determine MAC address (no suitable network adapter found)");
  }


} // namespace abl


#elif defined(__CYGWIN__) || defined(PREDEF_OS_LINUX)
//
// Linux, Cygwin
//
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>


namespace abl 
{


  void environment_impl_c::nodeIdImpl(node_id_t& id)
  {
    struct ifreq ifr;

    int s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s == -1) throw system_exception_c("cannot open socket");

    std::strcpy(ifr.ifr_name, "eth0");
    int rc = ioctl(s, SIOCGIFHWADDR, &ifr);
    close(s);
    if (rc < 0) throw system_exception_c("cannot get MAC address");
    struct sockaddr* sa = reinterpret_cast<struct sockaddr*>(&ifr.ifr_addr);
    std::memcpy(&id, sa->sa_data, sizeof(id));
  }


} // namespace abl


#elif defined(PREDEF_PLATFORM_UNIX)
//
// General Unix
//
#include <sys/ioctl.h>
#if defined(sun) || defined(__sun)
#include <sys/sockio.h>
#endif
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <unistd.h>


namespace abl {


  void environment_impl_c::nodeIdImpl(node_id_t& id)
  {
    char name[MAXHOSTNAMELEN];
    if (gethostname(name, sizeof(name)))
      throw system_exception_c("cannot get host name");

    struct hostent* pHost = gethostbyname(name);
    if (!pHost) throw system_exception_c("cannot get host IP address");

    int s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == -1) throw system_exception_c("cannot open socket");

    struct arpreq ar;
    std::memset(&ar, 0, sizeof(ar));
    struct sockaddr_in* pAddr = reinterpret_cast<struct sockaddr_in*>(&ar.arp_pa);
    pAddr->sin_family = AF_INET;
    std::memcpy(&pAddr->sin_addr, *pHost->h_addr_list, sizeof(struct in_addr));
    int rc = ioctl(s, SIOCGARP, &ar);
    close(s);
    if (rc < 0) throw system_exception_c("cannot get MAC address");
    std::memcpy(&id, ar.arp_ha.sa_data, sizeof(id));
  }


} // namespace abl


#endif 

#else

namespace abl 
{


  class environment_impl_c
  {
  public:
    typedef UInt8 node_id_t[6]; /// Ethernet address.

    static std::string getImpl(const std::string& name);	
    static bool hasImpl(const std::string& name);	
    static void setImpl(const std::string& name, const std::string& value);
    static std::string osNameImpl();	
    static std::string osVersionImpl();
    static std::string osArchitectureImpl();
    static std::string nodeNameImpl();
    static void nodeIdImpl(node_id_t& id);
    static unsigned processorCountImpl();
  };


} // namespace abl
 
namespace abl {


  std::string environment_impl_c::getImpl(const std::string& name)
  {
    std::wstring uname;
    unicode_converter_c::to_utf16(name, uname);
    DWORD len = GetEnvironmentVariableW(uname.c_str(), 0, 0);
    if (len == 0) throw not_found_exception_c(name);
    buffer_c<wchar_t> buffer(len);
    GetEnvironmentVariableW(uname.c_str(), buffer.begin(), len);
    std::string result;
    unicode_converter_c::to_utf8(buffer.begin(), len - 1, result);
    return result;
  }


  bool environment_impl_c::hasImpl(const std::string& name)
  {
    std::wstring uname;
    unicode_converter_c::to_utf16(name, uname);
    DWORD len = GetEnvironmentVariableW(uname.c_str(), 0, 0);
    return len > 0;
  }


  void environment_impl_c::setImpl(const std::string& name, const std::string& value)
  {
    std::wstring uname;
    std::wstring uvalue;
    unicode_converter_c::to_utf16(name, uname);
    unicode_converter_c::to_utf16(value, uvalue);
    if (SetEnvironmentVariableW(uname.c_str(), uvalue.c_str()) == 0)
      {
	std::string msg = "cannot set environment variable: ";
	msg.append(name);
	throw system_exception_c(msg);
      }
  }


  std::string environment_impl_c::osNameImpl()
  {
    OSVERSIONINFO vi;
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (GetVersionEx(&vi) == 0) throw system_exception_c("Cannot get OS version information");
    switch (vi.dwPlatformId)
      {
      case VER_PLATFORM_WIN32s:
	return "Windows 3.x";
      case VER_PLATFORM_WIN32_WINDOWS:
	return vi.dwMinorVersion == 0 ? "Windows 95" : "Windows 98";
      case VER_PLATFORM_WIN32_NT:
	return "Windows NT";
      default:
	return "Unknown";
      }
  }


  std::string environment_impl_c::osVersionImpl()
  {
    OSVERSIONINFOW vi;
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (GetVersionExW(&vi) == 0) throw system_exception_c("Cannot get OS version information");
    std::ostringstream str;
    str << vi.dwMajorVersion << "." << vi.dwMinorVersion << " (Build " << (vi.dwBuildNumber & 0xFFFF);
    std::string version;
    unicode_converter_c::to_utf8(vi.szCSDVersion, version);
    if (!version.empty()) str << ": " << version;
    str << ")";
    return str.str();
  }


  std::string environment_impl_c::osArchitectureImpl()
  {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    switch (si.wProcessorArchitecture)
      {
      case PROCESSOR_ARCHITECTURE_INTEL:
	return "IA32";
      case PROCESSOR_ARCHITECTURE_MIPS:
	return "MIPS";
      case PROCESSOR_ARCHITECTURE_ALPHA:
	return "ALPHA";
      case PROCESSOR_ARCHITECTURE_PPC:
	return "PPC";
      case PROCESSOR_ARCHITECTURE_IA64:
	return "IA64";
#ifdef PROCESSOR_ARCHITECTURE_IA32_ON_WIN64
      case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
	return "IA64/32";
#endif
#ifdef PROCESSOR_ARCHITECTURE_AMD64
      case PROCESSOR_ARCHITECTURE_AMD64:
	return "AMD64";
#endif
      default:
	return "Unknown";
      }
  }


  std::string environment_impl_c::nodeNameImpl()
  {
    wchar_t name[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    if (GetComputerNameW(name, &size) == 0) throw system_exception_c("Cannot get computer name");
    std::string result;
    unicode_converter_c::to_utf8(name, result);
    return result;
  }


  void environment_impl_c::nodeIdImpl(node_id_t& id)
  {
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = 0;
    ULONG len    = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(new char[len]);
    // Make an initial call to GetAdaptersInfo to get
    // the necessary size into len
    DWORD rc = GetAdaptersInfo(pAdapterInfo, &len);
    if (rc == ERROR_BUFFER_OVERFLOW) 
      {
	delete [] reinterpret_cast<char*>(pAdapterInfo);
	pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(new char[len]);
      }
    else if (rc != ERROR_SUCCESS)
      {
	throw system_exception_c("cannot get network adapter list");
      }
    try
      {
	bool found = false;
	if (GetAdaptersInfo(pAdapterInfo, &len) == NO_ERROR) 
	  {
	    pAdapter = pAdapterInfo;
	    while (pAdapter && !found) 
	      {
		if (pAdapter->Type == MIB_IF_TYPE_ETHERNET && pAdapter->AddressLength == sizeof(id))
		  {
		    std::memcpy(&id, pAdapter->Address, pAdapter->AddressLength);
		    found = true;
		  }
		pAdapter = pAdapter->Next;
	      }
	  }
	else throw system_exception_c("cannot get network adapter list");
	if (!found) throw system_exception_c("no Ethernet adapter found");
      }
    catch (exception_c&)
      {
	delete [] reinterpret_cast<char*>(pAdapterInfo);
	throw;
      }
    delete [] reinterpret_cast<char*>(pAdapterInfo);
  }


  unsigned environment_impl_c::processorCountImpl()
  {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
  }
 
}
#endif

namespace abl 
{

  std::string environment_c::get(const std::string& name)
  {
    return environment_impl_c::getImpl(name);
  }


  std::string environment_c::get(const std::string& name, const std::string& defaultValue)
  {
    if (has(name))
      return get(name);
    else
      return defaultValue;
  }

	
  bool environment_c::has(const std::string& name)
  {
    return environment_impl_c::hasImpl(name);
  }

	
  void environment_c::set(const std::string& name, const std::string& value)
  {
    environment_impl_c::setImpl(name, value);
  }


  std::string environment_c::os_name()
  {
    return environment_impl_c::osNameImpl();
  }

	
  std::string environment_c::os_version()
  {
    return environment_impl_c::osVersionImpl();
  }

	
  std::string environment_c::os_architecture()
  {
    return environment_impl_c::osArchitectureImpl();
  }
	

  std::string environment_c::node_name()
  {
    return environment_impl_c::nodeNameImpl();
  }


  std::string environment_c::node_id()
  {
    node_id_t id;
    node_id(id);
    char result[18];
    std::sprintf(result, "%02x:%02x:%02x:%02x:%02x:%02x",
		 id[0],
		 id[1],
		 id[2],
		 id[3],
		 id[4],
		 id[5]);
    return std::string(result);
  }


  void environment_c::node_id(node_id_t& id)
  {
    return environment_impl_c::nodeIdImpl(id);
  }


  unsigned environment_c::processor_count()
  {
    return environment_impl_c::processorCountImpl();
  }


} 
