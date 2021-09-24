#include "Websocket.h"

static const char *TAG = "Websocket";

Websocket websocket; //singleton instance of class

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
        ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
        if (data->op_code == 0x08 && data->data_len == 2)
        {
            ESP_LOGW(TAG, "Received closed message with code=%d", 256 * data->data_ptr[0] + data->data_ptr[1]);
        }
        else
        {
            if (data->data_len > 0)
            {
                websocket.handle(data->data_ptr, data->data_len);
            }
        }
        ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);

        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    }
}

enum HEADERS
{
    CONNECTED,
    REGISTRATION,
    REGISTER_PENDING,
    REGISTER_SUCCESS,
};

//TODO: CHANGE SERVER CODE TO SEND ENUM INSTEAD OF STRINGS (REMOVE THIS MAP)
static std::map<std::string, HEADERS> header_map;

static void header_map_init()
{
    header_map["CONNECTED"] = CONNECTED;
    header_map["REGISTRATION"] = REGISTRATION;
    header_map["REGISTER PENDING"] = REGISTER_PENDING;
    header_map["REGISTER SUCCESS"] = REGISTER_SUCCESS;
}

void Websocket::start(void)
{
    header_map_init();

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

    header_map.clear();
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

void Websocket::handle(const char *msg, uint8_t len)
{
    const cJSON *header = NULL;
    const cJSON *payload = NULL;
    int status = -1;
    printf("raw: %.*s\n", len, msg);
    cJSON *msg_json = cJSON_ParseWithLength(msg, len);
    if (msg_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 501;
        goto end;
    }

    header = cJSON_GetObjectItemCaseSensitive(msg_json, "header");
    if (!cJSON_IsString(header) || (header->valuestring == NULL))
    {
        status = 404;
        goto end;
    }

    payload = cJSON_GetObjectItemCaseSensitive(msg_json, "payload");

    printf("header: \"%s\"\n", header->valuestring);

    switch (header_map[header->valuestring])
    {
    case CONNECTED:
    {
        printf("Connected to Websocket\n");
        status = HEADERS::CONNECTED;
        break;
    }
    case REGISTRATION:
    {
        printf("Registration\n");
        status = HEADERS::REGISTRATION;
        break;
    }
    case REGISTER_PENDING:
    {
        printf("Register Pending\n");
        status = HEADERS::REGISTER_PENDING;
        break;
    }
    case REGISTER_SUCCESS:
    {
        if (!cJSON_IsObject(payload))
        {
            status = 505;
            goto end;
        }

        cJSON *username = cJSON_GetObjectItemCaseSensitive(payload, "username");

        if (cJSON_IsString(username) && (username->valuestring != NULL))
        {
            printf("username: \"%s\"\n", username->valuestring);
            strcpy(settings.username, username->valuestring);
            settings.save();
        }
        else
        {
            status = -1;
            goto end;
        }
        status = HEADERS::REGISTER_SUCCESS;
        break;
    }
    }

end:
    cJSON_Delete(msg_json);
    // return status;
    printf("status: %d\n", status);
}