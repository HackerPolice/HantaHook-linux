#include "definations.hpp"

/**
 * @file hantahook.cpp
 * @author Pritam Dutta (rohitdutta907@gmail.com)
 * @brief Main File for HantaHook a hack for CSGO
 * @version 0.1
 * @date 2021-02-10
 * 
 * @copyright Copyright (c) 2021
 */

/**
 * @brief Main thread to hook our program
 * 
 */
static void Main_Thread();


/* Entrypoint to the Library. Called when loading */
int __attribute__((constructor)) Start()
{
	std::thread main_thread(Main_Thread);
	main_thread.detach();

	return 0;
}

static void Main_Thread(){

}