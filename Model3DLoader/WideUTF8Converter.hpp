#ifndef WideUTF8Converter_hpp
#define WideUTF8Converter_hpp

std::string ConvertWideToUtf8(std::wstring wide) {
    return std::string(wide.begin(), wide.end());
}

#endif // !WideUTF8Converter_hpp



