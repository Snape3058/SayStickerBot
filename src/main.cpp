#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <tgbot/tgbot.h>
#include <ArtRobot/ArtRobot.h>

#include "Global.h"
#include "Log.h"
#include "UsersData.h"
#include "MakeSticker.h"
#include "InlineQuery.h"
#include "Tg.h"
#include "StringCheck.h"

using namespace std;
using namespace cv;
using namespace TgBot;

std::string botUsername;
std::string botUsernameLowercase;
int32_t botId = 0;
UsersData *usersData;

int main()
{
    cout << "====================" << endl
         << "|  SayStickerBot!  |" << endl
         << "====================" << endl;

    usersData = new UsersData();

    string token = getenv("TOKEN");
    Bot bot(token);

    bot.getEvents().onAnyMessage([&bot](Message::Ptr message) { // 处理收到的直接消息
        LogI("Message: chat->username=%s, chat->id=%lld, text=%s", message->chat->username.c_str(), message->chat->id, message->text.c_str());

        auto &api = bot.getApi();
        auto chatId = message->chat->id;

        if (message->forwardDate) // 是转发的消息
        {
            if (message->forwardFrom)
            {
                string &fromUsername = message->forwardFrom->username;
                int fromUserId = message->forwardFrom->id;
                string &content = message->text;
                LogV("Forward message: fromUsername=%s, fromUserId=%d, content=%s",
                     fromUsername.c_str(),
                     fromUserId,
                     content.c_str());

                if (fromUserId == botId)
                {
                    sendMessage(api, chatId, "哼～ (┙>∧<)┙彡 ┻━┻"); // 禁止收录自己
                    return;
                }

                auto ret = usersData->searchByUserIdAndContent(fromUserId, content); // 搜索是否已经存在
                if (ret.size() == 0)
                { // 不存在
                    auto stickerFileId = MakeSticker(api, chatId, message->forwardFrom, content, message->from->id);
                    if (stickerFileId.length())
                    { // 成功
                        usersData->add(message->forwardFrom->id, message->forwardFrom->username, message->text, stickerFileId);
                        sendSticker(api, chatId, stickerFileId);
                    }
                }
                else
                {                                            // 存在
                    sendSticker(api, chatId, ret[0].fileId); // 直接返回
                }
            }
            else
            { // 被转发用户的隐私设置原因无法获取uid
                sendMessage(api, chatId, "该用户的隐私设置不允许转发。");
            }
            return;
        }

        if (message->text.c_str()[0] == '/') // 如果是指令则跳过
            return;

        if (message->chat->type == Chat::Type::Private) // 只有私聊显示help
            sendMessage(api, chatId, "如果想要什么帮助的话请给我发 /help");
    });

    bot.getEvents().onCommand("help", [&bot](Message::Ptr message) { // /help
        auto &api = bot.getApi();
        auto chatId = message->chat->id;

        if (message->chat->type == Chat::Type::Private)
        { // 私聊
            sendMessage(api, chatId, "请转发消息给我。");
        } // 不支持群组
    });

    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) { // /start
        auto &api = bot.getApi();
        auto chatId = message->chat->id;

        sendMessage(api, chatId, "欢迎使用贴纸语录机器人。");
    });

    bot.getEvents().onCommand("list", [&bot](Message::Ptr message) { // /list
        auto &api = bot.getApi();
        auto chatId = message->chat->id;

        string retStr;
        if (message->text == "/list")
        {
            auto ret = usersData->searchByUserId(message->from->id);
            stringstream ss;
            for (auto c : ret)
            {
                ss << c.id << " " << c.content << "\n";
            }
            retStr = ss.str();
        }

        sendMessage(api, chatId, retStr);
    });

    bot.getEvents().onUnknownCommand([&bot](Message::Ptr message) { // 未知指令
        auto &api = bot.getApi();
        auto chatId = message->chat->id;

        if (message->chat->type == Chat::Type::Private)
        { // 私聊
            sendMessage(api, chatId, "你在说什么？\n如果想要什么帮助的话请给我发 /help");
        }
    });

    bot.getEvents().onInlineQuery([&bot](InlineQuery::Ptr inlineQuery) {
        auto &query = inlineQuery->query;

        LogI("InlineQuery: %s: %s", inlineQuery->from->username.c_str(), query.c_str());

        vector<InlineQueryResult::Ptr> results; // 准备results

        pushStickerOnInlineQuery(bot.getApi(), results, query);

        // debug json
        // TgTypeParser tgTypeParser;
        // cout << tgTypeParser.parseArray<InlineQueryResult>(&TgTypeParser::parseInlineQueryResult, results) << endl;

        try
        {
            bot.getApi().answerInlineQuery(inlineQuery->id, results);
        }
        catch (TgException &e)
        {
            LogE("answerInlineQuery: %s", e.what());
        }
    });

    while (true)
    {
        try
        {
            LogI("Starting ...");
            botUsernameLowercase = botUsername = bot.getApi().getMe()->username;
            lowercase(botUsernameLowercase);
            botId = bot.getApi().getMe()->id;
            LogI("Bot username: %s %d", botUsername.c_str(), botId);

            TgLongPoll longPoll(bot);
            while (true)
            {
                LogI("Long poll started.");
                longPoll.start();
            }

            // TgWebhookTcpServer webhookServer(8888, bot);
            // string webhookUrl(getenv("WEBHOOK_URL"));
            // bot.getApi().setWebhook(webhookUrl);
            // webhookServer.start();
        }
        catch (TgException &e)
        {
            LogE("Error: %s", e.what());
        }
        catch (...)
        {
            LogE("Unknow error.");
        }
        LogI("Restart.");
    }

    return 0;
}
