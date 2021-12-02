#include "Websocket.h"

static const char *TAG = "WEBSOCKET";

// WEBSOCKET META TAGS
#define REGISTRATION_WAITING_FOR_WEB_TAG "WAITING FOR WEB"

#define JOINED_EMPTY_GAME_TAG "Empty Game"

#define INVITE_SENT_TAG "INVITE SENT"
#define LIST_SENT_TAG "LIST SENT"
#define GAME_OVER_LOSER_TAG "You Lost :("
#define GAME_OVER_WINNER_TAG "You Won!"
#define SHIP_SUNK_TAG "SUNK"
#define SHIP_HIT_TAG "HIT"

Websocket websocket; // singleton instance of class

static void ws_connection_msg_task(void *args)
{
    for (;;)
    {
        websocket.send(messenger.build_connected_msg(false));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
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
    // Registration Header
    REGISTRATION,
    REGISTER_PENDING,
    REGISTER_SUCCESS,
    REGISTER_ERROR,
    // Game Headers
    // GAME_ALREADY_EXITS, //FIXME: REMOVE THIS FROM SERVER
    GAME_CREATED,
    GAME_STARTED,
    MOVE_RESULT,
    INVALID_MOVE,
    JOINED_GAME,
    POSITIONED_SHIPS,
    INVALID_LAYOUT,
    DATABASE_SUCCESS,
    BOARD_UPDATE,
    LEAVE_GAME,
    TERMINATED_REGISTER,
    // GAME_TYPE_APPROVED, //TODO: ADD THESE BACK IF/WHEN WE MAKE MULTIPLE GAME TYPES
    // INVALID_GAME_TYPE,

};

// TODO: CHANGE SERVER CODE TO SEND ENUM INSTEAD OF STRINGS (REMOVE THIS MAP)
static std::map<std::string, HEADERS> header_map;

static void header_map_init()
{
    header_map["CONNECTED"] = CONNECTED;
    header_map["REGISTRATION"] = REGISTRATION;
    header_map["REGISTER PENDING"] = REGISTER_PENDING;
    header_map["REGISTER SUCCESS"] = REGISTER_SUCCESS;
    header_map["GAME CREATED"] = GAME_CREATED;
    header_map["GAME STARTED"] = GAME_STARTED;
    header_map["JOINED GAME"] = JOINED_GAME;
    header_map["MADE MOVE"] = MOVE_RESULT;
    header_map["DATABASE SUCCESS"] = DATABASE_SUCCESS;
    header_map["BOARD UPDATE"] = BOARD_UPDATE;
    header_map["LEFT GAME"] = LEAVE_GAME;
    header_map["POSITIONED SHIPS"] = POSITIONED_SHIPS;
    header_map["INVALID LAYOUT"] = INVALID_LAYOUT;
    header_map["TERMINATED REGISTER"] = TERMINATED_REGISTER;
}

void Websocket::start(void)
{
    header_map_init();

    this->dropped = 0;

    esp_websocket_client_config_t websocket_cfg = {};

    websocket_cfg.uri = WEBSOCKET_URI;

    ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);

    Websocket::client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(Websocket::client, WEBSOCKET_EVENT_ANY, event_handler, (void *)Websocket::client);

    esp_websocket_client_start(Websocket::client);

    // start task
    xTaskCreate(ws_connection_msg_task, "ws_connection_msg_task", 4096, NULL, 10, NULL);

    websocket.send(messenger.build_connected_msg(true));
}

void Websocket::stop(void)
{
    esp_websocket_client_close(Websocket::client, portMAX_DELAY);
    ESP_LOGI(TAG, "Websocket Stopped");
    esp_websocket_client_destroy(Websocket::client);

    header_map.clear();
}

/**
 * @param - msg: this must be the output of a Messenger.h function call!
 */
