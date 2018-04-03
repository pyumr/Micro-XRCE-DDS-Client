// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "message.h"

#define YELLOW_DARK    "\x1B[0;33m"
#define PURPLE_DARK    "\x1B[0;35m"
#define GREY_LIGHT     "\x1B[0;37m"

#define GREY_DARK      "\x1B[1;30m"
#define RED            "\x1B[1;31m"
#define GREEN          "\x1B[1;32m"
#define YELLOW         "\x1B[1;33m"
#define BLUE           "\x1B[1;34m"
#define PURPLE         "\x1B[1;35m"
#define CYAN           "\x1B[1;36m"
#define WHITE          "\x1B[1;37m"

#define RESTORE_COLOR  "\x1B[0m"

#if defined(SERIALIZATION_LOGS) || defined(MESSAGE_LOGS)
//Only one call into a printf.
const char* data_to_string(const uint8_t* data, uint32_t size);

const char* data_to_string(const uint8_t* data, uint32_t size)
{
    static char buffer[4096];
    for(uint32_t i = 0; i < size; i++)
        sprintf(buffer + 3 * i, "%02X ", data[i]);
    return buffer;
}
#endif

#ifdef MESSAGE_LOGS
const char* request_to_string(const BaseObjectRequest* request);
const char* reply_to_string(const BaseObjectReply* reply);

const char* request_to_string(const BaseObjectRequest* request)
{
    static char buffer[256];
    sprintf(buffer, "#0x%04X | id: 0x%04X",
            get_num_request_id(request->request_id),
            get_num_object_id(request->object_id));

    return buffer;
}

const char* reply_to_string(const BaseObjectReply* reply)
{
    static char buffer[256];

    char status[64];
    switch(reply->result.status)
    {
        case STATUS_OK:
            sprintf(status, "OK");
            break;
        case STATUS_OK_MATCHED:
            sprintf(status, "OK_MATCHED");
            break;
        case STATUS_ERR_DDS_ERROR:
            sprintf(status, "ERR_DDS_ERROR");
            break;
        case STATUS_ERR_MISMATCH:
            sprintf(status, "ERR_MISMATCH");
            break;
        case STATUS_ERR_ALREADY_EXISTS:
            sprintf(status, "ERR_ALREADY_EXISTS");
            break;
        case STATUS_ERR_DENIED:
            sprintf(status, "ERR_DENIED");
            break;
        case STATUS_ERR_UNKNOWN_REFERENCE:
            sprintf(status, "ERR_UNKNOWN_REFERENCE");
            break;
        case STATUS_ERR_INVALID_DATA:
            sprintf(status, "ERR_INVALID_DATA");
            break;
        case STATUS_ERR_INCOMPATIBLE:
            sprintf(status, "ERR_INCOMPATIBLE");
            break;
        case STATUS_ERR_RESOURCES:
            sprintf(status, "ERR_RESOURCES");
            break;
        default:
            sprintf(status, "UNKNOWN");
    }

    sprintf(buffer, "#0x%04X | id: 0x%04X | from #0x%04X | %s",
            get_num_request_id(reply->related_request.request_id),
            get_num_object_id(reply->related_request.object_id),
            get_num_request_id(reply->related_request.request_id),
            status);

    return buffer;
}
#endif

#ifdef SERIALIZATION_LOGS
void PRINTL_SERIALIZATION(const char* pre, const uint8_t* buffer, uint32_t size)
{
    printf("%s%s<< [%u]: %s>>%s\n",
            pre,
            GREY_DARK,
            size,
            data_to_string(buffer, size),
            RESTORE_COLOR);
}
#endif

#ifdef MESSAGE_LOGS
void PRINT_SEQUENCE_NUMBER(uint16_t message_sequence_number, uint16_t local_sequence_number)
{
    printf("%s%sSequence number error => received: %u, expected: %u.%s\n",
            RECV,
            RED,
            message_sequence_number,
            local_sequence_number,
            RESTORE_COLOR);
}

void PRINTL_CREATE_CLIENT_SUBMESSAGE(const CREATE_CLIENT_Payload* payload)
{
    printf("%s%s[Create client | %s | session: 0x%02X]%s\n",
            SEND,
            YELLOW,
            request_to_string(&payload->base),
            payload->client_representation.session_id,
            RESTORE_COLOR);
}

