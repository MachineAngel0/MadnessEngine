
#define GLFW_INCLUDE_VULKAN
#include "src/vendor/glfw-3.4/include/GLFW/glfw3.h"

/*
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <stdint.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
*/

#include <stdio.h>

#include <asserts.h>
#include <logger.h>
#include <windows.h>
#include "src/platform/platform.h"
//#include "renderer/triangle.h"


//Go to File-> Settings -> Editor -> File and Code Templates.
//You will find there 3 tabs, namely, Templates, Includes, and Code.
//Now under Templates choose for example C Header File.
//Insert #pragma once to replace its content.
//Now every time you add a new Header from project menu you will have your template.


int main(void)
{

    printf("Hello World!\n");
    MFATAL("MFATAL and %s", "FATAL");
    MERROR("MERROR and %d", 10);
    MWARN("MWARN");
    MDEBUG("MDEBUG");
    MINFO("MINFO");
    MTRACE("MTRACE");

    //MASSERT(1==0);

    /*
    platform_state state;
    if(platform_startup(&state, "Kohi Engine Testbed", 100, 100, 1280, 720)) {
        while(TRUE) {
            platform_pump_messages(&state);
        }
    }
    platform_shutdown(&state);
    */

    /*
    GLFWwindow* window;
    initWindow(&window, 800, 600);
    initVulkan();
    mainLoop(&window);
    cleanup(&window);
*/


    return 0;
}
