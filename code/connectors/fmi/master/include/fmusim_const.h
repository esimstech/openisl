/*
 *     Name: fmusim_const.h
 *
 *     Description: Application constants.
 *
 *     Author: T. Roudier
 *     Copyright (c) 2019-2025 E-Sim Solutions Inc
 *
 *     Distributed under the MIT License.
 * 
 *     --------------------------------------------------------------------------
 * 
 *     Permission is hereby granted, free of charge, to any person obtaining a
 *     copy of this software and associated documentation files (the “Software”),
 *     to deal in the Software without restriction, including without limitation
 *     the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *     and/or sell copies of the Software, and to permit persons to whom the
 *     Software is furnished to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included in
 *     all copies or substantial portions of the Software.
 *
 *     THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *     THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *     FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *     DEALINGS IN THE SOFTWARE.
 * 
 *     --------------------------------------------------------------------------
 *
 */

#ifndef _FMUSIM_CONST_H_
#define _FMUSIM_CONST_H_

/*
 *     Constants and macros definition
 */

// Default names
#define DEFAULT_LOG_FILE_NAME	"islmasterfmi.log"

// Paths
// Paths
#if defined(WIN64) || defined(WIN32)
#define PLATFORM_PATH_SEP		"\\"
#else
#define PLATFORM_PATH_SEP		"/"
#endif
#define FMU_BINARIES_PATH		"%1%" PLATFORM_PATH_SEP "binaries" PLATFORM_PATH_SEP
#if defined(WIN64)
#define FMU_LIB_PATH			FMU_BINARIES_PATH "win64" PLATFORM_PATH_SEP "%2%.dll"
#elif defined(WIN32)
#define FMU_LIB_PATH			FMU_BINARIES_PATH "win32" PLATFORM_PATH_SEP "%2%.dll"
#elif defined(LINUX64)
#define FMU_LIB_PATH			FMU_BINARIES_PATH "linux64" PLATFORM_PATH_SEP "%2%.so"
#else
#define FMU_LIB_PATH			FMU_BINARIES_PATH "linux32" PLATFORM_PATH_SEP "%2%.so"
#endif // Platform

#define RESOURCES_LOCATION		"file://%1%/resources"

// File extensions
#define FMU_FILE_EXT			"fmu"

// Semaphore keys
#define SEM_KEY_KILLER			"_isl_sem_kill_fmimastersim_"

// Default Zip command
#define DEFAULT_ZIPCMD			"\"%1%" PLATFORM_PATH_SEP "tools" PLATFORM_PATH_SEP "ldz\" -i \"%2%\" -d \"%3%\""

// Log codes definition

