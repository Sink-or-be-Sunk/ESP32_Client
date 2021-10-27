#include "FriendManager.h"

FriendManager friendManager; // singleton instance of class

void FriendManager::update(const cJSON *list)
{
    const cJSON *item = NULL;
    int status = 0;
    listIdx = 0;

    cJSON_ArrayForEach(item, list)
    {
        cJSON *user = cJSON_GetObjectItemCaseSensitive(item, "user");
        cJSON *display = cJSON_GetObjectItemCaseSensitive(item, "display");

        if (!cJSON_IsString(user) || !cJSON_IsString(user))
        {
            status = -1;
            goto end;
        }

        if (user->valuestring == NULL || display->valuestring == NULL)
        {
            status = -2;
            goto end;
        }

        if (listIdx >= FRIEND_BUFFER_SIZE)
        {
            status = -3;
            goto end;
        }

        strncpy(uidList[listIdx], user->valuestring, SETTING_STR_LEN::USERNAME);
        strncpy(namesList[listIdx], display->valuestring, SETTING_STR_LEN::USERNAME);

        listIdx++;
    }

end:
    printf("Friend Manager Ended with status: %d\n", status);
}

const char *FriendManager::getCurDisplayName()
{
    return namesList[cur];
}

const char *FriendManager::getCurUsername()
{
    return uidList[cur];
}

/** returns false when no next allowed */
bool FriendManager::next()
{
    if (cur + 1 >= listIdx)
    {
        return false;
    }
    cur++;
    return true;
}

/** returns false when no prev allowed */
bool FriendManager::prev()
{
    if (cur - 1 < 0)
    {
        return false;
    }
    cur--;
    return true;
}
