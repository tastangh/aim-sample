/* SPDX-FileCopyrightText: 2015-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file esmart.h
 *
 *  This file is the main interface file
 *  for the embedded System Management and Recovery tool daemon
 *  used on embedded Linux based AIM boards
 *
 */

#ifndef ESMART_H_
#define ESMART_H_

#include "Ai_def.h"

/*! \def ESMART_MAGIC
 * Every esmart request and response starts with this magic number
 */
#define ESMART_MAGIC 0x45534D54


/*! \def ESMART_HOSTIO_CHANNEL
 * ID of PCI communication channel eSmart uses for
 * communication with host
 */
#define ESMART_HOSTIO_CHANNEL   0


/*! \enum esmart_req_id
 *
 * Enumeration of all possible request IDs
 * the esmartd supports
 */
enum esmart_req_id
{
    ESMART_REQ_ECHO = 0,                //!< Request that echoes an ASCII string
    ESMART_REQ_INIT_FTRANSFER,          //!< Request for initializing a file transfer
    ESMART_REQ_RECEIVE_CHUNK,           //!< Request for file chunk during file upload
    ESMART_REQ_SEND_CHUNK,              //!< Request for sending file chunk during file download
    ESMART_REQ_COMPLETE_FTRANSFER,      //!< Request for completing a file transfer
    ESMART_REQ_ABORT_FTRANSFER,         //!< Request for aborting a file transfer session
    ESMART_REQ_UPDATE_STATUS,           //!< Request for retrieving the update status
    ESMART_REQ_UPDATE_START,            //!< Request for starting the update
    ESMART_REQ_NOVRAM_GET,              //!< Request to read NOVRAM values from board
    ESMART_REQ_NOVRAM_SET,              //!< Request to set NOVRAM values on board
    ESMART_REQ_NOVRAM_PROG,             //!< Request to program NOVRAM values to board
    ESMART_REQ_VERSION_GET,             //!< Request to read Versuion values from board
    __ESMART_REQ_MAX                    //!< Marks end of request IDs
};


/*! \def ESMART_MAX_REQ_ID
 *
 * Highest request ID that is supported
 */
#define ESMART_MAX_REQ_ID  (__ESMART_REQ_MAX - 1)


/*! \enum esmart_ftransfer_dir
 *
 * Enumeration of possible file transfer directions
 */
enum esmart_ftransfer_dir
{
    ESMART_FTRANSFER_DOWN = 0,  //!< File transfer from host to embedded device
    ESMART_FTRANSFER_UP         //!< File transfer from embedded device to host
};


/*! \enum esmart_update_status
 *
 * Enumeration of possible update states
 */
enum esmart_update_status
{
    ESMART_UPDATE_TASK_FAILURE = -1, //!< Error in communication with update daemon
    ESMART_UPDATE_WAITING,           //!< Embedded device is waiting for update
    ESMART_UPDATE_STARTING,          //!< Start of update has been requested
    ESMART_UPDATE_EXTRACTING,        //!< Embedded device is extracting the update package
    ESMART_UPDATE_RUNNING,           //!< Update is in progress
    ESMART_UPDATE_FAILED,            //!< Update has finished and failed
    ESMART_UPDATE_FINISHED,          //!< Update has finished successfully
};



/*! \def ESMART_OK
 * No error occurred during request
 */
#define ESMART_OK                    0

/*! \def ESMART_FILE_EOF
 * End of file was reached during file upload
 */
#define ESMART_FILE_EOF              1

/*! \def ESMART_MAGIC_ERROR
 * Frame contained invalid magic number
 */
#define ESMART_MAGIC_ERROR          -1

/*! \def ESMART_MEMORY_ERROR
 * Not enough memory available
 */
#define ESMART_MEMORY_ERROR         -2

/*! \def ESMART_INVALID_FRAME_SIZE
 * Request frame has invalid size
 */
#define ESMART_INVALID_FRAME_SIZE   -3

/*! \def ESMART_INVALID_REQUEST
 * Unknown request was issued
 */
