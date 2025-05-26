/* SPDX-FileCopyrightText: 2013-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ANS1553.c
 *
 * This is the main file for the ANS1553
 * network server
 */


#include "Ai_Options.h"
#include "mil/ANS1553_CommonSettings.h"
#include "mil/ANS1553_ProtocolCommands.h"
#include "ANS_Server.h"
#include "ANS_Client.h"
#include "ANS_Startup.h"
#include "Ai_cdef.h"
#include "generated/version.h"
#include "Ai_Opthelp.h"
#include "Ai_Opterror.h"
#include "ANS_Log.h"
#include "BoardCommandHandlers.h"
#include "BoardProtocolHandlers.h"
#include "ANS_Protocol.h"
#include "Api1553.h"
#include "Ai_NotUsed.h"


#define ANS1553_SERVICE_NAME "ANS1553"


/*!
* ANS1553 server port number.
*/
static  unsigned int g_Ans1553Port = ANS1553_DEFAULT_PORT;

/*!
* ANS1553 client port number.
*/
static  unsigned int g_Ans1553ClientPort = ANS1553_DEFAULT_CLIENT_PORT;

/*! ANS1553_DEFAULT_EXEC_MODE
 * default execution mode for the ANS1553 Server. \n
 * By default the ANS1553 is not installed as Service.
 */
static AiBoolean g_run_service = FALSE;



/*! Information about the server application \n
 * This information is transmitted with the 'GetServerInfo' command.
 */
static struct AnsServerInfo g_Ans1553ServerInfo = {
        /* .applicationName = */ "ANS1553",
        /* .description     = */ "AIM Network Server for MIL-STD-1553 / STANAG3910 protocol"
};


/*! Version of the ANS1553 Server \n
    This version is transmitted with the 'GetServerInfo' command.
 */
static struct AnsServerVersion g_Ans1553ServerVersion = {
        /* be careful with order of initialisation */
        /* .majorVersion  = */ ANS1553_MAJOR_VERSION,
        /* .minorVersion  = */ ANS1553_MINOR_VERSION,
        /* .patchVersion  = */ ANS1553_PATCH_VERSION,
        /* .versionString = */ ANS1553_VERSION_STRING_FULL
};


/*! Handler functions for board related commands */
static struct AnsBoardCommandHandlers g_Ans1553BoardCommandHandlers = {
        /* be careful with order of initialisation */
        /* .open                = */ handleOpenCommand,
        /* .close               = */ handleCloseCommand,
        /* .readMemory          = */ handleReadMemCommand,
        /* .writeMemory         = */ handleWriteMemCommand,
        /* .ioCommand           = */ handleIoCommand,
        /* .ioCommandStruct     = */ handleIoCommandStruct,
        /* .getDriverVersion    = */ handleGetDriverVersionCommand,
        /* .getSoftwareVersion  = */ handleReadVersionCommand,
        /* .getDriverInfo       = */ handleGetDriverInfoCommand,
        /* .getBoardMemSize     = */ NULL,
        /* .openBoardEventStream  */ NULL,
        /* .closeBoardEventStream */ NULL,
        /* .registerCallback      */ handleRegisterCallbackCommand,
        /*  unregisterCallback    */ handleUnregisterCallbackCommand,
        /* .protocolHandler     = */ handleProtocolCommand
};


/*! \brief Sets flag to run application as a service */
static OPTIONS activate_service(OPTIONS_VAR *variable,  
                                void        *userdefined,
                                char        **argv,     
                                int         *nprocessed);




static  OPTIONS     postUsage (
    OPTIONS_VAR     *variable,
    void            *userdefined);

static  OPTIONS     postVersion (
    OPTIONS_VAR     *variable,
    void            *userdefined);

 static OPTIONS     postSetDebugMode (
    OPTIONS_VAR     *variable,
    void            *userdefined);

 static OPTIONS     postSetInfoMode (
    OPTIONS_VAR     *variable,
    void            *userdefined);

 static void performSanityCheck (
     char const     *pTypeName,
     size_t const   typeSize,
     size_t const   expected);


 static AnsBool     parseArguments (
      int         argc,       /*!< [in] number of arguments               */
      char        *argv[]);    /*!< [in] argument vector                   */


