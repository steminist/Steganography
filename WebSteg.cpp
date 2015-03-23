#include "provided.h"
#include <string>
#include "http.h"
using namespace std;

bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
    string tempText;
    if (! HTTP().get(url, tempText))
        return false;
    if (! Steg::hide(tempText, msg, host))
        return false;
    return true;
    
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
    string tempText;
    if (! HTTP().get(url, tempText))
        return false;
    if (! Steg::reveal(tempText, msg))
        return false;
    return true;
}