#define ESMART_INVALID_REQUEST      -4

/*! \def ESMART_INVALID_TRANSFER_DIR
 * Invalid file transfer direction was specified
 */
#define ESMART_INVALID_TRANSFER_DIR -5

/*! \def ESMART_INTERNAL_ERROR
 * Internal, not further specified error
 */
#define ESMART_INTERNAL_ERROR       -6

/*! \def ESMART_FILE_IN_USE
 * File that is subject to file transfer related
 * request is already in use.
 */
#define ESMART_FILE_IN_USE          -7

/*! \def ESMART_FILE_NO_ACCESS
 * File that is subject to file transfer related
 * request can not be accessed
 */
#define ESMART_FILE_NO_ACCESS        -8

/*! \def ESMART_INVALID_SESSION
 * Invalid file transfer session ID was specified
 */
#define ESMART_INVALID_SESSION      -9

/*! \def ESMART_FILE_RELEASE_ERROR
 * File that is subject to file transfer related
 * request can not be closed
 */
#define ESMART_FILE_RELEASE_ERROR   -10

/*! \def ESMART_FILE_READ_ERROR
 * File that is subject to file transfer related
 * request can not be read
 */
#define ESMART_FILE_READ_ERROR      -11

/*! \def ESMART_FILE_WRITE_ERROR
 * File that is subject to file transfer related
 * request can not be written
 */
#define ESMART_FILE_WRITE_ERROR     -12


/*! \def ESMART_RESPONSE_NOT_MATCHING
 * The request ID encoded in the response header
 * does not match the originating request ID
 */
#define ESMART_RESPONSE_NOT_MATCHING -13




/*! \struct esmart_request
 *
 * Generic header that precedes every request frame
 */
#pragma pack(4)
struct esmart_request
{
    AiUInt32 magic;             /*!< Magic number. See \ref ESMART_MAGIC */
    AiUInt32 request_id;        /*!< ID of request. See \ref esmart_req_id */
    AiUInt32 request_size;      /*!< Total size of request frame in bytes */
    AiUInt32 reserved_1;
    AiUInt32 reserved_2;
    AiUInt32 reserved_3;
    AiUInt32 reserved_4;
    AiUInt32 reserved_5;
};
#pragma pack()


/*! \struct esmart_response
 *
 * Generic header that precedes every response frame
 */
#pragma pack(4)
struct esmart_response
{
    AiUInt32 magic;             /*!< Magic number. See \ref ESMART_MAGIC */
    AiUInt32 request_id;        /*!< ID of corresponding request. See \ref esmart_req_id */
    AiInt32  request_status;    /*!< Overall status of request */
    AiUInt32 response_size;     /*!< Total size of response frame in bytes */
    AiUInt32 reserved_1;
    AiUInt32 reserved_2;
    AiUInt32 reserved_3;
    AiUInt32 reserved_4;
    AiUInt32 reserved_5;
};
#pragma pack()



/*! \struct esmart_echo_req
 *
 *  Request frame for ESMART_REQ_ECHO.
 *  Frame has variable length depending on string length
 */
#pragma pack(4)
struct esmart_echo_req
{
    struct esmart_request header;   /*!< Generic request header */
    AiUInt32 string_length;         /*!< number of characters the string to echo consists of */
    AiChar string;                  /*!< start of zero-terminated ASCII string */
};
#pragma pack()

#define esmart_echo_req_size(string_length) \
    sizeof(struct esmart_echo_req) - sizeof(AiChar) + string_length + 1


/*! \struct esmart_echo_resp
 *
 *  Response frame for ESMART_REQ_ECHO.
 *  Frame has variable length depending on string length
 */
#pragma pack(4)
struct esmart_echo_resp
{
    struct esmart_response header;  /*!< Generic response header */
    AiUInt32 string_length;         /*!< number of characters the echoed string consists of */
    AiChar string;                  /*!< start of zero-terminated ASCII string */
};
#pragma pack()

#define esmart_echo_resp_size(string_length) \
    sizeof(struct esmart_echo_resp) - sizeof(AiChar) + string_length + 1



