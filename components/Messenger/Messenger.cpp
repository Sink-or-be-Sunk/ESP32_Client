#include "Messenger.h"

static const char *TAG = "MESSENGER";

#define REGISTRATION_TAG "REGISTRATION"
#define REGISTRATION_ENQUEUE_TAG "ENQUEUE"
#define REGISTRATION_CONFIRM_TAG "CONFIRM"
#define REGISTRATION_DEQUEUE_TAG "DEQUEUE"

#define DATABASE_TAG "DATABASE"
#define DATABASE_INVITE_TAG "INVITE TO GAME"
#define DATABASE_GET_FRIENDS_TAG "GET FRIENDS"

#define GAME_NEW_GAME_TAG "NEW GAME"
#define GAME_MAKE_MOVE_TAG "MAKE MOVE"
#define GAME_POSITION_SHIPS_TAG "POSITION SHIPS"
#define GAME_JOIN_GAME_TAG "JOIN GAME"
#define DEVICE_INIT_CONNECTED_TAG "INIT CONNECTION"
#define DEVICE_CONNECTED_TAG "CONNECTED"
#define LEAVE_GAME_TAG "LEAVE GAME"

#define GAME_ATTACK_SOLO_TAG "SOLO"

Messenger messenger; // singleton instance of class

void Messenger::init(void)
{
    ESP_LOGI(TAG, "Initializing...");

    // FIXME: REMOVE THIS, NOT NEEDED

    ESP_LOGI(TAG, "Success");
}