void Websocket::send(char *msg)
{
    // wrapper for wifi client send to ws
    size_t len = strlen(msg);
    ESP_LOGI(TAG, "Sending %s", msg);
    int res = esp_websocket_client_send_text(Websocket::client, msg, len, portMAX_DELAY);
    free(msg);

    if (res == ESP_FAIL)
    {
        this->dropped++;
        ESP_LOGW(TAG, "Dropped %d Packets", this->dropped);
    }
    else
    {
        this->dropped = 0;
    }

    if (this->dropped >= WS_PACKET_DROPPED_LIMIT)
    {
        esp_restart(); // connection failed, restart
    }
}

void Websocket::handle(const char *msg, uint8_t len)
{
    const cJSON *header = NULL;
    const cJSON *payload = NULL;
    const cJSON *meta = NULL;
    int status = -404;
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
    printf("header: \"%s\"\n", header->valuestring);

    meta = cJSON_GetObjectItemCaseSensitive(msg_json, "meta");

    payload = cJSON_GetObjectItemCaseSensitive(msg_json, "payload");

    switch (header_map[header->valuestring])
    {
    case CONNECTED:
    {
        printf("Connected to Websocket\n");
        status = HEADERS::CONNECTED;
        break;
    }
    case DATABASE_SUCCESS:
    {
        if (!cJSON_IsString(meta) || (meta->valuestring == NULL))
        {
            status = -1 * HEADERS::DATABASE_SUCCESS;
            goto end;
        }

        payload = cJSON_GetObjectItemCaseSensitive(msg_json, "payload");

        if (strcmp(INVITE_SENT_TAG, meta->valuestring))
        {
            // FIXME: THIS RELIES ON THERE BEING TWO DB MESSAGES, INVITE AND GET FRIENDS
            //  GET_FRIENDS
            friendManager.update(payload);
            screenManager.setState(FRIENDS_LIST);
        }
        else
        {
            // INVITE FRIEND
            screenManager.splash(INVITE_SENT);
        }
        status = HEADERS::DATABASE_SUCCESS;
        break;
    }
    case BOARD_UPDATE:
    {
        if (!cJSON_IsString(meta) || (meta->valuestring == NULL))
        {
            status = -1 * HEADERS::BOARD_UPDATE;
            goto end;
        }

        ledManager.update(meta->valuestring);
        status = HEADERS::BOARD_UPDATE;

        break;
    }
    case REGISTRATION:
    {
        // TODO: NEEDS IMPLEMENTATION
        status = HEADERS::REGISTRATION;
        break;
    }
    case TERMINATED_REGISTER:
    {
        screenManager.setState(INIT_PAIRING);
        status = HEADERS::TERMINATED_REGISTER;
        break;
    }
    case REGISTER_PENDING:
    {
        if (strcmp(meta->valuestring, REGISTRATION_WAITING_FOR_WEB_TAG))
        {
            if (screenManager.getState() == WAITING_PAIRING)
            {
                screenManager.setState(CONFIRM_PAIRING);
            }
            else
            {
                printf("Error: tried to confirm pairing when not requested");
            }
            status = HEADERS::REGISTER_PENDING;
        }
        else
        {
            status = -2;
        }

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
            strncpy(settings.username, username->valuestring, SETTING_STR_LEN::USERNAME);
            settings.save();
            screenManager.splash(DEVICE_PAIRED, REBOOT);
            vTaskDelay(2000 / portTICK_RATE_MS);
            esp_restart();
        }
        else
        {
            status = -1;
            goto end;
        }
        status = HEADERS::REGISTER_SUCCESS;
        break;
    }

    case REGISTER_ERROR:
    {
        // TODO: NEEDS IMPLEMENTATION
        status = HEADERS::REGISTER_ERROR;
        break;
    }
    case GAME_CREATED:
    {
        gameState.setState(LOBBY);
        screenManager.setState(INVITE_FRIEND);
        status = HEADERS::GAME_CREATED;
        break;
    }
    case GAME_STARTED:
    {
        gameState.setState(IN_PROGRESS);
        screenManager.setState(ATTACK);
        status = HEADERS::GAME_STARTED;
        break;
    }
    case LEAVE_GAME:
    {
        gameState.reset();
        gameState.setState(SETUP);
        screenManager.splash(LEFT_GAME, CREATE_GAME);
        break;
    }
    case MOVE_RESULT:
    {
        if (!cJSON_IsObject(payload))
        {
            status = 505;
            goto end;
        }

        cJSON *move_c = cJSON_GetObjectItemCaseSensitive(payload, "c");
        cJSON *move_r = cJSON_GetObjectItemCaseSensitive(payload, "r");
        cJSON *to = cJSON_GetObjectItemCaseSensitive(payload, "to");
        cJSON *result = cJSON_GetObjectItemCaseSensitive(payload, "result");
        // cJSON *result_ship = cJSON_GetObjectItemCaseSensitive(payload, "result_ship");
        if (cJSON_IsNumber(move_c) && cJSON_IsNumber(move_r) && cJSON_IsString(to) && (to->valuestring != NULL) && cJSON_IsString(result) && (result->valuestring != NULL) && (cJSON_IsString(meta) && (meta->valuestring != NULL)))
        {
            printf("C: \"%d\", R: \"%d\", to: \"%s\", result: \"%s\"\n", move_c->valueint, move_r->valueint, to->valuestring, result->valuestring);
            gameState.moveReceived(move_c->valueint, move_r->valueint, to->valuestring, meta->valuestring);

            if (!strcmp(SHIP_SUNK_TAG, result->valuestring) || !strcmp(SHIP_HIT_TAG, result->valuestring))
            {
                // rumble hits/sinks reguardless of player
                motorManager.rumble();
            }

            // check if game is over
            if (!strcmp(GAME_OVER_WINNER_TAG, meta->valuestring) || !strcmp(GAME_OVER_LOSER_TAG, meta->valuestring))
            {
                gameState.reset();
                gameState.setState(SETUP);
                screenManager.splash(GAME_OVER, CREATE_GAME);
                break;
            }
            screenManager.splash(MOVE_MADE);
        }
        else
        {
            status = -1;
            goto end;
        }

        status = HEADERS::MOVE_RESULT;
        break;
    }
    case INVALID_MOVE:
    {
        // TODO: ADD META INFO TO GAME STATE HERE

        screenManager.splash(NOTIFY_INVALID_MOVE);
        status = HEADERS::INVALID_MOVE;
        break;
    }
    case JOINED_GAME:
    {
        if (!cJSON_IsObject(payload))
        {
            status = 505;
            goto end;
        }

        cJSON *opponent = cJSON_GetObjectItemCaseSensitive(payload, "opponent");

        if (cJSON_IsString(opponent) && (opponent->valuestring != NULL))
        {
            printf("opponent: \"%s\"\n", opponent->valuestring);

            gameState.setState(LOBBY);

            if (strcmp(JOINED_EMPTY_GAME_TAG, opponent->valuestring))
            {
                printf("Two players in lobby\n");
                strncpy(gameState.opponent, opponent->valuestring, SETTING_STR_LEN::USERNAME);
                screenManager.splash(PLAYER_IN_LOBBY, READY_UP_SHIPS);
            }
            else
            {
                printf("Joined Own Game\n");
                screenManager.setState(INVITE_FRIEND);
            }
        }
        else
        {
            status = -1;
            goto end;
        }

        status = HEADERS::JOINED_GAME;
        break;
    }
    case POSITIONED_SHIPS:
    {
        screenManager.splash(NOTIFY_POSITION_SHIPS);
        status = HEADERS::POSITIONED_SHIPS;
        break;
    }
    case INVALID_LAYOUT:
    {
        screenManager.splash(INVALID_SHIP_LAYOUT);
        status = HEADERS::INVALID_LAYOUT;
        break;
    }
    }

end:
    cJSON_Delete(msg_json);
    // return status;
    printf("status: %d\n", status);
}