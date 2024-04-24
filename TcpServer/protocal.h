#ifndef PROTOCAL_H
#define PROTOCAL_H

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define REGIST_OK "regist ok"                            // 注册
#define REGIST_FAILED "regist failed"

#define LOGIN_OK "login ok"                            // 登录
#define LOGIN_FAILED "login failed"

// 好友操作
#define SEARCH_USER_OK "search user ok"                  // 查找
#define SEARCH_USER_OFFLINE "user offline"
#define SEARCH_USER_EMPTY "no such people"

#define ADD_FRIEND_OK "add friend ok"                    // 添加好友 1对方存在在线，0对方存在不在线，2不存在，3已是好友，4请求错误
#define ADD_FRIEND_OFFLINE "added friend offline"
#define ADD_FRIEND_EMPTY "no such people"
#define ADD_FRIEND_EXIST "added friend already exist"

#define DEL_FRIEND_OK "delete friend ok"                 // 删除好友
#define DEL_FRIEND_FAILED "delete friend failed"

#define UNKNOWN_ERROR "unknown error"                    // 通用未知错误

#define DIR_NOT_EXIST "cur dir not exist"
#define FILE_NAME_EXIST "file name exist"
#define CREATE_DIR_OK "create dir ok"

using Uint = unsigned;

enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,

    ENUM_MSG_TYPE_REGIST_REQUEST,       // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,       // 注册回复

    ENUM_MSG_TYPE_LOGIN_REQUEST,        // 登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,        // 登录回复

    // 好友操作
    ENUM_MSG_TYPE_ONLINE_USERS_REQUEST, // 所有在线用户请求
    ENUM_MSG_TYPE_ONLINE_USERS_RESPOND, // 所有在线用户响应

    ENUM_MSG_TYPE_SEARCH_USER_REQUEST,  // 查找用户请求
    ENUM_MSG_TYPE_SEARCH_USER_RESPOND,  // 查找用户响应

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,   // 添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,   // 添加好友响应

    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,     // 被添加好友消息回复同意
    ENUM_MSG_TYPE_ADD_FRIEND_REJECT,    // 被添加好友消息回复拒绝

    ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST, // 刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND, // 刷新好友响应

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST, // 删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND, // 删除好友响应

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST, // 私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND, // 私聊回复

    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST, // 群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND, // 群聊回复

    // 文件操作
    ENUM_MSG_TYPE_CREATE_DIR_REQUEST, // 新建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND, // 新建文件夹回复

    ENUM_MSG_TYPE_FLUSH_DIR_REQUEST, // 刷新文件夹请求
    ENUM_MSG_TYPE_FLUSH_DIR_RESPOND, // 刷新文件夹回复

    ENUM_MSG_TYPE_DELETE_FILE_REQUEST, // 刷新文件夹请求
    ENUM_MSG_TYPE_DELETE_FILE_RESPOND, // 刷新文件夹回复

    ENUM_MSG_TYPE_RENAME_FILE_REQUEST, // 重命名文件夹请求
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND, // 重命名文件夹回复

    ENUM_MSG_TYPE_ENTRY_DIR_REQUEST, // 进入文件夹请求
    ENUM_MSG_TYPE_ENTRY_DIR_RESPOND, // 进入文件夹回复

    ENUM_MSG_TYPE_PRE_DIR_REQUEST, // 上一文件夹请求
    ENUM_MSG_TYPE_PRE_DIR_RESPOND, // 上一文件夹回复

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST, // 上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND, // 上传文件回复

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST, // 下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND, // 下载文件响应

    ENUM_MSG_TYPE_MOVE_FILE_REQUEST, // 移动文件请求
    ENUM_MSG_TYPE_MOVE_FILE_RESPOND, // 移动文件响应

    ENUM_MSG_TYPE_SHARE_FILE_REQUEST, // 移动文件请求
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND, // 移动文件响应
    ENUM_MSG_TYPE_SHARE_FILE_NOTE, // 移动文件提示
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND, // 移动文件提示响应

    ENUM_MSG_TYPE_MAX = 0x00ffffff, // uint最大值 0xffffffff
};

struct PDU{
    Uint uilPDULen;
    Uint UiMsgType;
    char caData[64];
    Uint uiMsgLen;
    int caMsg[];
};

PDU* mkPDU(Uint uiMsgLend);


#endif // PROTOCAL_H