/*! \brief Performs 1553 specific initialization of the ANS
 *
 * @return returns AnsTrue on success, AnsFalse otherwise
 */
static AnsBool Ans1553Init(void);





static OPTIONS_TABLE    g_options[] =
{
    /* Usage help message                   */
    {
        OPTHELP_DEFINE_DOUBLE_VERBOSE("help"),
        {   OPTIONS_NOT_REQUIRED,   OPTIONS_NO_ARGUMENTS,   },
        OPTHELP_DEFINE_FUNC_NONE(),
        OPTHELP_DEFINE_PRIOR_NONE(),
        OPTHELP_DEFINE_POST(postUsage),
        OPTHELP_DEFINE_VARIABLE_NONE(),
        OPTHELP_DEFINE_USERDEFINED_NONE()
    },

    /* Version printing                   */
    {
         OPTHELP_DEFINE_DOUBLE_VERBOSE("version"),
         {   OPTIONS_NOT_REQUIRED,   OPTIONS_NO_ARGUMENTS,   },
         OPTHELP_DEFINE_FUNC_NONE(),
         OPTHELP_DEFINE_PRIOR_NONE(),
         OPTHELP_DEFINE_POST(postVersion),
         OPTHELP_DEFINE_VARIABLE_NONE(),
         OPTHELP_DEFINE_USERDEFINED_NONE()
    },

    /* port number override option  */
    {
        OPTHELP_DEFINE_DOUBLE_VERBOSE("port"),
        {
            OPTIONS_NOT_REQUIRED,
            OPTIONS_HAS_ARGUMENTS,
        },
        OPTHELP_DEFINE_FUNC(options_store_int),
        OPTHELP_DEFINE_PRIOR_NONE(),
        OPTHELP_DEFINE_POST_NONE(),
        OPTHELP_DEFINE_VARIABLE_INT(g_Ans1553Port),
        OPTHELP_DEFINE_USERDEFINED_NONE()
    },

    /* port number override option  */
    {
        OPTHELP_DEFINE_DOUBLE_VERBOSE("client-port"),
        {
            OPTIONS_NOT_REQUIRED,
            OPTIONS_HAS_ARGUMENTS,
        },
        OPTHELP_DEFINE_FUNC(options_store_int),
        OPTHELP_DEFINE_PRIOR_NONE(),
        OPTHELP_DEFINE_POST_NONE(),
        OPTHELP_DEFINE_VARIABLE_INT(g_Ans1553ClientPort),
        OPTHELP_DEFINE_USERDEFINED_NONE()
    },

    /* Enable debug log messages                */
    {
        OPTHELP_DEFINE_DOUBLE_VERBOSE("debug"),
        {
            OPTIONS_NOT_REQUIRED,
            OPTIONS_NO_ARGUMENTS,
        },
        OPTHELP_DEFINE_FUNC_NONE(),
        OPTHELP_DEFINE_PRIOR_NONE(),
        OPTHELP_DEFINE_POST(postSetDebugMode),
        OPTHELP_DEFINE_VARIABLE_NONE(),
        OPTHELP_DEFINE_USERDEFINED_NONE()
    },

    /* Enable informational log messages                */
    {
        OPTHELP_DEFINE_DOUBLE_VERBOSE("info"),
        {
            OPTIONS_NOT_REQUIRED,
            OPTIONS_NO_ARGUMENTS,
        },
        OPTHELP_DEFINE_FUNC_NONE(),
        OPTHELP_DEFINE_PRIOR_NONE(),
        OPTHELP_DEFINE_POST(postSetInfoMode),
        OPTHELP_DEFINE_VARIABLE_NONE(),
        OPTHELP_DEFINE_USERDEFINED_NONE()
    },

    /* execution mode flag override option  */
    {
        OPTHELP_DEFINE_DOUBLE_VERBOSE("service"),
        {
            OPTIONS_REQUIRED,
            OPTIONS_NO_ARGUMENTS,
        },
        OPTHELP_DEFINE_FUNC(activate_service),
        OPTHELP_DEFINE_PRIOR_NONE(),
        OPTHELP_DEFINE_POST_NONE(),
        OPTHELP_DEFINE_VARIABLE_NONE(),
        OPTHELP_DEFINE_USERDEFINED_NONE()
    },

    OPTIONS_TABLE_END
};