/*! \struct esmart_receive_chunk_req
 *
 * Request frame for ESMART_REQ_RECEIVE_CHUNK
 */
#pragma pack(4)
struct esmart_receive_chunk_req
{
    struct esmart_request header;   /*!< Generic header */
    AiUInt32 session_id;            /*!< File transfer session ID to receive data chunk from */
    AiUInt32 max_data_size;         /*!< Maximum number of bytes the data chunk consists of */
};
#pragma pack()


/*! \struct esmart_receive_chunk_resp
 *
 * Response frame for ESMART_REQ_RECEIVE_CHUNK.
 * Frame has variable length depending in data_size
 */
#pragma pack(4)
struct esmart_receive_chunk_resp
{
    struct esmart_response header;  /*!< Generic response header */
    AiInt32 status;                 /*!< status code for request */
    AiUInt32 session_id;            /*!< The session ID the data chunk belongs to */
    AiUInt32 data_size;             /*!< size of data chunk in bytes */
    AiUInt8 data;                   /*!< start of raw binary data */
};
#pragma pack()


/*! \def esmart_receive_chunk_resp_size
 *
 * calculate total size in bytes of response frame for \ref esmart_receive_chunk_resp
 * for a given data chunk size
 */
#define esmart_receive_chunk_resp_size(data_size) \
    (sizeof(struct esmart_receive_chunk_resp) + data_size - sizeof(AiUInt8))


/*! \struct esmart_send_chunk_req
 *
 *  Request frame for ESMART_REQ_SEND_CHUNK.
 *  Frame has variable length depending on data_size
 */
#pragma pack(4)
struct esmart_send_chunk_req
{
    struct esmart_request header;   /*!< Generic request header */
    AiUInt32 session_id;            /*!< File transfer session ID to send chunk for */
    AiUInt32 data_size;             /*!< number of bytes the data chunk consists of */
    AiUInt8 data;                   /*!< start of raw binary data */
};
#pragma pack()


/*! \def esmart_send_chunk_req_size
 *
 * calculate total size in bytes of request frame for \ref esmart_send_chunk_req
 * for a given data chunk size
 */
#define esmart_send_chunk_req_size(data_size) \
    ((AiUInt32)(sizeof(struct esmart_send_chunk_req) + data_size - sizeof(AiUInt8)))


/*! \struct esmart_send_chunk_resp
 *
 * Response frame for ESMART_REQ_SEND_CHUNK.
 */
#pragma pack(4)
struct esmart_send_chunk_resp
{
    struct esmart_response header;  /*!< Generic response header */
    AiInt32 status;                 /*!< status code */
    AiUInt32 session_id;            /*!< File transfer session ID the chunk was sent to */
};
#pragma pack()


/*! \struct esmart_complete_transfer_req
 *
 * Request frame for ESMART_REQ_COMPLETE_FTRANSFER
 */
#pragma pack(4)
struct esmart_complete_transfer_req
{
    struct esmart_request header;   /*!< Generic request header */
    AiUInt32 session_id;            /*!< File transfer session ID to complete */
};
#pragma pack()


/*! \struct esmart_complete_transfer_resp
 *
 * Response frame for ESMART_REQ_COMPLETE_FTRANSFER
 */
#pragma pack(4)
struct esmart_complete_transfer_resp
{
    struct esmart_response header;  /*!< Generic response header */
    AiInt32 status;                 /*!< status code */
    AiUInt32 session_id;            /*!< File transfer session ID that was completed */
};
#pragma pack()


/*! \struct esmart_abort_transfer_req
 *
 * Request frame for ESMART_REQ_ABORT_FTRANSFER
 */
#pragma pack(4)
struct esmart_abort_transfer_req
{
    struct esmart_request header;   /*!< Generic request header */
    AiUInt32 session_id;            /*!< File transfer session ID to abort */
};
#pragma pack()


/*! \struct esmart_abort_transfer_resp
 *
 * Response frame for ESMART_REQ_ABORT_FTRANSFER
 */