char *Messenger::build_registration_msg(REGISTRATION_TYPE reg_type)
{
    char *string = NULL; // point to output (built) string
    cJSON *msg = NULL;   // main json wrapper object
    cJSON *id = NULL;    // unique device id (mac address)
    cJSON *req = NULL;   // request header
    cJSON *data = NULL;  // data: request object wrapper
    cJSON *type = NULL;  // REGISTER_TYPE: Register type header
    cJSON *ssid = NULL;  // wifi ssid of connected device

    msg = cJSON_CreateObject();
    if (msg == NULL)
    {
        goto end;
    }

    id = cJSON_CreateString(settings.username);
    if (id == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "id", id);

    req = cJSON_CreateString(REGISTRATION_TAG);
    if (req == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "req", req);

    data = cJSON_CreateObject();
    if (data == NULL)
    {
        goto end;
    }

    switch (reg_type)
    {
    case ENQUEUE:
    {
        type = cJSON_CreateString(REGISTRATION_ENQUEUE_TAG);
        break;
    }
    case CONFIRM:
    {
        type = cJSON_CreateString(REGISTRATION_CONFIRM_TAG);
        break;
    }
    case DEQUEUE:
    {
        type = cJSON_CreateString(REGISTRATION_DEQUEUE_TAG);
        break;
    }
    }
    if (type == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(data, "type", type);

    ssid = cJSON_CreateString(settings.ssid);
    if (ssid == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(data, "ssid", ssid);

    cJSON_AddItemToObject(msg, "data", data);

    string = cJSON_Print(msg);
    if (string == NULL)
    {
        ESP_LOGE(TAG, "Failed to print registration msg");
    }

end:
    cJSON_Delete(msg);
    return string;
}

char *Messenger::build_connected_msg(bool init)
{
    char *string = NULL; // point to output (built) string
    cJSON *msg = NULL;   // main json wrapper object
    cJSON *id = NULL;    // device username
    cJSON *req = NULL;   // request header

    msg = cJSON_CreateObject();
    if (msg == NULL)
    {
        goto end;
    }

    id = cJSON_CreateString(settings.username);
    if (id == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "id", id);

    if (init)
    {
        req = cJSON_CreateString(DEVICE_INIT_CONNECTED_TAG);
    }
    else
    {
        req = cJSON_CreateString(DEVICE_CONNECTED_TAG);
    }
    if (req == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "req", req);

    string = cJSON_Print(msg);
    if (string == NULL)
    {
        ESP_LOGE(TAG, "Failed to print connected msg");
    }

end:
    cJSON_Delete(msg);
    return string;
}

char *Messenger::build_game_msg(GAME_REQ_TYPE type, cJSON *data)
{
    char *string = NULL; // point to output (built) string
    cJSON *msg = NULL;   // main json wrapper object
    cJSON *id = NULL;    // unique device id (mac address)
    cJSON *req = NULL;   // request header

    msg = cJSON_CreateObject();
    if (msg == NULL)
    {
        goto end;
    }

    id = cJSON_CreateString(settings.username);
    if (id == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "id", id);

    switch (type)
    {
    case NEW_GAME:
    {
        req = cJSON_CreateString(GAME_NEW_GAME_TAG);
        // no data to add to msg
        break;
    }
    case MAKE_MOVE:
    {
        req = cJSON_CreateString(GAME_MAKE_MOVE_TAG);
        cJSON_AddItemToObject(msg, "data", data);
        break;
    }
    case POSITION_SHIPS:
    {
        req = cJSON_CreateString(GAME_POSITION_SHIPS_TAG);
        cJSON_AddItemToObject(msg, "data", data);
        break;
    }
    case JOIN_GAME_REQ:
    {
        req = cJSON_CreateString(GAME_JOIN_GAME_TAG);
        cJSON_AddItemToObject(msg, "data", data);
        break;
    }
    }
    if (req == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "req", req);

    string = cJSON_Print(msg);
    if (string == NULL)
    {
        ESP_LOGE(TAG, "Failed to print game msg");
    }

end:
    cJSON_Delete(msg);
    return string;
}

char *Messenger::build_new_game_msg(void)
{
    return build_game_msg(NEW_GAME, NULL);
}

char *Messenger::build_attack_msg(char r, char c, ATTACK_TYPE type, const char *to)
{
    cJSON *data = NULL;      // data: request object wrapper
    cJSON *row = NULL;       // row of move
    cJSON *col = NULL;       // col of move
    cJSON *json_type = NULL; // move type
    cJSON *json_to = NULL;   // player that is being attacked
    char r_str[2] = {r, 0};
    char c_str[2] = {c, 0};

    data = cJSON_CreateObject();
    if (data == NULL)
    {
        goto end;
    }

    row = cJSON_CreateString(r_str);
    if (row == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(data, "r", row);

    col = cJSON_CreateString(c_str);
    if (col == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(data, "c", col);

    switch (type)
    {
    case SOLO:
    {
        json_type = cJSON_CreateString(GAME_ATTACK_SOLO_TAG);
        break;
    }
    }
    cJSON_AddItemToObject(data, "type", json_type);

    json_to = cJSON_CreateString(to);
    if (json_to == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(data, "to", json_to);

end:
    return build_game_msg(MAKE_MOVE, data);
}

char *Messenger::build_join_game_msg(const char *game)
{
    cJSON *json_game = NULL; // game

    json_game = cJSON_CreateString(game);
    if (json_game == NULL)
    {
        goto end;
    }

end:
    return build_game_msg(JOIN_GAME_REQ, json_game);
}
static cJSON *build_one_ship_position(const char *t, uint8_t r, uint8_t c)
{
    cJSON *pos = NULL;  // position object
    cJSON *row = NULL;  // row of position
    cJSON *col = NULL;  // col of position
    cJSON *type = NULL; // ship type

    pos = cJSON_CreateObject();
    if (pos == NULL)
    {
        goto end;
    }

    row = cJSON_CreateNumber(r);
    if (row == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(pos, "r", row);

    col = cJSON_CreateNumber(c);
    if (col == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(pos, "c", col);

    type = cJSON_CreateString(t);
    if (type == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(pos, "t", type);
end:
    return pos;
}

char *Messenger::build_position_ships(void)
{
    cJSON *data = NULL; // data: request object wrapper
    cJSON *pos = NULL;  // position object
    uint8_t r1;
    uint8_t c1;
    uint8_t r2;
    uint8_t c2;

    data = cJSON_CreateArray();
    if (data == NULL)
    {
        goto end;
    }

    for (int i = PATROL; i <= CARRIER; i++)
    {
        shipManager.getShip((ship_position_t)i, &r1, &c1, &r2, &c2);

        for (int j = 0; j < 2; j++)
        {
            switch (i)
            {
            case PATROL:
            {
                pos = build_one_ship_position("P", (j == 0) ? r1 : r2, (j == 0) ? c1 : c2);
                break;
            }
            case SUBMARINE:
            {
                pos = build_one_ship_position("S", (j == 0) ? r1 : r2, (j == 0) ? c1 : c2);
                break;
            }
            case BATTLESHIP:
            {
                pos = build_one_ship_position("B", (j == 0) ? r1 : r2, (j == 0) ? c1 : c2);
                break;
            }
            case CARRIER:
            {
                pos = build_one_ship_position("C", (j == 0) ? r1 : r2, (j == 0) ? c1 : c2);
                break;
            }
            }
            if (pos == NULL)
            {
                goto end;
            }
            cJSON_AddItemToArray(data, pos);
        }
    }

end:
    return build_game_msg(POSITION_SHIPS, data);
}

char *Messenger::build_db_msg(DATABASE_REQ_TYPE type, const char *username)
{
    char *string = NULL;   // point to output (built) string
    cJSON *msg = NULL;     // main json wrapper object
    cJSON *id = NULL;      // device username
    cJSON *req = NULL;     // request header
    cJSON *data = NULL;    // data: request object wrapper
    cJSON *db_type = NULL; // DATABASE_REQ_TYPE: Database type header
    cJSON *db_data = NULL; // data for db sub-object

    msg = cJSON_CreateObject();
    if (msg == NULL)
    {
        goto end;
    }

    id = cJSON_CreateString(settings.username);
    if (id == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "id", id);

    req = cJSON_CreateString(DATABASE_TAG);
    if (req == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "req", req);

    data = cJSON_CreateObject();
    if (data == NULL)
    {
        goto end;
    }

    switch (type)
    {
    case INVITE:
    {
        db_type = cJSON_CreateString(DATABASE_INVITE_TAG);

        db_data = cJSON_CreateString(username);
        if (db_data == NULL)
        {
            goto end;
        }
        cJSON_AddItemToObject(data, "data", db_data);

        break;
    }
    case GET_FRIENDS:
    {
        db_type = cJSON_CreateString(DATABASE_GET_FRIENDS_TAG);
        break;
    }
    }
    if (db_type == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(data, "type", db_type);

    cJSON_AddItemToObject(msg, "data", data);

    string = cJSON_Print(msg);
    if (string == NULL)
    {
        ESP_LOGE(TAG, "Failed to print db msg");
    }

end:
    cJSON_Delete(msg);
    return string;
}

char *Messenger::build_leave_game_msg()
{
    char *string = NULL; // point to output (built) string
    cJSON *msg = NULL;   // main json wrapper object
    cJSON *id = NULL;    // device username
    cJSON *req = NULL;   // request header

    msg = cJSON_CreateObject();
    if (msg == NULL)
    {
        goto end;
    }

    id = cJSON_CreateString(settings.username);
    if (id == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "id", id);

    req = cJSON_CreateString(LEAVE_GAME_TAG);
    if (req == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "req", req);

    string = cJSON_Print(msg);
    if (string == NULL)
    {
        ESP_LOGE(TAG, "Failed to print leave game msg");
    }

end:
    cJSON_Delete(msg);
    return string;
}
