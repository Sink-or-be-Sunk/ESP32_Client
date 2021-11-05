#include "Messenger.h"

#define REGISTRATION_TAG "REGISTRATION"
#define REGISTRATION_ENQUEUE_TAG "ENQUEUE"
#define REGISTRATION_CONFIRM_TAG "CONFIRM"

#define DATABASE_TAG "DATABASE"
#define DATABASE_INVITE_TAG "INVITE TO GAME"
#define DATABASE_GET_FRIENDS_TAG "GET FRIENDS"

#define GAME_NEW_GAME_TAG "NEW GAME"
#define GAME_MAKE_MOVE_TAG "MAKE MOVE"
#define GAME_POSITION_SHIPS_TAG "POSITION SHIPS"
#define GAME_JOIN_GAME_TAG "JOIN GAME"
#define DEVICE_CONNECTED_TAG "CONNECTED"

#define GAME_ATTACK_SOLO_TAG "SOLO"

Messenger messenger; // singleton instance of class

static void get_device_ssid(char result[SSID_MAX_LEN])
{
    wifi_ap_record_t ap;
    esp_wifi_sta_get_ap_info(&ap);
    snprintf(result, SSID_MAX_LEN, "%s", ap.ssid);
}

static void get_device_id(char *service_name)
{
    uint8_t eth_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, DEVICE_ID_MAX_LEN, "%02X%02X%02X%02X%02X%02X",
             eth_mac[0], eth_mac[1], eth_mac[2],
             eth_mac[3], eth_mac[4], eth_mac[5]);
}

void Messenger::init(void)
{
    get_device_id(this->device_id);
    get_device_ssid(this->device_ssid);
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

    char device_id[13];
    get_device_id(device_id);
    id = cJSON_CreateString(device_id);
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
    }
    if (type == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(data, "type", type);

    char device_ssid[SSID_MAX_LEN];
    get_device_ssid(device_ssid);
    ssid = cJSON_CreateString(device_ssid);
    if (ssid == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(data, "ssid", ssid);

    cJSON_AddItemToObject(msg, "data", data);

    string = cJSON_Print(msg);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print obj.\n");
    }

end:
    cJSON_Delete(msg);
    return string;
}

char *Messenger::build_connected_msg()
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

    req = cJSON_CreateString(DEVICE_CONNECTED_TAG);
    if (req == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(msg, "req", req);

    string = cJSON_Print(msg);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print obj.\n");
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
        fprintf(stderr, "Failed to print obj.\n");
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
        gameState.getShip((ship_position_t)i, &r1, &c1, &r2, &c2);

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
            printf("array size: %d\n", cJSON_GetArraySize(data));
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
        fprintf(stderr, "Failed to print obj.\n");
    }

end:
    cJSON_Delete(msg);
    return string;
}
