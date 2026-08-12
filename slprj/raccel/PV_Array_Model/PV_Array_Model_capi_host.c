#include "PV_Array_Model_capi_host.h"
static PV_Array_Model_host_DataMapInfo_T root;
static int initialized = 0;
__declspec( dllexport ) rtwCAPI_ModelMappingInfo *getRootMappingInfo()
{
    if (initialized == 0) {
        initialized = 1;
        PV_Array_Model_host_InitializeDataMapInfo(&(root), "PV_Array_Model");
    }
    return &root.mmi;
}

rtwCAPI_ModelMappingInfo *mexFunction() {return(getRootMappingInfo());}