int main(int argc, char* argv[])
{
    int rc;

    /* Initialize the log facility */
    rc = ANSLogInitialize();
    if ( 0 != rc )
    {
        fprintf(stderr, "ANS1553: ANSLogInitialize failed!\n");
        return EXIT_FAILURE;    /** \return EXIT_FAILURE                    */
    }


    /* Parse any program arguments specified. */
    if(!parseArguments(argc, argv))
    {
        return EXIT_FAILURE;
    }

    /* Perform protocol specific initialization of
     * server
     */
    if(!Ans1553Init())
    {
        ANSLogError("Failed to initialize protocol specific settings");
        return EXIT_FAILURE;
    }

    /* if program not used as service */
    if(!g_run_service)
    {   
        AnsServerStart();
    }
    else
    {
        if(!AnsServiceStart(ANS1553_SERVICE_NAME))
        {
            return EXIT_FAILURE;
        }
    }

    return 0;
}





/***************************************************************************//**
 This function prints a program help message to stdout.
*******************************************************************************/
static  OPTIONS     postUsage (
    OPTIONS_VAR     *variable,      /*!< [in] default parameter (not used)  */
    void            *userdefined)   /*!< [in] default parameter (not used)  */
{
   NOTUSED_PTR(variable);
   NOTUSED_PTR(userdefined);

   printf(
   "%s V" ANS1553_VERSION_STRING_FULL "\n Arguments:\n"
   "--port <port number>:\n"
   "           Overrides the default server port number, which is %d.\n"
   "\n"
   "--client-port <port number>:\n"
   "           Overrides the default client port number, which is %d.\n"
   "\n"
   "--info:    Enables informational log messages (e.g. login/logout)\n"
   "\n"
   "--debug:   Enables full debug log messages\n"
   "\n"
   "--service: Run the ANS as system service\n"
   "\n"
   "--help:    Displays a brief option description (this message).\n"
   "\n"
   "--version: Prints the version of the ANS1553 Server.\n"
   "\n",
        g_Ans1553ServerInfo.name, g_Ans1553Port, g_Ans1553ClientPort);

    return OPTIONS_BREAK;       /** \return OPTIONS_BREAK           */
}


static  OPTIONS     postVersion (
    OPTIONS_VAR     *variable,      /*!< [in] default parameter (not used)  */
    void            *userdefined)   /*!< [in] default parameter (not used)  */
{
   NOTUSED_PTR(variable);
   NOTUSED_PTR(userdefined);

   printf(ANS1553_VERSION_STRING_FULL "\n");

   return OPTIONS_BREAK;       /** \return OPTIONS_BREAK           */
}


/***************************************************************************//**
 This function enables logging of debug messages.
*******************************************************************************/
static  OPTIONS     postSetDebugMode (
    OPTIONS_VAR     *variable,      /*!< [in] default parameter (not used)  */
    void            *userdefined)   /*!< [in] default parameter (not used)  */
{
    NOTUSED_PTR(variable);
    NOTUSED_PTR(userdefined);

    /* Enable full debug messages logging.                                  */
    ntlog_lset(NTLOG_LEVEL_DEBUG);

    /* Log the action.                                                      */
    ANSLogDebug("ANS1553: Debug messages enabled");

    ANSLogDebug("Performing sanity checks...");

    performSanityCheck("AiUInt8",  sizeof(AiUInt8),  1);
    performSanityCheck("AiUInt16", sizeof(AiUInt16), 2);
    performSanityCheck("AiUInt32", sizeof(AiUInt32), 4);
    performSanityCheck("AiInt8",   sizeof(AiInt8),  1);
    performSanityCheck("AiInt16",  sizeof(AiInt16), 2);
    performSanityCheck("AiInt32",  sizeof(AiInt32), 4);
    performSanityCheck("short",    sizeof(short), 2);
    performSanityCheck("AiUInt64", sizeof(AiUInt64), 8);
    performSanityCheck("AiInt64" , sizeof(AiInt64), 8);
    performSanityCheck("unsigned int", sizeof(unsigned int), 4);

    return OPTIONS_OK;      /*FR OPTIONS_OK         */
}