#pragma pack(4)
struct esmart_abort_transfer_resp
{
    struct esmart_response header;  /*!< Generic response header */
    AiInt32 status;                 /*!< status code */
    AiUInt32 session_id;            /*!< File transfer session ID that was aborted */
};
#pragma pack()


/*! \struct esmart_init_transfer_req
 * 
 * Request frame for ESMART_REQ_INIT_FTRANSFER
 * Request has variable length depending on file_name_len
 */
#pragma pack(4)
struct esmart_init_transfer_req
{
    struct esmart_request header;           /*!< Generic request header */
    enum esmart_ftransfer_dir direction;    /*!< Direction of file transfer. See \ref esmart_ftransfer_dir */
    AiUInt32 file_name_len;                 /*!< size of file name in bytes excluding trailing zero!!! */
    AiUInt8 file_name;                      /*!< start of file name ASCII string */
};
#pragma pack()


/*! \def esmart_init_transfer_req_size
 *
 * calculate total size in bytes of request frame for \ref esmart_init_transfer_req
 * for a given file name length as returned by strlen.
 */
#define esmart_init_transfer_req_size(file_name_len) \
    ((AiUInt32)(sizeof(struct esmart_init_transfer_req) + file_name_len + 1 - sizeof(AiUInt8)))


/*! \struct esmart_init_transfer_resp
 *
 * Response frame for ESMART_REQ_INIT_FTRANSFER
 */
#pragma pack(4)
struct esmart_init_transfer_resp
{
    struct esmart_response header;  /*!< Generic response header */
    AiInt32 status;                 /*!< status code */
    AiUInt32 session_id;            /*!< ID that is assigned to the session */
};
#pragma pack()


/*! \struct esmart_update_status_req
 *
 * Request frame for ESMART_REQ_UPDATE_STATUS
 */
#pragma pack(4)
struct esmart_update_status_req
{
    struct esmart_request header;   /*!< Generic request header */
};
#pragma pack()


/*! \struct esmart_update_status_resp
 *
 * Response frame for ESMART_REQ_UPDATE_STATUS
 */
#pragma pack(4)
struct esmart_update_status_resp
{
    struct esmart_response header;      /*!< Generic response header */
    enum esmart_update_status status;   /*!< Current status of update. See \ref esmart_update_status */
    AiUInt32 components_done;           /*!< Number of components that are already updated when update is in progress */
    AiUInt32 num_components;            /*!< Total number of components that are to update when update is in progress */
};
#pragma pack()


/*! \struct esmart_update_start_req
 *
 *  Request frame for ESMART_REQ_UPDATE_START
 *  Frame has variable length depending on file_name_len
 */
#pragma pack(4)
struct esmart_update_start_req
{
    struct esmart_request header;   /*!< Generic request header */
    AiUInt32 file_name_len;         /*!< length of update package name in bytes (excluding trailing zero!) */
    AiUInt8 file_name;              /*!< start of update package file name ASCII string */
};
#pragma pack()


/*! \def esmart_update_start_req_size
 *
 * calculate total size in bytes of request frame for \ref esmart_update_start_req
 * for a given file name length as returned by strlen.
 */
#define esmart_update_start_req_size(file_name_len) \
    ((AiUInt32)(sizeof(struct esmart_update_start_req) + file_name_len + 1 - sizeof(AiUInt8)))

/*! \struct esmart_update_start_resp
 *
 * Response frame for ESMART_REQ_UPDATE_START
 */
#pragma pack(4)
struct esmart_update_start_resp
{
    struct esmart_response header;      /*!< Generic response header */
    enum esmart_update_status status;   /*!< Current status of update. See \ref esmart_update_status */
    AiUInt32 components_done;           /*!< Number of components that are already updated */
    AiUInt32 num_components;            /*!< Total number of components that are to update */
};
#pragma pack()


/*! \struct esmart_novram_get_req
 *
 *  Request frame for ESMART_REQ_NOVRAM_GET.
 */
