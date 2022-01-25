#include "pch.h"
#include "Websocket.h"
#include "WebsocketDebuggerDlg.h"

CWebsocket::CWebsocket (CWebsocketDebuggerDlg *dlg)
{
	mMainDialog = dlg;

	mIsRun = false;
	mClientTLS = nullptr;
	mClientNON = nullptr;
	mConnectionPtrTLS = nullptr;
	mConnectionPtrNON = nullptr;

	mIsSecure = false;
	mIsSelfSigned = false;
}

CWebsocket::~CWebsocket ()
{

}

void CWebsocket::wsOnSocketInit (websocketpp::connection_hdl hdl)
{
	std::string _message = "[INFO] WebSocket has been initialized : " + ((mIsSecure) ? mClientTLS->get_con_from_hdl (hdl)->get_ec ().message () : mClientNON->get_con_from_hdl (hdl)->get_ec ().message ()) + "\n";

	postLogMessageANSI (_message);

#if defined (_DEBUG)
	OutputDebugStringA (_message.c_str ());
#endif
}

context_ptr CWebsocket::wsOnTlsInit (websocketpp::connection_hdl hdl)
{
	std::string _message;

	websocketpp_client_tls::connection_ptr _con = mClientTLS->get_con_from_hdl (hdl);
	context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context> (asio::ssl::context::tlsv1);

	try {
		asio::ssl::context::options _options = (mIsSelfSigned) ? asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3 | asio::ssl::context::single_dh_use : asio::ssl::context::default_workarounds;
		ctx->set_options (_options);
		_message = "[INFO] WebSocket TLS has been initialized : " + _con->get_ec ().message () + "\n";
	}
	catch (std::exception &e) {
		_message = "[ERROR] Thread start failed : " + std::string (e.what ()) + "\n";
	}

	postLogMessageANSI (_message);

	return ctx;
}

void CWebsocket::wsOnOpen (websocketpp::connection_hdl hdl)
{
	std::string _message = (mIsSecure) ? mClientTLS->get_con_from_hdl (hdl)->get_ec ().message () : mClientNON->get_con_from_hdl (hdl)->get_ec ().message ();
	std::string message = "[INFO] WebSocket opened : " + _message + "\n";

	postLogMessageANSI (message);

#if defined (_DEBUG)
	OutputDebugStringA (message.c_str ());
#endif
}

void CWebsocket::wsOnFail (websocketpp::connection_hdl hdl)
{
	int _code = (mIsSecure) ? mClientTLS->get_con_from_hdl (hdl)->get_local_close_code () : mClientNON->get_con_from_hdl (hdl)->get_local_close_code ();
	std::string _message = (mIsSecure) ? mClientTLS->get_con_from_hdl (hdl)->get_ec ().message () : mClientNON->get_con_from_hdl (hdl)->get_ec ().message ();
	std::string message = "[ERROR] WebSocket failed : " + _message + " (status code:" + std::to_string (_code) + ")\n";

	postLogMessageANSI (message);

#if defined (_DEBUG)
	OutputDebugStringA (message.c_str ());
#endif

	mMainDialog->PostMessage (WM_PUT_THREAD_MESSAGE, POSTMESSAGE_TYPE_STOP_WSTHREAD, 0);
	mIsRun = false;
	mMainDialog->PostMessage (WM_PUT_THREAD_MESSAGE, POSTMESSAGE_TYPE_RESET_CONTROLS, 0);
}

void CWebsocket::wsOnClose (websocketpp::connection_hdl hdl)
{
	int _code = (mIsSecure) ? mClientTLS->get_con_from_hdl (hdl)->get_local_close_code () : mClientNON->get_con_from_hdl (hdl)->get_local_close_code ();
	std::string _message = (mIsSecure) ? mClientTLS->get_con_from_hdl (hdl)->get_ec ().message () : mClientNON->get_con_from_hdl (hdl)->get_ec ().message ();
	std::string message = "[INFO] WebSocket closed : " + _message + " (status code:" + std::to_string (_code) + ")\n";

	postLogMessageANSI (message);

#if defined (_DEBUG)
	OutputDebugStringA (message.c_str ());
#endif

	mMainDialog->PostMessage (WM_PUT_THREAD_MESSAGE, POSTMESSAGE_TYPE_STOP_WSTHREAD, 0);
	mIsRun = false;
	mMainDialog->PostMessage (WM_PUT_THREAD_MESSAGE, POSTMESSAGE_TYPE_RESET_CONTROLS, 0);
}

void CWebsocket::wsOnMessageTLS (websocketpp::connection_hdl hdl, websocketpp_message_tls_ptr msg)
{
	_wsOnMessage (std::string (msg->get_payload ()));
}

void CWebsocket::wsOnMessageNON (websocketpp::connection_hdl hdl, websocketpp_message_non_ptr msg)
{
	_wsOnMessage (std::string (msg->get_payload ()));
}

