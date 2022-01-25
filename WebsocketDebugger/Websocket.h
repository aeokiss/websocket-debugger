#pragma once

#pragma warning (push)
#pragma warning (disable: 4267)
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#pragma warning (pop)

typedef websocketpp::client<websocketpp::config::asio_client> websocketpp_client_non;
typedef websocketpp::client<websocketpp::config::asio_tls_client> websocketpp_client_tls;
typedef websocketpp::config::asio_client::message_type::ptr websocketpp_message_non_ptr;
typedef websocketpp::config::asio_tls_client::message_type::ptr websocketpp_message_tls_ptr;
typedef websocketpp::lib::shared_ptr<asio::ssl::context> context_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

#define WM_PUT_THREAD_MESSAGE	(WM_USER + 100)

#define OutputDebugStringUTF8(_message) OutputDebugString(CW2T(CA2W((char*)_message,CP_UTF8)))
#define UTF8_TO_CSTRING(_str) ((CString) CW2T(CA2W((char*)_str,CP_UTF8)))

#define POSTMESSAGE_TYPE_RESET_CONTROLS		1
#define POSTMESSAGE_TYPE_STOP_WSTHREAD		2
#define POSTMESSAGE_TYPE_UTF8				11
#define POSTMESSAGE_TYPE_ANSI				12

class CWebsocketDebuggerDlg;

class CWebsocket
{
public:
	CWebsocket (CWebsocketDebuggerDlg *dlg);
	~CWebsocket ();

	void clearCustomHeaders (void);
	void addCustomHeaders (CString key, CString value);
	bool getIsRun (void) { return mIsRun; }
	void setIsRun (bool run) { mIsRun = run; }

	void setURL (CString url);
	CString getURL (void) {	return UTF8_TO_CSTRING (mURL.c_str ()); }
	void setSubprotocol (CString subprotocol) { mSubprotocol = wchar_to_utf8 (subprotocol.GetBuffer ()); }
	CString getSubprotocol (void) { return UTF8_TO_CSTRING (mSubprotocol.c_str ()); }

	void setSelfSigned (bool selfsigned) { mIsSelfSigned = selfsigned; }
	bool getSelfSigned (void) { return mIsSelfSigned; }

	void sendMessage (CString message);

	void run (void);

	void postLogMessageUTF8 (std::string &message);
	void postLogMessageANSI (std::string &message);
	void postLogMessage (WPARAM wParam, std::string message);

	void stop (void);
	void close (void);

private:
	CWebsocketDebuggerDlg *mMainDialog;

	std::thread mThream;
	websocketpp_client_tls *mClientTLS;
	websocketpp_client_non *mClientNON;
	websocketpp_client_tls::connection_ptr mConnectionPtrTLS;
	websocketpp_client_non::connection_ptr mConnectionPtrNON;

	std::string mURL;
	std::string mSubprotocol;
	bool mIsSecure;
	bool mIsSelfSigned;

	bool mIsRun;

	std::map<std::string, std::string> mCustomHeaders;

	void wsOnSocketInit (websocketpp::connection_hdl hdl);
	context_ptr wsOnTlsInit (websocketpp::connection_hdl hdl);
	void wsOnFail (websocketpp::connection_hdl hdl);
	void wsOnOpen (websocketpp::connection_hdl hdl);
	void wsOnMessageTLS (websocketpp::connection_hdl hdl, websocketpp_message_tls_ptr msg);
	void wsOnMessageNON (websocketpp::connection_hdl hdl, websocketpp_message_non_ptr msg);
	void _wsOnMessage (std::string &message);
	void wsOnClose (websocketpp::connection_hdl hdl);
	bool wsOnPing (websocketpp::connection_hdl hdl, std::string s);

	std::string wchar_to_utf8 (wchar_t *str);
	std::string string_replase_all (std::string &str, const std::string &from, const std::string &to);
	void string_trim (std::string &s);
};

