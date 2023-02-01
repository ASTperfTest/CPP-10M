#ifndef _STEAM_REQUEST_MESSAGE_H_
#define _STEAM_REQUEST_MESSAGE_H_

class SendMessageRequest : public HttpRequest
{
public:
	SendMessageRequest(const char *token, const char *umqId, const char *steamId, const char *text) :
		HttpRequest(HttpPost, STEAM_API_URL "/ISteamWebUserPresenceOAuth/Message/v0001")
	{
		Content = new FormUrlEncodedContent(this)
			<< CHAR_PARAM("access_token", token)
			<< CHAR_PARAM("umqid", umqId)
			<< CHAR_PARAM("steamid_dst", steamId)
			<< CHAR_PARAM("type", "saytext")
			<< CHAR_PARAM("text", text);
	}
};

class SendTypingRequest : public HttpRequest
{
public:
	SendTypingRequest(const char *token, const char *umqId, const char *steamId) :
		HttpRequest(HttpPost, STEAM_API_URL "/ISteamWebUserPresenceOAuth/Message/v0001")
	{
		Content = new FormUrlEncodedContent(this)
			<< CHAR_PARAM("access_token", token)
			<< CHAR_PARAM("umqid", umqId)
			<< CHAR_PARAM("steamid_dst", steamId)
			<< CHAR_PARAM("type", "typing");
	}
};

#endif //_STEAM_REQUEST_MESSAGE_H_
