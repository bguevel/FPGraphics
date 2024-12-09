/*
 *  CSCI 441, Computer Graphics, Fall 2024
 *
 *  Project: lab02
 *  File: main.cpp
 *
 *	Author: Jeffrey Paone - Fall 2024
 *
 *  Description:
 *      Contains the base code for a basic flight simulator.
 *
 */

#include "FPEngine.h"

//**************************************************************************************
//
// Our main function

int main() {
    auto labEngine = new FPEngine();
    labEngine->initialize();
    if (labEngine->getError() == CSCI441::OpenGLEngine::OPENGL_ENGINE_ERROR_NO_ERROR) {
        labEngine->run();
    }
    labEngine->shutdown();
    delete labEngine;

	return EXIT_SUCCESS;				// exit our program successfully!
}