void PRINTL_CREATE_RESOURCE_SUBMESSAGE(const CREATE_Payload* payload)
{
    char content[4096];
    switch(payload->object_representation.kind)
    {
        case OBJK_PARTICIPANT:
            sprintf(content, "PARTICIPANT");
            break;
        case OBJK_TOPIC:
            sprintf(content, "TOPIC | id: 0x%04X | topic: %u",
                    get_num_object_id(payload->object_representation._.data_reader.subscriber_id),
                    payload->object_representation._.data_reader.base.representation._.string_represenatation.size);
            break;
        case OBJK_PUBLISHER:
            sprintf(content, "PUBLISHER | id: 0x%04X",
                    get_num_object_id(payload->object_representation._.publisher.participant_id));
            break;
        case OBJK_SUBSCRIBER:
            sprintf(content, "SUBSCRIBER | id: 0x%04X",
                    get_num_object_id(payload->object_representation._.subscriber.participant_id));
            break;
        case OBJK_DATAWRITER:
            sprintf(content, "DATA_WRITER | id: 0x%04X | xml: %u",
                    get_num_object_id(payload->object_representation._.data_writer.publisher_id),
                    payload->object_representation._.data_writer.base.representation._.string_represenatation.size);
             break;
        case OBJK_DATAREADER:
            sprintf(content, "DATA_READER | id: 0x%04X | xml: %u",
                    get_num_object_id(payload->object_representation._.data_reader.subscriber_id),
                    payload->object_representation._.data_reader.base.representation._.string_represenatation.size);
            break;
        default:
            sprintf(content, "UNKNOWN");
    }

    printf("%s%s[Create | %s | %s]%s\n",
            SEND,
            YELLOW,
            request_to_string(&payload->base),
            content,
            RESTORE_COLOR);
}

void PRINTL_GET_INFO_SUBMESSAGE(const GET_INFO_Payload* payload)
{
    //TODO
}

void PRINTL_DELETE_RESOURCE_SUBMESSAGE(const DELETE_Payload* payload)
{
    printf("%s%s[Delete | %s]%s\n",
            SEND,
            YELLOW,
            request_to_string(&payload->base),
            RESTORE_COLOR);
}

void PRINTL_STATUS_SUBMESSAGE(const STATUS_Payload* payload)
{
    printf("%s%s[Status | %s]%s\n",
            RECV,
            PURPLE,
            reply_to_string(&payload->base),
            RESTORE_COLOR);
}

void PRINTL_INFO_SUBMESSAGE(const INFO_Payload* payload)
{
    //TODO
}

void PRINTL_READ_DATA_SUBMESSAGE(const READ_DATA_Payload* payload)
{
    char format[128];
    switch(payload->read_specification.data_format)
    {
        case FORMAT_DATA:
            sprintf(format, "FORMAT_DATA");
        break;
        case FORMAT_DATA_SEQ:
            sprintf(format, "FORMAT_DATA_SEQ");
        break;
        case FORMAT_SAMPLE:
            sprintf(format, "FORMAT_SAMPLE");
        break;
        case FORMAT_SAMPLE_SEQ:
            sprintf(format, "FORMAT_SAMPLE_SEQ");
        break;
        case FORMAT_PACKED_SAMPLES:
            sprintf(format, "FORMAT_PACKED_SAMPLES");
        break;
        default:
            sprintf(format, "FORMAT_DATA_UNKNOWN");
    }

    printf("%s%s[Read data | %s | %s]%s\n",
            SEND,
            YELLOW,
            request_to_string(&payload->base),
            format,
            RESTORE_COLOR);
}

void PRINTL_WRITE_DATA_DATA_SUBMESSAGE(const WRITE_DATA_Payload_Data* payload)
{
    printf("%s%s[Write data | FORMAT_DATA | %s]%s\n",
            SEND,
            YELLOW,
            request_to_string(&payload->base),
            RESTORE_COLOR);
}

void PRINTL_DATA_DATA_SUBMESSAGE(const DATA_Payload_Data* payload)
{
    printf("%s%s[Data | FORMAT_DATA | %s]%s\n",
            RECV,
            PURPLE,
            request_to_string(&payload->base),
            RESTORE_COLOR);
}

void PRINTL_ACKNACK_SUBMESSAGE(const ACKNACK_Payload* payload)
{
    printf("%s%s[Acknack | seq_num: %u | bitmap: %s]%s\n",
            RECV,
            PURPLE,
            payload->first_unacked_seq_num,
            data_to_string(payload->nack_bitmap, 2),
            RESTORE_COLOR);
}

void PRINTL_HEARTBEAT_SUBMESSAGE(const HEARTBEAT_Payload* payload)
{
    printf("%s%s[Heartbeat | first: %u | last: %u]%s\n",
            RECV,
            PURPLE,
            payload->first_unacked_seq_nr,
            payload->last_unacked_seq_nr,
            RESTORE_COLOR);
}

#endif
