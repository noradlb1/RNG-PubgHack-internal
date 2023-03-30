#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/hex.h>
#include <cryptopp/ccm.h>
#include <curl/curl.h>
#include <string>
#include <ctime>
#include <random>
#include <sstream>
#include <nlohmann/json.hpp>
#include <ThemidaSDK.h>
#include "../xor.hpp"
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "urlmon.lib")
#ifndef r_auth_h
#define r_auth_h
nlohmann::json response_decoder;;
char* crashshit = NULL;
typedef int(__stdcall* pMessageBoxTimeoutA)(HWND, LPCSTR, LPCSTR, UINT, WORD, DWORD);
auto MessageBoxTimeoutA = (pMessageBoxTimeoutA)GetProcAddress(GetModuleHandleA(xorstr_("user32.dll")), xorstr_("MessageBoxTimeoutA"));

//extern "C" __declspec(dllimport) int MessageBoxTimeoutA(HWND,LPCSTR,LPCSTR,UINT,WORD,DWORD);
namespace r_auth {
	char enc_key[33];
	namespace r_encryption {
		void replace_all(std::string& data, std::string toSearch, std::string replaceStr)
		{
			size_t pos = data.find(toSearch);
			while (pos != std::string::npos)
			{
				data.replace(pos, toSearch.size(), replaceStr);
				pos = data.find(toSearch, pos + replaceStr.size());
			}
		}
		inline std::string encrypt_string(const std::string& str_in, const std::string& iv) {
			std::string str_out;
			try {
				CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryption((CryptoPP::byte*)enc_key, strlen(enc_key), (CryptoPP::byte*)iv.c_str());

				CryptoPP::StringSource encryptor(str_in, true,
					new CryptoPP::StreamTransformationFilter(encryption,
						new CryptoPP::Base64Encoder(
							new CryptoPP::StringSink(str_out),
							false
						)
					)
				);
			}
			catch (CryptoPP::Exception ex) {
				MessageBoxTimeoutA(0, xorstr_("Unknown exception!"), xorstr_("ERROR CODE: 100"), MB_ICONERROR, 0, 5000);
				exit(0);
			}
			replace_all(str_out, xorstr_("+"), xorstr_("-"));
			replace_all(str_out, xorstr_("/"), xorstr_("_"));
			replace_all(str_out, xorstr_("="), xorstr_(""));
			return iv + str_out;
		}

		inline std::string decrypt_string(std::string str_in, std::string iv) {
			std::string str_out;
			try {
				CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryption((CryptoPP::byte*)enc_key, strlen(enc_key), (CryptoPP::byte*)iv.c_str());

				CryptoPP::StringSource decryptor(str_in, true,
					new CryptoPP::Base64Decoder(
						new CryptoPP::StreamTransformationFilter(decryption,
							new CryptoPP::StringSink(str_out)
						)
					)
				);
			}
			catch (CryptoPP::Exception ex) {
				MessageBoxTimeoutA(0, xorstr_("Unknown exception!"), xorstr_("ERROR CODE: 101"), MB_ICONERROR, 0, 5000);
				exit(0);
			}
			return str_out;
		}

		std::string random_string(int len)
		{
			static const std::string allowed_chars{ xorstr_("ABCDFGHJKLMNPQRSTVWXZbcdfghjklmnpqrstvwxz") };
			static thread_local std::default_random_engine randomEngine(std::random_device{}());
			static thread_local std::uniform_int_distribution<int> randomDistribution(0, allowed_chars.size() - 1);
			std::string id(len ? len : 32, '\0');
			for (std::string::value_type& c : id) {
				c = allowed_chars[randomDistribution(randomEngine)];
			}
			return id;
		}
		inline std::string encrypt(std::string message) {
			return encrypt_string(message, random_string(16));
		}

