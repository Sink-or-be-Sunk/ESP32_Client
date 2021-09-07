#include "WebSocketMsg.h"

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
