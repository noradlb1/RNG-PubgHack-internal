#pragma once

#ifndef c_auth_i
#define c_auth_i

#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/hex.h>
#include <cryptopp/ccm.h>
#include <string>
#include <codecvt>
#include <locale>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")
#include <atlsecurity.h>
#include <windows.h>
#include <string>
#include <ctime>
#include <tlhelp32.h>
#include "ChiperEncoder.h"
#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#pragma comment(lib, "rpcrt4.lib")

#include "Swifty_hor.hpp"

namespace c_auth
{
    char filename[MAX_PATH];
    void Msg(std::string Message, std::string Title, int Type)
    {
        MessageBoxA(0, Message.c_str(), Title.c_str(), Type);
    }
    class encryption
    {
      public:
        static std::string encrypt_string(const std::string &str_in, const std::string &key, const std::string &iv)
        {
            std::string str_out;
            CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryption((CryptoPP::byte *)key.c_str(), key.length(), (CryptoPP::byte *)iv.c_str());

            CryptoPP::StringSource encryptor(str_in, true,
                                             new CryptoPP::StreamTransformationFilter(encryption,
                                                                                      new CryptoPP::Base64Encoder(
                                                                                          new CryptoPP::StringSink(str_out),
                                                                                          false)));
            return str_out;
        }

        static std::string decrypt_string(const std::string &str_in, const std::string &key, const std::string &iv)
        {
            std::string str_out;

            CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryption((CryptoPP::byte *)key.c_str(), key.length(), (CryptoPP::byte *)iv.c_str());

            CryptoPP::StringSource decryptor(str_in, true,
                                             new CryptoPP::Base64Decoder(
                                                 new CryptoPP::StreamTransformationFilter(decryption,
                                                                                          new CryptoPP::StringSink(str_out))));
            return str_out;
        }

        static std::string sha256(const std::string &plain_text)
        {
            std::string hashed_text;
            CryptoPP::SHA256 hash;

            try
            {
                CryptoPP::StringSource hashing(plain_text, true,
                                               new CryptoPP::HashFilter(hash,
                                                                        new CryptoPP::HexEncoder(
                                                                            new CryptoPP::StringSink(hashed_text),
                                                                            false)));
            }
            catch (CryptoPP::Exception &ex)
            {
                std::thread Msgthread(&Msg, ex.what(), XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
            }

            return hashed_text;
        }

        static std::string hex_encode(const std::string &plain_text)
        {
            std::string encoded_text;

            try
            {
                CryptoPP::StringSource encoding(plain_text, true,
                                                new CryptoPP::HexEncoder(
                                                    new CryptoPP::StringSink(encoded_text),
                                                    false));
            }
            catch (CryptoPP::Exception &ex)
            {
                std::thread Msgthread(&Msg, ex.what(), XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
            }

            return encoded_text;
        }

        static std::string iv_key()
        {
            UUID uuid = {0};
            std::string guid;

            ::UuidCreate(&uuid);

            RPC_CSTR szUuid = NULL;
            if (::UuidToStringA(&uuid, &szUuid) == RPC_S_OK)
            {
                guid = (char *)szUuid;
                ::RpcStringFreeA(&szUuid);
            }

            return guid.substr(0, 8);
        }

        static std::string encrypt(std::string message, std::string enc_key, std::string iv)
        {
            enc_key = sha256(enc_key).substr(0, 32);

            iv = sha256(iv).substr(0, 16);
            return const_cast<char *>(encrypt_string(message, enc_key, iv).c_str());
        }

        static std::string decrypt(std::string message, std::string enc_key, std::string iv)
        {
            enc_key = sha256(enc_key).substr(0, 32);

            iv = sha256(iv).substr(0, 16);
            return decrypt_string(message, enc_key, iv);
        }
    };

    class utils
    {
      public:
        static std::vector<std::string> split(const std::string &str, const char separator)
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

        static std::string get_hwid()
        { // get user SID
            ATL::CAccessToken accessToken;
            ATL::CSid currentUserSid;
            if (accessToken.GetProcessToken(TOKEN_READ | TOKEN_QUERY) &&
                accessToken.GetUser(&currentUserSid))
                return std::string(CT2A(currentUserSid.Sid()));
        }

        static std::time_t string_to_timet(std::string timestamp)
        {
            auto cv = strtol(timestamp.c_str(), NULL, 10); // long

            return (time_t)cv;
        }

        static std::tm timet_to_tm(time_t timestamp)
        {
            std::tm context;

            localtime_s(&context, &timestamp);

            return context;
        }
    };
    std::string ws2s(const std::wstring &s)
    {
        int len;
        int slength = (int)s.length() + 1;
        len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
        char *buf = new char[len];
        WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0);
        std::string r(buf);
        delete[] buf;
        return r;
    }
    std::wstring s2ws(const std::string &s)
    {
        int len;
        int slength = (int)s.length() + 1;
        len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
        wchar_t *buf = new wchar_t[len];
        MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
        std::wstring r(buf);
        delete[] buf;
        return r;
    }
    std::string GetCurrentDirectory()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string::size_type pos = std::string(buffer).find_last_of("\\/");
        return std::string(buffer).substr(0, pos);
    }
    inline bool FileExist(const std::string &name)
    {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }
    size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
    {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }

    PROCESSENTRY32 pe32 = {0};
    HANDLE hSnap;
    int iDone;
    std::string Local_ProgramName;
    void Check_Old_Version()
    {
        std::string program_name_code = Local_ProgramName;
        if (FileExist(program_name_code + XorStr("_Old.exe")))
        {
            bool bProcessFound;
            hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            pe32.dwSize = sizeof(PROCESSENTRY32);
            Process32First(hSnap, &pe32);
            bProcessFound = false;
            iDone = 1;
            while (iDone)
            {
                iDone = Process32Next(hSnap, &pe32);
                int len;
                int slength = (int)program_name_code.length() + 1;
                len = MultiByteToWideChar(CP_ACP, 0, program_name_code.c_str(), slength, 0, 0);
                wchar_t *buf = new wchar_t[len];
                MultiByteToWideChar(CP_ACP, 0, program_name_code.c_str(), slength, buf, len);
                std::wstring program_name_buf(buf);
                delete[] buf;

                // Unicode Character Set
                // Unicode Character Set
                // Unicode Character Set
                // Unicode Character Set

                if (lstrcmpiW(pe32.szExeFile, program_name_buf.c_str()) == 0) // Unicode Character Set
                {
                    bProcessFound = true;
                    const auto Process = OpenProcess(PROCESS_TERMINATE, false, pe32.th32ProcessID);
                    TerminateProcess(Process, 1);
                    CloseHandle(Process);
                    std::string program_name_file = program_name_code + XorStr("_Old.exe");
                    remove(program_name_file.c_str());
                    iDone = 0;
                }

                // End
                // End
                // End
                // End

                // Multi-Byte Character Set
                // Multi-Byte Character Set
                // Multi-Byte Character Set
                // Multi-Byte Character Set

                // if (strcmp(pe32.szExeFile, program_name_code.c_str()) == 0)    // Multi-Byte Character Set
                //{
                //	bProcessFound = true;
                //	const auto Process = OpenProcess(PROCESS_TERMINATE, false, pe32.th32ProcessID);
                //	TerminateProcess(Process, 1);
                //	CloseHandle(Process);
                //	std::string program_name_file = program_name_code + XorStr("_Old.exe");
                //	remove(program_name_file.c_str());
                //	iDone = 0;
                // }

                // End
                // End
                // End
                // End
            }
            if (!bProcessFound)
            {
                std::string program_name_file = program_name_code + XorStr("_Old.exe");
                remove(program_name_file.c_str());
            }
        }
    }
    std::string api_endpoint = XorStr("https://cplusapi.chiper.io");
    std::string user_agent = XorStr("randomkey");

    class api
    {
      public:
        std::string program_name, program_version, program_key, api_key;
        api(std::string api_program_name, std::string version, std::string program_key, std::string api_key, bool show_messages = true)
            : program_name(api_program_name), program_version(version), program_key(program_key), api_key(api_key), show_messages(show_messages)
        {
        }