// Error codes
enum {
	ERROR_CMDLINE = 1000,
	//
	ERROR_KILLTHREAD_SENDSTOP,
	ERROR_KILLTHREAD_SEMACQUIRE,
	ERROR_KILLTHREAD_SIMSTOPFAILED,
	//
	ERROR_FMUSIM_RUNFAILED,
	//
	ERROR_MODEL_FILENOTEXIST,
	ERROR_MODEL_DIRNOTCREATED,
	ERROR_MODEL_MODELDESC_NOTEXIST,
	ERROR_MODEL_LOADMODELDESC,
	ERROR_MODEL_PARSEMODELDESC,
	ERROR_MODEL_FAILEDXMLCREATE,
	ERROR_MODEL_XMKFILENOTEXIST,
	ERROR_MODEL_LOADXMLFILE,
	ERROR_MODEL_CHECKMODEL,
	ERROR_MODEL_NOTVALIDFMIVERSION,
	ERROR_MODEL_VARNOTVALID,
	ERROR_MODEL_SAVEFAILED,
	ERROR_MODEL_FMUXML_E1,
	ERROR_MODEL_FMUXML_E2,
	//
	ERROR_SLAVE2_LOGGER,
	ERROR_SLAVE2_SIMFAILED,
	//
	ERROR_RUNTH_NOINSTANCE,
	ERROR_RUNTH_NOTREADY,
	ERROR_RUNTH_STOPTIME,
	ERROR_RUNTH_STEPSIZE,
	ERROR_RUNTH_STEPTOLERANCE,
	ERROR_RUNTH_CREATESESSION,
	ERROR_RUNTH_INITVARS,
	ERROR_RUNTH_ISLCONNECT,
	ERROR_RUNTH_MAPFUNCTIONS,
	ERROR_RUNTH_MF_ERRORMSG,
	ERROR_RUNTH_INSTANTIATE,
	ERROR_RUNTH_INST_ERRORMSG,
	ERROR_RUNTH_INITIALIZE,
	ERROR_RUNTH_INIT_ERRORMSG,
	ERROR_RUNTH_ENDINITIALIZE,
	ERROR_RUNTH_ENDINIT_ERRORMSG,
	ERROR_RUNTH_FMUINITVARS,
	ERROR_RUNTH_DOSTEP,
	ERROR_RUNTH_DOSTEP_ERRORMSG,
	ERROR_RUNTH_VARISLTOFMU,
	ERROR_RUNTH_VARFMUTOISL,
	ERROR_RUNTH_ISLDISCONNECT,
	ERROR_RUNTH_SENDSTOPSESSION,
	ERROR_RUNTH_FMUTERMINATE,
	ERROR_RUNTH_TERMINATE_ERRORMSG,
	ERROR_RUNTH_FMUFREE,
	ERROR_RUNTH_FREEFMU_ERRORMSG,
	ERROR_RUNTH_FREELIB
};

// Warning codes
enum {
	WARNING_KILLTHREAD_SEMDELETE = 1300,
	WARNING_KILLTHREAD_STOPRECEIVED,
	WARNING_KILLTHREAD_SIMRUNNING,
	//
	WARNING_FMUSIM_SIMRUNNING,
	//
	WARNING_MODEL_TMPDIRNOTREMOVED,
	WARNING_MODEL_MODELLOADED,
	WARNING_MODEL_SAVENOMODEL,
	WARNING_MODEL_NOVARFOUND,
	//
	WARNING_SLAVE2_LOGGER,
	//
	WARNING_RUNTH_STARTTIME,
	WARNING_RUNTH_CLOSEISL,
	WARNING_RUNTH_CLOSEFMU,
	WARNING_RUNTH_LISTENEXITSESSION,
	WARNING_RUNTH_FAILED_FREELIB
};

// Info codes
enum {
	INFO_HELPMSG = 1500,
	INFO_VERSION,
	//
	INFO_MODEL_UNLOADING,
	INFO_MODEL_REMOVETMPDIR,
	INFO_MODEL_LOADING,
	INFO_MODEL_CLEANDIR,
	INFO_MODEL_CREATEDIR,
	INFO_MODEL_RUNNINGZIPCMD,
	INFO_MODEL_UNLOADINGPREVFMU,
	INFO_MODEL_UNLOADINGPREVXML,
	INFO_MODEL_STORAGEMODE,
	INFO_MODEL_LOADED,
	INFO_MODEL_SAVED,
	//
	INFO_MDLVAR_ISSTORED,
	INFO_MDLVAR_INITIALIZEMDL,
	INFO_MDLVAR_INITIALIZEISL,
	//
	INFO_SLAVE_DISCONNECT,
	//
	INFO_SLAVE2_LOGGER,
	//
	INFO_RUNTH_CHDIR,
	INFO_RUNTH_SIMSTARTED,
	INFO_RUNTH_SIMSTOPPED,
	INFO_RUNTH_ISLCONNECT,
	INFO_RUNTH_INITDONE,
	INFO_RUNTH_INSTANTIATED,
	INFO_RUNTH_FMUINITDONE,
	INFO_RUNTH_FMUENDINITDONE,
	INFO_RUNTH_SIMCOMPLETED,
	INFO_RUNTH_CLOSEISL,
	INFO_RUNTH_CLOSINGFMU
};

#endif // _FMUSIM_CONST_H_
