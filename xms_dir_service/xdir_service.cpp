#include "xdir_service.h"
#include "xdir_handle.h"
XServiceHandle* XDirService::CreateServiceHandle()
{
    return new XDirHandle();
}

XDirService::XDirService()
{
}


XDirService::~XDirService()
{
}