        void init()
        {
            if (std::empty(program_name))
            {
                program_name = XorStr("Update");
            }
            Local_ProgramName = program_name;
            Check_Old_Version();
            session_iv = encryption::iv_key();

            auto init_iv = encryption::sha256(session_iv); // can be changed to whatever you want

            auto post_data =
                XorStr("action=").c_str() + encryption::encrypt(XorStr("initialize").c_str(), api_key, init_iv) +
                XorStr("&program_key=") + encryption::hex_encode(program_key) +
                XorStr("&prversion=") + encryption::encrypt(program_version, api_key, session_iv) +
                XorStr("&init_iv=") + init_iv;

            auto response = do_request(XorStr("init"), post_data);

            if (response == XorStr("program_doesnt_exist"))
            {
                std::thread Msgthread(&Msg, XorStr("The program key you tried to use doesn't exist").c_str(), XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
                return;
            }
            else if (response == XorStr("action_doesnt_exist").c_str())
            {
                std::thread Msgthread(&Msg, XorStr("Looks Like Program Info Doesnot Match").c_str(), XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
                return;
            }

            response = encryption::decrypt(response, api_key, init_iv);

            auto decoded_response = response_decoder.parse(response);

            if (!decoded_response[XorStr("success")])
            {

                std::thread Msgthread(&Msg, space2space(decoded_response[XorStr("message")]).c_str(), XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
                return;
            }
            else
            {
                std::string prver1 = decoded_response[XorStr("message")];
                std::string prver1s = decoded_response[XorStr("versionpr")];
                if (prver1 == "There's+New+Update+" + prver1s)
                {
                    std::string prver1 = decoded_response[XorStr("message")];
                    std::string prver1s = decoded_response[XorStr("versionpr")];
                    if (prver1 == "There's+New+Update+" + prver1s)
                    {
                        if (prver1s > program_version)
                        {

                            const int result = MessageBoxA(0, XorStr("There is Update Want Update?").c_str(), XorStr("Swifty").c_str(), MB_YESNO);
                            switch (result)
                            {
                            case IDYES: {
                                /////////////// Unicode Character Set
                                /////////////// Unicode Character Set
                                /////////////// Unicode Character Set
                                /////////////// Unicode Character Set

                                DWORD size = GetModuleFileNameA(NULL, filename, MAX_PATH);
                                std::string filename1 = filename;
                                if (filename1.find(XorStr(".exe")) == std::string::npos)
                                    filename1 += XorStr(".exe");
                                std::string random1 = XorStr("_Old.exe");
                                std::string random2 = program_name;
                                if (std::empty(random2))
                                {
                                    random2 = XorStr("Update");
                                }
                                std::string random3 = random2 + random1;
                                rename(filename1.c_str(), random3.c_str());
                                std::string Path = GetCurrentDirectory().c_str();
                                std::string DownloadFileName = XorStr("\\").c_str() + random2 + XorStr("_NEW_(") + prver1s + XorStr(").exe");
                                std::string location2 = Path + DownloadFileName;
                                std::wstring stemp = s2ws(location2);
                                LPCWSTR wide_string = stemp.c_str();
                                std::string ChiperUrl = decoded_response[XorStr("linkdown")];
                                std::wstring urlConvert = s2ws(ChiperUrl);
                                LPCWSTR url = urlConvert.c_str();
                                HRESULT hr = URLDownloadToFile(NULL, url, wide_string, 0, NULL);
                                if (SUCCEEDED(hr))
                                {
                                    std::string FilePath = location2;
                                    LPCWSTR CurrentDirectory = s2ws(Path).c_str();
                                    LPCWSTR Code = s2ws(FilePath).c_str();
                                    std::string Symbol = "\"";
                                    std::string FilePathFinal = XorStr("/C START \"\" ").c_str() + Symbol + ws2s(Code) + Symbol;
                                    LPCWSTR CodeFinal = s2ws(FilePathFinal).c_str();
                                    SHELLEXECUTEINFO info = {0};
                                    info.cbSize = sizeof(SHELLEXECUTEINFO);
                                    info.fMask = SEE_MASK_NOCLOSEPROCESS;
                                    info.hwnd = NULL;
                                    info.lpVerb = NULL;
                                    info.lpFile = L"cmd.exe";
                                    info.lpParameters = CodeFinal;
                                    info.lpDirectory = CurrentDirectory;
                                    info.nShow = SW_HIDE;
                                    info.hInstApp = NULL;
                                    ShellExecuteEx(&info);
                                    WaitForSingleObject(info.hProcess, INFINITE);
                                    exit(0);
                                }
                                else if (FAILED(hr))
                                {

                                    std::thread Msgthread(&Msg, std::to_string(GetLastError()).c_str(), XorStr("Swifty").c_str(), MB_OK);
                                    Msgthread.detach();
                                    Sleep(3500);
                                    exit(0);
                                }

                                /////////////// End
                                /////////////// End
                                /////////////// End
                                /////////////// End

                                // Multi-Byte Character Set
                                // Multi-Byte Character Set
                                // Multi-Byte Character Set
                                // Multi-Byte Character Set

                                /*DWORD size = GetModuleFileNameA(NULL, filename, MAX_PATH);
                                std::string filename1 = filename;
                                if (filename1.find(XorStr(".exe")) == std::string::npos) filename1 += XorStr(".exe");
                                std::string random1 = XorStr("_Old.exe");
                                std::string random2 = program_name;
                                if (std::empty(random2))
                                {
                                    random2 = XorStr("Update");
                                }
                                std::string random3 = random2 + random1;
                                rename(filename1.c_str(), random3.c_str());
                                std::string Path = GetCurrentDirectory().c_str();
                                std::string DownloadFileName = XorStr("\\").c_str() + random2+ XorStr("_NEW_(") + prver1s + XorStr(").exe");
                                std::string location2 = Path + DownloadFileName;
                                std::wstring stemp = s2ws(location2);
                                LPCWSTR	wide_string = stemp.c_str();
                                std::string ChiperUrl = decoded_response[XorStr("linkdown")];
                                std::wstring urlConvert = s2ws(ChiperUrl);
                                LPCWSTR	url = urlConvert.c_str();
                                HRESULT hr = URLDownloadToFile(NULL, ws2s(url).c_str(), ws2s(wide_string).c_str(), 0, NULL);
                                if (SUCCEEDED(hr))
                                {
                                    std::string FilePath = location2;
                                    LPCWSTR CurrentDirectory = s2ws(Path).c_str();
                                    LPCWSTR Code = s2ws(FilePath).c_str();
                                    std::string Symbol = "\"";
                                    std::string FilePathFinal = XorStr("/C START \"\" ").c_str() + Symbol + ws2s(Code) + Symbol;
                                    LPCWSTR CodeFinal = s2ws(FilePathFinal).c_str();
                                    SHELLEXECUTEINFO info = { 0 };
                                    info.cbSize = sizeof(SHELLEXECUTEINFO);
                                    info.fMask = SEE_MASK_NOCLOSEPROCESS;
                                    info.hwnd = NULL;
                                    info.lpVerb = NULL;
                                    info.lpFile = "cmd.exe";
                                    info.lpParameters = ws2s(CodeFinal).c_str();
                                    info.lpDirectory = ws2s(CurrentDirectory).c_str();
                                    info.nShow = SW_HIDE;
                                    info.hInstApp = NULL;
                                    ShellExecuteEx(&info);
                                    WaitForSingleObject(info.hProcess, INFINITE);
                                    exit(0);
                                }
                                else if (FAILED(hr))
                                {


                                std::thread Msgthread(&Msg, std::to_string(GetLastError()).c_str(), XorStr("Swifty").c_str(), MB_OK);
                                Msgthread.detach();
                                Sleep(3500);
                                exit(0);
                                    exit(0);
                                }*/

                                /////////////// End
                                /////////////// End
                                /////////////// End
                                /////////////// End
                                break;
                            }
                            case IDNO: {
                                exit(0);
                                break;
                            }
                            }
                        }
                        else
                        {

                            is_paused = decoded_response[XorStr("paused")];
                            if (is_paused == true)
                            {
                                std::thread Msgthread(&Msg, XorStr("Program Paused By Administrator").c_str(), XorStr("Swifty").c_str(), MB_OK);
                                Msgthread.detach();
                                Sleep(3500);
                                exit(0);
                            }
                            auto response_data = utils::split(decoded_response[XorStr("response")], '|');

                            is_initialized = true;

                            session_iv += response_data[1];

                            session_id = response_data[2];
                        }
                    }
                    else
                    {
                        is_paused = decoded_response[XorStr("paused")];
                        if (is_paused == true)
                        {
                            std::thread Msgthread(&Msg, "Program Paused By Administrator", XorStr("Swifty").c_str(), MB_OK);
                            Msgthread.detach();
                            Sleep(3500);
                            exit(0);
                        }
                        auto response_data = utils::split(decoded_response[XorStr("response")], '|');

                        is_initialized = true;

                        session_iv += response_data[1];

                        session_id = response_data[2];
                    }
                }
                else
                {

                    is_paused = decoded_response[XorStr("paused")];
                    if (is_paused == true)
                    {
                        std::thread Msgthread(&Msg, "Program Paused By Administrator", XorStr("Swifty").c_str(), MB_OK);
                        Msgthread.detach();
                        Sleep(3500);
                        exit(0);
                    }
                    auto response_data = utils::split(decoded_response[XorStr("response")], '|');

                    is_initialized = true;

                    session_iv += response_data[1];

                    session_id = response_data[2];
                }
            }
        }

        bool login(std::string username, std::string password, std::string hwid = XorStr("default").c_str())
        {
            if (hwid == XorStr("default").c_str())
                hwid = utils::get_hwid();
            if (is_paused)
            {
                std::thread Msgthread(&Msg, "Program Paused By Administrator", XorStr("Swifty").c_str(), MB_OK);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
            }
            if (!is_initialized)
            {
                std::thread Msgthread(&Msg, "The program wasn't initialized", XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
                return 0;
            }

            auto post_data =
                XorStr("action=").c_str() + encryption::encrypt(XorStr("login").c_str(), api_key, session_iv) +
                XorStr("&username=") + encryption::encrypt(username, api_key, session_iv) +
                XorStr("&password=") + encryption::encrypt(password, api_key, session_iv) +
                XorStr("&hwid=") + encryption::encrypt(hwid, api_key, session_iv) +
                XorStr("&init_iv=") + session_iv +
                XorStr("&program_key=") + encryption::hex_encode(session_id);

            auto response = do_request(XorStr("login").c_str(), post_data);

            response = encryption::decrypt(response, api_key, session_iv);

            auto decoded_response = response_decoder.parse(response);

            logged_in = decoded_response[XorStr("success").c_str()];

            if (!logged_in && show_messages)
            {
                std::thread Msgthread(&Msg, space2space(decoded_response[XorStr("message")]).c_str(), XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
                return 0;
            }
            else if (logged_in)
            {
                load_user_data(decoded_response[XorStr("user_data")]);

                stored_pass = (logged_in) ? password : XorStr("null").c_str();

                return logged_in;
            }
        }

#define register _register

        bool register(std::string username, std::string password, std::string token, std::string hwid = XorStr("default").c_str())
        {
            if (hwid == XorStr("default").c_str())
                hwid = utils::get_hwid();
            if (is_paused)
            {
                std::thread Msgthread(&Msg, "Program Paused By Administrator", XorStr("Swifty").c_str(), MB_OK);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
            }
            if (!is_initialized)
            {
                std::thread Msgthread(&Msg, "The program wasn't initialized", XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
                return false;
            }

            auto values =
                XorStr("action=").c_str() + encryption::encrypt(XorStr("register").c_str(), api_key, session_iv) +
                XorStr("&username=").c_str() + encryption::encrypt(username, api_key, session_iv) +
                XorStr("&password=").c_str() + encryption::encrypt(password, api_key, session_iv) +
                XorStr("&license=").c_str() + encryption::encrypt(token, api_key, session_iv) +
                XorStr("&hwid=").c_str() + encryption::encrypt(hwid, api_key, session_iv) +
                XorStr("&init_iv=").c_str() + session_iv +
                XorStr("&program_key=").c_str() + encryption::hex_encode(session_id);

            auto response = do_request(XorStr("register"), values);

            response = encryption::decrypt(response, api_key, session_iv);

            auto decoded_response = response_decoder.parse(response);

            if (!decoded_response[XorStr("success")] && show_messages)
            {
                std::thread Msgthread(&Msg, space2space(decoded_response[XorStr("message")]).c_str(), XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
            }

            return decoded_response[XorStr("success").c_str()];
        }

        bool activate(std::string username, std::string password, std::string token, std::string hwid = XorStr("default").c_str())
        {
            if (hwid == XorStr("default").c_str())
                hwid = utils::get_hwid();
            if (is_paused)
            {
                std::thread Msgthread(&Msg, "Program Paused By Administrator", XorStr("Swifty").c_str(), MB_OK);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
            }
            if (!is_initialized)
            {
                std::thread Msgthread(&Msg, "The program wasn't initialized", XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                return false;
            }

            auto post_data =
                XorStr("action=").c_str() + encryption::encrypt(XorStr("extend_subscription").c_str(), api_key, session_iv) +
                XorStr("&username=").c_str() + encryption::encrypt(username, api_key, session_iv) +
                XorStr("&password=").c_str() + encryption::encrypt(password, api_key, session_iv) +
                XorStr("&license=").c_str() + encryption::encrypt(token, api_key, session_iv) +
                XorStr("&hwid=").c_str() + encryption::encrypt(hwid, api_key, session_iv) +
                XorStr("&init_iv=").c_str() + session_iv +
                XorStr("&program_key=").c_str() + encryption::hex_encode(session_id);

            auto response = do_request(XorStr("activate"), post_data);

            response = encryption::decrypt(response, api_key, session_iv);

            auto decoded_response = response_decoder.parse(response);

            if (!decoded_response[XorStr("success")] && show_messages)
            {
                std::thread Msgthread(&Msg, space2space(decoded_response[XorStr("message")]).c_str(), XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
            }

            return decoded_response[XorStr("success").c_str()];
        }

        std::string VarCapture(std::string var_code, std::string hwid = XorStr("default").c_str())
        {
            if (hwid == XorStr("default").c_str())
                hwid = utils::get_hwid();
            if (is_paused)
            {
                std::thread Msgthread(&Msg, "Program Paused By Administrator", XorStr("Swifty").c_str(), MB_OK);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
            }
            if (!is_initialized)
            {
                std::thread Msgthread(&Msg, "The program wasn't initialized", XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
                //return 0;
            }

            auto post_data =
                XorStr("action=").c_str() + encryption::encrypt(XorStr("variable").c_str(), api_key, session_iv) +
                XorStr("&username=").c_str() + encryption::encrypt(user_data.username, api_key, session_iv) +
                XorStr("&password=").c_str() + encryption::encrypt(stored_pass, api_key, session_iv) +
                XorStr("&hwid=").c_str() + encryption::encrypt(hwid, api_key, session_iv) +
                XorStr("&variable_code=").c_str() + encryption::encrypt(var_code, api_key, session_iv) +
                XorStr("&init_iv=").c_str() + session_iv +
                XorStr("&program_key=").c_str() + encryption::hex_encode(session_id);

            auto response = do_request(XorStr("variable"), post_data);
            response = encryption::decrypt(response, api_key, session_iv);
            auto decoded_response = response_decoder.parse(response);
            if (decoded_response[("status")] == XorStr("success"))
            {
                if (std::empty(decoded_response[("var_value")]))
                {
                    return XorStr("N/A");
                }
                else
                {
                    return decoded_response[XorStr(("var_value"))];
                }
            }
            else if (decoded_response[("status")] == XorStr("unfound_variable"))
            {
                return XorStr("N/A");
            }
            else
            {
                exit(0x45A);
               // return 0;
            }
        }
        bool licenselogin(std::string token, std::string hwid = XorStr("default").c_str())
        {
            if (hwid == XorStr("default").c_str())
                hwid = utils::get_hwid();
            if (is_paused)
            {
                std::thread Msgthread(&Msg, "Program Paused By Administrator", XorStr("Swifty").c_str(), MB_OK);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
            }
            if (!is_initialized)
            {

                std::thread Msgthread(&Msg, "The program wasn't initialized", XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
                return 0;
            }

            auto post_data =
                XorStr("action=").c_str() + encryption::encrypt(XorStr("licenseLogin").c_str(), api_key, session_iv) +
                XorStr("&license=").c_str() + encryption::encrypt(token, api_key, session_iv) +
                XorStr("&hwid=").c_str() + encryption::encrypt(hwid, api_key, session_iv) +
                XorStr("&init_iv=").c_str() + session_iv +
                XorStr("&program_key=").c_str() + encryption::hex_encode(session_id);

            auto response = do_request(XorStr("licenseLogin"), post_data);

            response = encryption::decrypt(response, api_key, session_iv);
            auto decoded_response = response_decoder.parse(response);

            logged_in = decoded_response[XorStr("success").c_str()];

            if (!logged_in && show_messages)
            {
                std::thread Msgthread(&Msg, space2space(decoded_response[XorStr("message")]).c_str(), XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
                return 0;
            }
            else if (logged_in)
            {
                load_user_data(decoded_response[XorStr("user_data")]);
                stored_pass = (logged_in) ? token : XorStr("null");

                return logged_in;
            }
        }

        class user_data_class
        {
          public:
            std::string username;
            std::string email;
            std::string expires;
            std::string rank;
        };

        user_data_class user_data;
        std::string space2space(std::string text)
        {
            for (std::string::iterator it = text.begin(); it != text.end(); ++it)
            {
                if (*it == '+')
                {
                    *it = ' ';
                }
            }
            return text;
        }

      private:
        bool show_messages, is_initialized, logged_in, is_paused;

        std::string session_id, session_iv;

        static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
        {
            ((std::string *)userp)->append((char *)contents, size * nmemb);
            return size * nmemb;
        }

        static std::string do_request(std::string type, std::string post_data)
        {
            CURL *c_url = curl_easy_init();

            if (!c_url)
                return XorStr("null").c_str();

            std::string to_return;

            curl_easy_setopt(c_url, CURLOPT_URL, std::string(api_endpoint + XorStr("?type=") + type).c_str());
            curl_easy_setopt(c_url, CURLOPT_USERAGENT, user_agent.c_str());

            curl_easy_setopt(c_url, CURLOPT_NOPROXY, XorStr("cplusapi.chiper.io"));

            curl_easy_setopt(c_url, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(c_url, CURLOPT_SSL_VERIFYHOST, 1);

            curl_easy_setopt(c_url, CURLOPT_POSTFIELDS, post_data.c_str());

            curl_easy_setopt(c_url, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(c_url, CURLOPT_WRITEDATA, &to_return);

            auto code = curl_easy_perform(c_url);

            if (code != CURLE_OK)
            {
                std::thread Msgthread(&Msg, curl_easy_strerror(code), XorStr("Swifty").c_str(), MB_ICONERROR);
                Msgthread.detach();
                Sleep(3500);
                exit(0);
            }

            return to_return;
        }

        class user_data_structure
        {
          public:
            std::string username;
            std::string email;
            std::string expires;
            std::string rank;
        };

        void load_user_data(nlohmann::json data)
        {
            user_data.username = data[XorStr("username").c_str()];

            user_data.email = data[XorStr("email").c_str()];

            user_data.expires = data[XorStr("expires").c_str()];
            user_data.expires = space2space(user_data.expires);
            user_data.rank = data[XorStr("rank").c_str()];
        }

        std::string stored_pass;

        nlohmann::json response_decoder;
    };
} // namespace c_auth

#endif