void CWebsocket::_wsOnMessage (std::string &message)
{
	string_trim (message);
	message = string_replase_all (message, "\r\n", "\\n");
	message = string_replase_all (message, "\n", "\\n");
	message += "\n";

	postLogMessageUTF8 (message);

#if defined (_DEBUG)
	OutputDebugStringUTF8 (message.c_str ());
#endif
}

bool CWebsocket::wsOnPing (websocketpp::connection_hdl hdl, std::string s)
{
	std::string _message = "[INFO] Received a PING : " + ((mIsSecure) ? mClientTLS->get_con_from_hdl (hdl)->get_ec ().message () : mClientNON->get_con_from_hdl (hdl)->get_ec ().message ()) + " / [" + s + "]\n";

	postLogMessageANSI (_message);

#if defined (_DEBUG)
	OutputDebugStringA (_message.c_str ());
#endif
	return true;
}

void CWebsocket::postLogMessageUTF8 (std::string &message)
{
	postLogMessage (POSTMESSAGE_TYPE_UTF8, message);
}

void CWebsocket::postLogMessageANSI (std::string &message)
{
	postLogMessage (POSTMESSAGE_TYPE_ANSI, message);
}

void CWebsocket::postLogMessage (WPARAM wParam, std::string message)
{
	if (wParam != POSTMESSAGE_TYPE_UTF8 && wParam != POSTMESSAGE_TYPE_ANSI)
		return;
	char *_message = new char[message.length () + 1];
	memcpy (_message, message.c_str (), message.length () + 1);
	mMainDialog->PostMessage (WM_PUT_THREAD_MESSAGE, wParam, (LPARAM) _message);
}

std::string CWebsocket::wchar_to_utf8 (wchar_t *str)
{
	std::string result;
	int strSize = WideCharToMultiByte (CP_UTF8, 0, str, -1, nullptr, 0, nullptr, nullptr);
	char *pStr = new char[strSize];
	WideCharToMultiByte (CP_UTF8, 0, str, -1, pStr, strSize, 0, 0);
	result = pStr;
	delete pStr;

	return result;
}

std::string CWebsocket::string_replase_all (std::string &str, const std::string &from, const std::string &to)
{
	size_t _start_pos = 0;
	while ((_start_pos = str.find (from, _start_pos)) != std::string::npos) {
		str.replace (_start_pos, from.length (), to);
		_start_pos += to.length ();
	}
	return str;
}

void CWebsocket::string_trim (std::string &s)
{
	// ltrim
	s.erase (s.begin (), std::find_if (s.begin (), s.end (), [](unsigned char ch) {
		return !std::isspace (ch);
	})
	);
	// rtrim
	s.erase (std::find_if (s.rbegin (), s.rend (), [](unsigned char ch) {
		return !std::isspace (ch);
	}).base (), s.end ());
}