#pragma pack(4)
struct esmart_novram_get_req
{
    struct esmart_request header;   /*!< Generic request header */
    AiUInt32 start_offset;          /*!< Novram offset in bytes to start get operation with (Must be a multiple of 4) */
    AiUInt32 num_entries;           /*!< Number of Novram entries to get (num_entries * 4 is total size in bytes) */
};
#pragma pack()


/*! \struct esmart_novram_get_resp
 *
 * Response frame for ESMART_REQ_NOVRAM_GET
 * Frame has variable length depending on number of NOVRAM entries read.
 */
#pragma pack(4)
struct esmart_novram_get_resp
{
    struct esmart_response header;      /*!< Generic response header */
    AiInt32 status;                     /*!< status code */
    AiUInt32 num_entries;               /*!< Number of Novram entries gotten (num_entries * 4 is total size in bytes) */
    AiUInt8 novram_data;                /*!< raw binary Novram data starts here */
};
#pragma pack()

/*! \def esmart_novram_get_resp_size
 *
 * calculate total size in bytes of response frame for \ref esmart_novram_get_resp
 * for a given number of Novram entries.
 */
#define esmart_novram_get_resp_size(num_entries) \
    ((AiUInt32)(sizeof(struct esmart_novram_get_resp) + (num_entries * sizeof(AiUInt32)) - sizeof(AiUInt8)))


/*! \struct esmart_novram_set_req
 *
 *  Request frame for ESMART_REQ_NOVRAM_SET.
 *  Frame has variable size depending on num_entries
 */
#pragma pack(4)
struct esmart_novram_set_req
{
    struct esmart_request header;   /*!< Generic request header */
    AiUInt32 start_offset;          /*!< Novram offset in bytes to start set operation with (Must be a multiple of 4) */
    AiUInt32 num_entries;           /*!< Number of Novram entries to set (num_entries * 4 is total size in bytes) */
    AiUInt8 novram_data;            /*!< Raw binary novram data starts here */
};
#pragma pack()


/*! \def esmart_novram_set_req_size
 *
 * calculate total size in bytes of request frame for \ref esmart_novram_set_req
 * for a given number of Novram entries.
 */
#define esmart_novram_set_req_size(num_entries) \
    ((AiUInt32)(sizeof(struct esmart_novram_set_req) + (num_entries * sizeof(AiUInt32)) - sizeof(AiUInt8)))


/*! \struct esmart_novram_set_resp
 *
 * Response frame for ESMART_REQ_NOVRAM_SET
 */
#pragma pack(4)
struct esmart_novram_set_resp
{
    struct esmart_response header;      /*!< Generic response header */
    AiInt32 status;                     /*!< status code */
};
#pragma pack()


/*! \struct esmart_novram_prog_req
 *
 *  Request frame for ESMART_REQ_NOVRAM_PROG.
 */
#pragma pack(4)
struct esmart_novram_prog_req
{
    struct esmart_request header;   /*!< Generic request header */
};
#pragma pack()

/*! \struct esmart_novram_prog_resp
 *
 * Response frame for ESMART_REQ_NOVRAM_PROG
 */
#pragma pack(4)
struct esmart_novram_prog_resp
{
    struct esmart_response header;      /*!< Generic response header */
    AiInt32 status;                     /*!< status code */
};
#pragma pack()

/*! \struct esmart_version_get_req
 *
 *  Request frame for ESMART_REQ_VERSION_GET.
 */
#pragma pack(4)
struct esmart_version_get_req
{
    struct esmart_request header;   /*!< Generic request header */
    TY_E_VERSION_ID versionId;            /*!< Versin Id ro read */
};
#pragma pack()


/*! \struct esmart_version_get_resp
 *
 * Response frame for ESMART_REQ_VERSION_GET
 */
#pragma pack(4)
struct esmart_version_get_resp
{
    struct esmart_response header;      /*!< Generic response header */
    AiInt32 status;                     /*!< status code */
    TY_E_VERSION_ID versionId;          /*!< version id */
    TY_VER_INFO version;                /*!< version info */
};
#pragma pack()

#endif /* ESMART_H_ */