		inline std::string decrypt(std::string message) {
			return decrypt_string(message.substr(16).c_str(), message.substr(0, 16));
		}
	}
	namespace r_utils {
		inline std::vector<std::string> split(const std::string& str, const char* separator)
		{
			std::vector<std::string> output;
			std::string::size_type prev_pos = 0, pos = 0;
			while ((pos = str.find(separator, pos)) != std::string::npos)
			{
				auto substring(str.substr(prev_pos, pos - prev_pos));
				output.push_back(substring);
				prev_pos = ++pos;
			}
			output.push_back(str.substr(prev_pos, pos - prev_pos));
			return output;
		}
		inline std::string get_username()
		{
			char acUserName[100];
			DWORD nUserName = sizeof(acUserName);
			GetUserNameA(acUserName, &nUserName);
			return acUserName;
		}
		inline std::string get_hwid() {
			HANDLE h = CreateFileA(xorstr_("\\\\.\\PhysicalDrive0"), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (h == INVALID_HANDLE_VALUE) return {};
			std::unique_ptr<std::remove_pointer<HANDLE>::type, void(*)(HANDLE)> hDevice{ h, [](HANDLE handle) {CloseHandle(handle); } };
			STORAGE_PROPERTY_QUERY storagePropertyQuery{};
			storagePropertyQuery.PropertyId = StorageDeviceProperty;
			storagePropertyQuery.QueryType = PropertyStandardQuery;
			STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader{};
			DWORD dwBytesReturned = 0;
			if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
				&storageDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER), &dwBytesReturned, NULL))
				return {};
			const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
			std::unique_ptr<BYTE[]> pOutBuffer{ new BYTE[dwOutBufferSize]{} };
			if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
				pOutBuffer.get(), dwOutBufferSize, &dwBytesReturned, NULL))
				return {};
			STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(pOutBuffer.get());
			const DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
			if (dwSerialNumberOffset == 0) return {};
			std::string serialNumber = reinterpret_cast<const char*>(pOutBuffer.get() + dwSerialNumberOffset);
			r_encryption::replace_all(serialNumber, " ", "");
			return serialNumber;
		}
		bool isASCII(const std::string& s)
		{
			return !std::any_of(s.begin(), s.end(), [](char c) {
				return static_cast<unsigned char>(c) > 127;
				});
		}
	}

	namespace r_userdata {
		int expiry = 0;
		std::string expirytime;
		HANDLE expirethread;
	}
	std::string post_datax = xorstr_("");

	namespace r_api {
		inline size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
			((std::string*)userp)->append((char*)contents, size * nmemb);
			return size * nmemb;
		}
		void checkifExpired()
		{
			VM_START
				while (1)
				{
					Sleep(120000);
					if (r_userdata::expiry < std::time(0))
					{
						ExitProcess(1);
					}
					//CURL* c_url = curl_easy_init();
					//CURLcode code;
					//std::string to_return;
					//if (c_url) {
					//	curl_easy_setopt(c_url, CURLOPT_URL, xorstr_("https://akcheat.net/api/CUser"));
					//	curl_easy_setopt(c_url, CURLOPT_USERAGENT, xorstr_("RAZEVIP/1.0"));
					//	curl_easy_setopt(c_url, CURLOPT_NOPROXY, xorstr_("akcheat.net"));
					//	curl_easy_setopt(c_url, CURLOPT_SSL_VERIFYPEER, 0);
					//	curl_easy_setopt(c_url, CURLOPT_SSL_VERIFYHOST, 0);
					//	curl_easy_setopt(c_url, CURLOPT_PINNEDPUBLICKEY, xorstr_("sha256//axAjpGrMopwEiQVPR0QbVWdFOn6JKGo5erveKwvnQ9Y="));
					//	curl_easy_setopt(c_url, CURLOPT_POSTFIELDSIZE, post_datax.size());
					//	curl_easy_setopt(c_url, CURLOPT_POSTFIELDS, post_datax.c_str());
					//	curl_easy_setopt(c_url, CURLOPT_WRITEFUNCTION, write_callback);
					//	curl_easy_setopt(c_url, CURLOPT_WRITEDATA, &to_return);
					//	curl_easy_setopt(c_url, CURLOPT_FRESH_CONNECT, true);
					//	code = curl_easy_perform(c_url);
					//	if (code != CURLE_OK)
					//	{
					//		exit(0);
					//	}
					//	if (to_return.find(xorstr_("false")) != std::string::npos)
					//		exit(0);
					//	curl_easy_cleanup(c_url);
					//}
				}
			VM_END
		}
		int integritycheck1;
		static std::time_t string_to_timet(std::string timestamp) {
			auto cv = strtol(timestamp.c_str(), NULL, 10); // long

			return (time_t)cv;
		}

		static std::tm timet_to_tm(time_t timestamp) {
			std::tm context;

			localtime_s(&context, &timestamp);

			return context;
		}
		static std::string tm_to_readable_time(tm ctx) {
			char buffer[25];

			strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M", &ctx);

			return std::string(buffer);
		}
		static std::string do_request(std::string post_data) {
			VM_START
				STR_ENCRYPT_START
				CURL* c_url = curl_easy_init();
			CURLcode code;
			std::string to_return;
			if (c_url) {
				curl_easy_setopt(c_url, CURLOPT_URL, "https://akcheat.net/api/Login");
				curl_easy_setopt(c_url, CURLOPT_USERAGENT, "RAZEVIP/1.0");
				curl_easy_setopt(c_url, CURLOPT_NOPROXY, "akcheat.net");
				curl_easy_setopt(c_url, CURLOPT_SSL_VERIFYPEER, 0);
				curl_easy_setopt(c_url, CURLOPT_SSL_VERIFYHOST, 0);
				curl_easy_setopt(c_url, CURLOPT_PINNEDPUBLICKEY, "sha256//yiP50g5UIqkd9eSDGLf6ScBx6MEUQOmlwlodORkNBsM=");
				curl_easy_setopt(c_url, CURLOPT_POSTFIELDSIZE, post_data.size());
				curl_easy_setopt(c_url, CURLOPT_POSTFIELDS, post_data.c_str());
				curl_easy_setopt(c_url, CURLOPT_WRITEFUNCTION, write_callback);
				curl_easy_setopt(c_url, CURLOPT_WRITEDATA, &to_return);
				curl_easy_setopt(c_url, CURLOPT_FRESH_CONNECT, true);
				code = curl_easy_perform(c_url);
				if (code != CURLE_OK)
				{
					MessageBoxTimeoutA(0, "Unknown exception!", "ERROR CODE: 102", MB_ICONERROR, 0, 5000);
					exit(0);
				}
				curl_easy_cleanup(c_url);
			}
			STR_ENCRYPT_END
				VM_END
				return to_return;
		}
		void saveKey(const char* key)
		{
			FILE* fp;
			fopen_s(&fp, xorstr_("C:\\sxvip.ini"), "w+");
			if (fp)
			{
				fprintf_s(fp, key);
				fclose(fp);
			}
		}
		std::string savedKey()
		{
			char line[33] = { 0 };
			FILE* fp;
			fopen_s(&fp, xorstr_("C:\\sxvip.ini"), "r+");
			if (fp)
			{
				fgets(line, 33, fp);
				fclose(fp);
			}
			return line;
		}
