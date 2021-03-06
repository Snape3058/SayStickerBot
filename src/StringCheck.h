#pragma once

#include <tgbot/tgbot.h>

inline void fixUsername(std::string &username)
{
    if (username.c_str()[0] == '@') // 首位是@的话去掉
        username = username.c_str() + 1;
}

inline bool checkUsername(const std::string &username)
{
    for (auto ite = username.begin(); ite != username.end(); ite++)
    {
        if (
            !(
                (*ite >= '0' && *ite <= '9') ||
                (*ite >= 'a' && *ite <= 'z') ||
                (*ite >= 'A' && *ite <= 'Z') ||
                (*ite == '_')))
            return false;
    }
    return true;
}

inline void lowercase(std::string &str)
{
    transform(str.begin(), str.end(), str.begin(), ::tolower); // 用户名转小写
}

inline bool lowercaseEq(const std::string &__str1, const std::string &__str2)
{
    std::string str1 = __str1;
    std::string str2 = __str2;
    transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
    transform(str2.begin(), str2.end(), str2.begin(), ::tolower);
    return str1 == str2;
}

inline std::string getUsername(TgBot::User::Ptr &user)
{
    return user->username.empty() ? "user" + std::to_string(user->id) : user->username; // 如果用户名为空则用"user"+uid
}

inline std::string getStickerName(const std::string &username)
{
    return "Say_" + username + "_by_" + botUsername; // 贴纸名字
}

inline bool checkSelf(const std::string &username)
{
    if (lowercaseEq(username, botUsername))
        return false;
    return true;
}

inline bool checkSelf(uint64_t userId)
{
    if (userId == botId)
        return false;
    return true;
}
