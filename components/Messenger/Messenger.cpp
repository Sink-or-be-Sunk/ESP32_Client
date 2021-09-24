#include "Messenger.h"

#define REGISTRATION_TAG "REGISTRATION"
#define REGISTRATION_ENQUEUE_TAG "ENQUEUE"
#define REGISTRATION_CONFIRM_TAG "CONFIRM"

Messenger messenger; //singleton instance of class

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
    char *string = NULL; //point to output (built) string
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

    // id = cJSON_CreateString("PLAYER ID");
    char device_id[13];
    get_device_id(device_id);
    id = cJSON_CreateString(device_id);
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