////#define NOLOGIN
//		inline bool r_login(std::string subkey, const char* secret_key) {
//#ifdef NOLOGIN
//			r_userdata::expiry = 0x7FFFFFFF;
//			CHECK_CODE_INTEGRITY(r_api::integritycheck1, 0x35796538);
//			r_api::integritycheck1 = 0x35796538;
//			r_userdata::expirytime = "Never";
//			return true;
//#endif // NOLOGIN
//
//			/*VM_START
//				STR_ENCRYPT_START
//				std::string session_id;
//			const char* version = "3.6";
//			subkey.erase(remove_if(subkey.begin(), subkey.end(), [](char c) { return isspace(c); }), subkey.end());
//			auto savedkey = savedKey();
//			if (savedkey.length() == 32)
//				subkey = savedkey;
//			std::string result;
//			std::string session_key = r_encryption::random_string(32);
//			char values[512];
//			memcpy(enc_key, "YUb6VQg9TBsYEPUBFqXeKxr6X3lyRSyg", 32);
//			sprintf(values, "pcname=%s&hwid=%s&subkey=%s&secret_key=%s&hs=%s", r_encryption::encrypt(r_utils::get_username()).c_str(), r_encryption::encrypt(r_utils::get_hwid()).c_str(), r_encryption::encrypt(subkey).c_str(), r_encryption::encrypt(secret_key).c_str(), r_encryption::encrypt(session_key).c_str());
//			result = do_request(values);
//			post_datax = values;
//			result = r_encryption::decrypt(result);
//			auto decoded_response = response_decoder.parse(result);
//			result = decoded_response["result"];
//			if (result == "success") {
//				if (decoded_response["sessionkey"] == session_key)
//				{
//					r_userdata::expiry = std::atoi(std::string(decoded_response["keytime"]).c_str());
//					CHECK_CODE_INTEGRITY(r_api::integritycheck1, 0x35796538);
//					if (r_api::integritycheck1 != 0x35796538)
//						crashshit[0xFFFFFFFF]--;
//					if (r_userdata::expiry > 1738528005)
//						r_userdata::expirytime = xorstr_("Never");
//					else
//						r_userdata::expirytime = tm_to_readable_time(timet_to_tm(string_to_timet(decoded_response["keytime"])));
//					r_userdata::expirethread = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(checkifExpired), nullptr, NULL, nullptr);
//					saveKey(subkey.c_str());
//					return true;
//				}
//				else
//				{
//					MessageBoxTimeoutA(0, "Unknown exception!", "ERROR CODE: 104", MB_ICONERROR, 0, 5000);
//					ExitProcess(1);
//				}
//			}
//			else if (result == "key_invalid") {
//				MessageBoxTimeoutA(0, "ERROR : invalid Key!", "ERROR", MB_ICONERROR, 0, 5000);
//				ExitProcess(1);
//			}
//			else if (result == "wrong_hwid") {
//				MessageBoxTimeoutA(0, "ERROR : Your HWID does not match!", "ERROR", MB_ICONERROR, 0, 5000);
//				ExitProcess(1);
//			}
//			else if (result == "key_expired") {
//				MessageBoxTimeoutA(0, "ERROR : This Key Expired!", "ERROR", MB_ICONERROR, 0, 5000);
//				ExitProcess(1);
//			}
//			else if (result == "user_is_banned") {
//				MessageBoxTimeoutA(0, "ERROR : This Key/Pc Banned!", "ERROR", MB_ICONERROR, 0, 5000);
//				ExitProcess(1);
//			}
//			else if (result == "login_disabled") {
//				MessageBoxTimeoutA(0, "Login Closed At This Time Please Try Again Later", "ERROR", MB_ICONERROR, 0, 5000);
//				ExitProcess(1);
//			}
//			else
//			{
//				MessageBoxTimeoutA(0, "Unknown exception!", "ERROR CODE: 105", MB_ICONERROR, 0, 5000);
//				ExitProcess(1);
//			}
//			return false;
//			ExitProcess(1);
//			STR_ENCRYPT_END
//				VM_END*/
//		}
	}
}

#endif