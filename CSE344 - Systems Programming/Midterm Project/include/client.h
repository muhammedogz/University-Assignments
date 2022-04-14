#ifndef CLIENT_H
#define CLIENT_H

/**
 * @brief Check if the given arguemtns are valid or not
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @param pathToServerFifo Path to server fifo
 * @param pathToDataFile Path to data file
 * @return int 1 if arguments are valid, error otherwise
 */
int detectArguments(int argc, char *argv[], char **pathToServerFifo, char **pathToDataFile);

/**
 * @brief Read the file and return its content
 *
 * @param fileName File name
 * @param fileSize File size
 * @return char* File content
 */
char *readFile(char *fileName, int *fileSize);

/**
 * @brief Write to stderr the usage of the program
 *
 */
void invalidUsage();

#endif