/***************************************************************************//**
 This function enables logging of informational messages.
*******************************************************************************/
static  OPTIONS     postSetInfoMode (
    OPTIONS_VAR     *variable,      /*!< [in] default parameter (not used)  */
    void            *userdefined)   /*!< [in] default parameter (not used)  */
{
    NOTUSED_PTR(variable);
    NOTUSED_PTR(userdefined);

    /* Enable full debug messages logging.                                  */
    ntlog_lset(NTLOG_LEVEL_INFO);

    /* Log the action.                                                      */
    ANSLogDebug("ANS1553: Info messages enabled");

    return OPTIONS_OK;      /*FR OPTIONS_OK         */
}


/***************************************************************************//**
This function performs some sanity checks to ensure that the AIM library
is properly set up.
*******************************************************************************/
 static void performSanityCheck (
     char const     *pTypeName, /*!< [in] name of data type                 */
     size_t const   typeSize,   /*!< [in] detected data type size           */
     size_t const   expected)   /*!< [in] expected data type size           */
{
    if( typeSize == expected )
    {
        ANSLogDebug(" OK: %.32s", pTypeName);
    }
    else
    {
        ANSLogError(" FAILED: %.32s: expected=%d defined=%d ***", pTypeName, (int) expected, (int) typeSize);
    }
}


 static OPTIONS activate_service(OPTIONS_VAR *variable,  
                                 void        *userdefined,              
                                 char        **argv,     
                                 int     *nprocessed)
 {
     g_run_service = TRUE;

     return OPTIONS_OK;
 }


 /***************************************************************************//**
 This function parses the arguments given at the command line.
 *******************************************************************************/
 static AnsBool     parseArguments (
     int         argc,       /*!< [in] number of arguments               */
     char        *argv[])    /*!< [in] argument vector                   */
 {
     AnsBool        rc;
     int             sourceindex = 1;
     OPTIONS         options;
     OPTIONS_DESC    options_desc = {g_options};

     if (argv == NULL || argv[0] == NULL)
         return AnsTrue;        /** \return AnsTrue on success         */

     /* Return if there are no arguments to parse.                       */
     if (argc < 2)
         return AnsTrue;

     /* Parse the arguments.                                             */
     if (options_init(&options_desc) != OPTIONS_OK)
         return AnsFalse;       /** \return AnsFalse on problems       */

     options = options_parse(argv, &sourceindex, &options_desc);
     switch ( options )
     {
     case OPTIONS_OK:
         rc = AnsTrue;
         break;

     case OPTIONS_BREAK:
         rc = AnsFalse;
         break;

     default:
         ANSLogError("%.128s", opterror_get(options, &options_desc));
         rc = AnsFalse;
         break;
     }

     (void) options_deinit(&options_desc);

     return rc;
 }


 static AnsBool Ans1553Init(void)
 {
     AiUInt8 numBoards = 0;

     ANSLogDebug("Performing 1553 specific initialization of ANS");

     AnsProtocol_setMagic(&g_AnsProtocol, ANS1553_MAGIC_VALUE);
     AnsProtocol_setVersion(&g_AnsProtocol, &g_Ans1553ProtocolVersion);
     AnsServer_setPort(&g_AnsServer, g_Ans1553Port);
     AnsServer_setClientPort(&g_AnsServer, g_Ans1553ClientPort);
     AnsServer_setInfo(&g_AnsServer, &g_Ans1553ServerInfo);
     AnsServer_setVersion(&g_AnsServer, &g_Ans1553ServerVersion);
     AnsServer_setBoardCommandHandlers(&g_AnsServer, &g_Ans1553BoardCommandHandlers);

     numBoards = ApiInit();
     AnsServer_setHostedBoardCount(&g_AnsServer, numBoards);

     return AnsTrue;
 }

 
