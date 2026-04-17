#include <QApplication>
#include <QFontDatabase>

#include "view/Mainwindow.h"

#ifdef WIN32
// clang-format off
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
// clang-format on
LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException);

#endif

int main(int argc, char* argv[]) {
#ifdef WIN32
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#endif

    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

    int fontId = QFontDatabase::addApplicationFont(":/font/WenQuanWeiMiHei.ttf");
    QString mysh = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QFont font(mysh, 10);
    QApplication::setFont(font);

    MainWindow w;
    w.show();
    return a.exec();
}

// Unhandled Exception的回调函数
#ifdef WIN32

#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>

// 创建Dump文件
void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS* pException) {
    std::filesystem::create_directory("./dump");
    HANDLE hDumpFile =
        CreateFileW(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // Dump信息
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;
    // 写入Dump文件内容
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    CloseHandle(hDumpFile);
}

std::wstring String2Wstring(const std::string& wstr) {
    if (wstr.empty()) {
        return std::wstring();
    }
    size_t charsNeeded = std::mbstowcs(nullptr, wstr.c_str(), 0);
    if (charsNeeded == static_cast<size_t>(-1)) {
        return std::wstring();
    }
    std::wstring result(charsNeeded, 0);
    std::mbstowcs(result.data(), wstr.c_str(), charsNeeded + 1);
    return result;
}

// 处理Unhandled Exception的回调函数
LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException) {
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y%m%d_%H%M%S");
    std::string file_name = ss.str();
    std::string time_str = file_name;
    std::wstring dump_file = String2Wstring("./dump/" + time_str + ".dmp");

    CreateDumpFile(dump_file.c_str(), pException);
    return EXCEPTION_EXECUTE_HANDLER;
}

#endif
