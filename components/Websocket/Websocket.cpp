#include "Websocket.h"

static const char *TAG = "Websocket";

Websocket websocket; //singleton instance of class

static size_t json_len(const char *msg)
{
    if (msg[0] != '{')
    {
        return 0; //invalid
    }
    const int timeout = 200;
    size_t len = 0;
    int stack = 0;

    for (int i = 0; i < timeout; i++)
    {
        len++;
        if (msg[i] == '{')
        {
            stack++;
        }
        else if (msg[i] == '}')
        {
            stack--;
        }

        if (stack == 0)
        {
            return len;
        }
    }
    return 0; //timeout!
}

static void event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        break;
    case WEBSOCKET_EVENT_DATA:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
        //TODO: ADD SOCKET MANAGER CODE HERE
        ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
        if (data->op_code == 0x08 && data->data_len == 2)
        {
            ESP_LOGW(TAG, "Received closed message with code=%d", 256 * data->data_ptr[0] + data->data_ptr[1]);
        }
        else
        {
            // ESP_LOGW(TAG, "Received=%.*s", data->data_len, (char *)data->data_ptr);
            size_t len = json_len(data->data_ptr);
            if (len == 0)
            {
                ESP_LOGE(TAG, "Invalid JSON MSG");
            }
            else
            {
                websocket.handle(data->data_ptr, len);
            }
        }
        ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);

        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    }
}

void Websocket::start(void)
{
    esp_websocket_client_config_t websocket_cfg = {};

    websocket_cfg.uri = CONFIG_WEBSOCKET_URI;

    ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);

    Websocket::client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(Websocket::client, WEBSOCKET_EVENT_ANY, event_handler, (void *)Websocket::client);

    esp_websocket_client_start(Websocket::client);
}

void Websocket::stop(void)
{
    esp_websocket_client_close(Websocket::client, portMAX_DELAY);
    ESP_LOGI(TAG, "Websocket Stopped");
    esp_websocket_client_destroy(Websocket::client);
}

/**
 * @param - msg: this must be the output of a WebSocketMsg.h function call!
 */
int Websocket::send(char *msg)
{
    //wrapper for wifi client send to ws
    size_t len = strlen(msg);
    ESP_LOGI(TAG, "Sending %s", msg);
    int res = esp_websocket_client_send_text(Websocket::client, msg, len, portMAX_DELAY);
    free(msg);
    return res;
}

// void Websocket::handle(const char *msg, uint8_t len)
// {
//     size_t real_len = json_len(msg);
//     ESP_LOGW(TAG, "Raw=%.*s", len, msg);

//     cJSON *json = cJSON_ParseWithLength(msg, real_len);
//     printf("parsed: %s\n", cJSON_Print(json));

//     char *str = NULL;

//     cJSON *header = cJSON_GetObjectItem(json, "header");
//     cJSON *meta = cJSON_GetObjectItem(json, "meta");
//     str = cJSON_Print(header);

//     if (strcmp(str, "REGISTER PENDING"))
//     {
//         char *metastr = NULL;
//         metastr = cJSON_Print(meta);
//         printf("meta: %s\n", metastr);
//         strcpy(settings.username, metastr);
//         free(metastr);
//     }
//     free(str);
//     cJSON_Delete(header);
//     cJSON_Delete(meta);
//     cJSON_Delete(json);
// }
void Websocket::handle(const char *msg, uint8_t len)
{
    const cJSON *header = NULL;
    const cJSON *payload = NULL;
    int status = 0;
    cJSON *msg_json = cJSON_ParseWithLength(msg, len);
    if (msg_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }

    header = cJSON_GetObjectItemCaseSensitive(msg_json, "header");
    if (cJSON_IsString(header) && (header->valuestring != NULL))
    {
        printf("Checking header \"%s\"\n", header->valuestring);
        if (strcmp("REGISTER SUCCESS", header->valuestring))
        {
            status = -1;
            goto end;
        }
    }

    payload = cJSON_GetObjectItemCaseSensitive(msg_json, "payload");
    if (cJSON_IsObject(payload))
    {
        cJSON *username = cJSON_GetObjectItemCaseSensitive(payload, "username");

        if (cJSON_IsString(username) && (username->valuestring != NULL))
        {
            printf("Checking username \"%s\"\n", username->valuestring);
            strcpy(settings.username, username->valuestring);
            settings.save();
        }
    }

end:
    cJSON_Delete(msg_json);
    // return status;
    printf("status: %d\n", status);
}