void CWebsocket::run (void)
{
	websocketpp_client_tls ws_client_tls;
	websocketpp_client_non ws_client_non;
	websocketpp::lib::error_code websocketpp_error_code;

	std::string _message;

	if (mIsSecure) {
		ws_client_tls.clear_access_channels (websocketpp::log::alevel::all);
		ws_client_tls.clear_error_channels (websocketpp::log::elevel::all);

		ws_client_tls.init_asio ();
		ws_client_tls.start_perpetual ();

		ws_client_tls.set_socket_init_handler (bind (&CWebsocket::wsOnSocketInit, this, ::_1));
		ws_client_tls.set_open_handler (bind (&CWebsocket::wsOnOpen, this, ::_1));
		ws_client_tls.set_close_handler (bind (&CWebsocket::wsOnClose, this, ::_1));
		ws_client_tls.set_fail_handler (bind (&CWebsocket::wsOnFail, this, ::_1));
		ws_client_tls.set_message_handler (bind (&CWebsocket::wsOnMessageTLS, this, ::_1, ::_2));
		ws_client_tls.set_tls_init_handler (bind (&CWebsocket::wsOnTlsInit, this, ::_1));
		//ws_client_tls.set_ping_handler (bind (&CWebsocket::wsOnPing, this, ::_1, ::_2));

		mConnectionPtrTLS = ws_client_tls.get_connection (mURL, websocketpp_error_code);
	}
	else {
		ws_client_non.clear_access_channels (websocketpp::log::alevel::all);
		ws_client_non.clear_error_channels (websocketpp::log::elevel::all);

		ws_client_non.init_asio ();
		ws_client_non.start_perpetual ();

		ws_client_non.set_socket_init_handler (bind (&CWebsocket::wsOnSocketInit, this, ::_1));
		ws_client_non.set_open_handler (bind (&CWebsocket::wsOnOpen, this, ::_1));
		ws_client_non.set_close_handler (bind (&CWebsocket::wsOnClose, this, ::_1));
		ws_client_non.set_fail_handler (bind (&CWebsocket::wsOnFail, this, ::_1));
		ws_client_non.set_message_handler (bind (&CWebsocket::wsOnMessageNON, this, ::_1, ::_2));
		//ws_client_non.set_ping_handler (bind (&CWebsocket::wsOnPing, this, ::_1, ::_2));

		mConnectionPtrNON = ws_client_non.get_connection (mURL, websocketpp_error_code);
	}

	if (websocketpp_error_code) {
		mClientTLS = nullptr;
		mClientNON = nullptr;

		_message = "[ERROR] WebSocket get connection failed : " + websocketpp_error_code.message () + "\n";

		postLogMessageANSI (_message);
#if defined (_DEBUG)
		OutputDebugStringA (_message.c_str ());
#endif
		mMainDialog->PostMessage (WM_PUT_THREAD_MESSAGE, POSTMESSAGE_TYPE_STOP_WSTHREAD, 0);
		mIsRun = false;
		mMainDialog->PostMessage (WM_PUT_THREAD_MESSAGE, POSTMESSAGE_TYPE_RESET_CONTROLS, 0);
	}
	else {
		_message = "[INFO] Connect to the server (" + mURL + ")\n";
		postLogMessageANSI (_message);

		if (mIsSecure) {
			if (mSubprotocol.length () > 0)
				mConnectionPtrTLS->add_subprotocol (mSubprotocol, websocketpp_error_code);
			for (std::map<std::string, std::string>::iterator iter = mCustomHeaders.begin (); iter != mCustomHeaders.end (); iter++)
				mConnectionPtrTLS->append_header (iter->first, iter->second);
			ws_client_tls.connect (mConnectionPtrTLS);
			mClientTLS = &ws_client_tls;
			ws_client_tls.run ();
		}
		else {
			if (mSubprotocol.length () > 0)
				mConnectionPtrNON->add_subprotocol (mSubprotocol, websocketpp_error_code);
			for (std::map<std::string, std::string>::iterator iter = mCustomHeaders.begin (); iter != mCustomHeaders.end (); iter++)
				mConnectionPtrNON->append_header (iter->first, iter->second);
			ws_client_non.connect (mConnectionPtrNON);
			mClientNON = &ws_client_non;
			ws_client_non.run ();
		}
	}
	if (mIsSecure)
		ws_client_tls.stop_perpetual ();
	else
		ws_client_non.stop_perpetual ();

	mConnectionPtrTLS = nullptr;
	mConnectionPtrNON = nullptr;
}

void CWebsocket::clearCustomHeaders (void)
{
	mCustomHeaders.clear ();
}

void CWebsocket::addCustomHeaders (CString key, CString value)
{
	mCustomHeaders.insert (std::pair<std::string, std::string> (wchar_to_utf8 (key.GetBuffer ()), wchar_to_utf8 (value.GetBuffer ())));
}

void CWebsocket::setURL (CString url)
{
	mURL = wchar_to_utf8 (url.GetBuffer ());
	mIsSecure = (std::strncmp (mURL.c_str (), "wss://", 6) == 0);
}

void CWebsocket::sendMessage (CString message)
{
	std::string _message;

	if (mIsSecure)
		mConnectionPtrTLS->send (wchar_to_utf8 (message.GetBuffer ()), websocketpp::frame::opcode::text);
	else
		mConnectionPtrNON->send (wchar_to_utf8 (message.GetBuffer ()), websocketpp::frame::opcode::text);
	_message = "[INFO] Sent message : " + wchar_to_utf8 (message.GetBuffer ());

	postLogMessageUTF8 (_message);
}

void CWebsocket::stop (void)
{
	if (mIsSecure) {
		if (mClientTLS) {
			if (!mClientTLS->stopped ())
				mClientTLS->stop ();
			mClientTLS = nullptr;
		}
	}
	else {
		if (mClientNON) {
			if (!mClientNON->stopped ())
				mClientNON->stop ();
			mClientNON = nullptr;
		}
	}
}
void CWebsocket::close (void)
{
	std::string _message;
	if ((mIsSecure == true && mConnectionPtrTLS == nullptr) || (mIsSecure == false && mConnectionPtrNON == nullptr)) {
		_message = "[ERROR] Connection pointer is null. Please try again in a few minutes.";
		postLogMessageANSI (_message);
	}
	else {
		websocketpp::session::state::value _state = (mIsSecure) ? mConnectionPtrTLS->get_state () : mConnectionPtrNON->get_state ();
		switch (_state) {
			case websocketpp::session::state::connecting:
				_message = "[ERROR] Now connecting to the server. Please try again in a few minutes.";
				postLogMessageANSI (_message);
				break;
			case websocketpp::session::state::closing:
				_message = "[ERROR] The connection is closing";
				postLogMessageANSI (_message);
				break;
			case websocketpp::session::state::closed:
				_message = "[ERROR] The connection is closed.";
				postLogMessageANSI (_message);
				break;
			case websocketpp::session::state::open:
			default:
				if (mIsSecure)
					mConnectionPtrTLS->close (websocketpp::close::status::normal, "Quitting");
				else
					mConnectionPtrNON->close (websocketpp::close::status::normal, "Quitting");
				break;
		}
	}
}