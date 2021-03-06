/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.4 at Mon Feb 08 14:07:16 2016. */

#ifndef PB_INOTIFICATION_PB_H_INCLUDED
#define PB_INOTIFICATION_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Struct definitions */
typedef struct _RemoveSubscription_request_response {
    pb_callback_t FunctionID;
} RemoveSubscription_request_response;

typedef struct _RemoveSubscription_response {
    pb_callback_t FunctionResults;
} RemoveSubscription_response;

typedef struct _Subscribe_request_response {
    pb_callback_t FunctionID;
} Subscribe_request_response;

typedef struct _Subscribe_response {
    pb_callback_t FunctionResults;
} Subscribe_response;

typedef struct _functionResults {
    bool has_FunctionID;
    uint32_t FunctionID;
    bool has_Successful;
    bool Successful;
} functionResults;

/* Default values for struct fields */

/* Initializer values for message structs */
#define Subscribe_request_response_init_default  {{{NULL}, NULL}}
#define Subscribe_response_init_default          {{{NULL}, NULL}}
#define RemoveSubscription_request_response_init_default {{{NULL}, NULL}}
#define RemoveSubscription_response_init_default {{{NULL}, NULL}}
#define functionResults_init_default             {false, 0, false, 0}
#define Subscribe_request_response_init_zero     {{{NULL}, NULL}}
#define Subscribe_response_init_zero             {{{NULL}, NULL}}
#define RemoveSubscription_request_response_init_zero {{{NULL}, NULL}}
#define RemoveSubscription_response_init_zero    {{{NULL}, NULL}}
#define functionResults_init_zero                {false, 0, false, 0}

/* Field tags (for use in manual encoding/decoding) */
#define RemoveSubscription_request_response_FunctionID_tag 1
#define RemoveSubscription_response_FunctionResults_tag 1
#define Subscribe_request_response_FunctionID_tag 1
#define Subscribe_response_FunctionResults_tag   1
#define functionResults_FunctionID_tag           1
#define functionResults_Successful_tag           2

/* Struct field encoding specification for nanopb */
extern const pb_field_t Subscribe_request_response_fields[2];
extern const pb_field_t Subscribe_response_fields[2];
extern const pb_field_t RemoveSubscription_request_response_fields[2];
extern const pb_field_t RemoveSubscription_response_fields[2];
extern const pb_field_t functionResults_fields[3];

/* Maximum encoded size of messages (where known) */
#define functionResults_size                     8

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define INOTIFICATION_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
