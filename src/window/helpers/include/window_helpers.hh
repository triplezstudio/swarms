//
// Created by Martin Gruscher on 18.05.26.
//

#ifndef SWARMS_WINDOW_HELPERS_HH
#define SWARMS_WINDOW_HELPERS_HH
#include <string>

namespace tz {

void initWindow(int width, int height, const std::string& title);
void closeWindow(void);
void showCursor();
void hideCursor();
void beginDrawing();
void endDrawing();

}

#endif //SWARMS_WINDOW_HELPERS_HH
