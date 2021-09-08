#include "WebSocketMsg.h"

static void get_device_id(char *service_name)
{
    const size_t max = 13;
    uint8_t eth_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, max, "%02X%02X%02X%02X%02X%02X",
             eth_mac[0], eth_mac[1], eth_mac[2], 
             eth_mac[3], eth_mac[4], eth_mac[5]);
}

char *create_new_game_req(void)
{
    char *string = NULL; //point to output (built) string
    cJSON *id = NULL;
    cJSON *req = NULL;

    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL)
    {
        goto end;
    }

    id = cJSON_CreateString("PLAYER ID");
    //TODO: ADD DEVICE ID FUNCTION CALL HERE
    if (id == NULL)
    {
        goto end;
    }
    /* after creation was successful, immediately add it to the obj,
     * thereby transferring ownership of the pointer to it */
    cJSON_AddItemToObject(obj, "id", id);

    req = cJSON_CreateString("NEW GAME");
    if (req == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(obj, "req", req);

    string = cJSON_Print(obj);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print obj.\n");
    }

end:
    cJSON_Delete(obj);
    return string;
}
