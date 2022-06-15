#include <stdio.h>
#include "../include/servant.h"

ServantVariables servantVariables;

int sigintReceived = 0;

void signalHandler()
{
  sigintReceived = 1;
  printf("\n");
  printf("[!] SIGINT received.\n");
}

void atexitHandler()
{
  freeList(servantVariables.cities, freeString);
  freeList(servantVariables.citiesStruct, freeCity);
}

int detectArguments(int argc, char *argv[])
{
  if (argc != 9)
  {
    GLOBAL_ERROR = INVALID_ARGUMENTS;
    return -1;
  }

  // use getopt to parse arguments
  int opt;
  int dFound = 0;
  int cFound = 0;
  int rFound = 0;
  int pFound = 0;
  while ((opt = getopt(argc, argv, "d:c:r:p:")) != -1)
  {
    switch (opt)
    {
    case 'd':
      if (dFound)
      {
        GLOBAL_ERROR = INVALID_ARGUMENTS;
        return -1;
      }
      servantVariables.directoryPath = optarg;
      dFound = 1;
      break;
    case 'c':
      if (cFound)
      {
        GLOBAL_ERROR = INVALID_ARGUMENTS;
        return -1;
      }
      servantVariables.cityInterval = optarg;
      cFound = 1;
      break;
    case 'r':
      if (rFound)
      {
        GLOBAL_ERROR = INVALID_ARGUMENTS;
        return -1;
      }
      servantVariables.ipAddress = optarg;
      rFound = 1;
      break;
    case 'p':
      if (pFound)
      {
        GLOBAL_ERROR = INVALID_ARGUMENTS;
        return -1;
      }
      servantVariables.port = atoi(optarg);
      pFound = 1;
      break;
    default:
      GLOBAL_ERROR = INVALID_ARGUMENTS;
      return -1;
    }
  }

  if (!dFound || !cFound || !rFound || !pFound)
  {
    dprintf(STDERR_FILENO, "[!] Invalid arguments.\n");
    GLOBAL_ERROR = INVALID_ARGUMENTS;
    return -1;
  }

  // divide cities to two parts with delimeter -
  char *p = servantVariables.cityInterval;
  char *q = strchr(p, '-');
  if (q == NULL)
  {
    dprintf(STDERR_FILENO, "[!] Invalid cities.\n");
    GLOBAL_ERROR = INVALID_ARGUMENTS;
    return -1;
  }
  *q = '\0';
  servantVariables.cityStart = atoi(p);
  servantVariables.cityEnd = atoi(q + 1);
  if (servantVariables.cityStart > servantVariables.cityEnd)
  {
    dprintf(STDERR_FILENO, "[!] City start must be less than city end.\n");
    GLOBAL_ERROR = INVALID_ARGUMENTS;
    return -1;
  }

  // check if port is bigger than 2000
  if (servantVariables.port < 2000)
  {
    dprintf(STDERR_FILENO, "[!] Port must be bigger than 2000.\n");
    GLOBAL_ERROR = INVALID_ARGUMENTS;
    return -1;
  }

  // check if given directory exist
  if (access(servantVariables.directoryPath, F_OK) == -1)
  {
    dprintf(STDERR_FILENO, "[!] Directory %s does not exist.\n", servantVariables.directoryPath);
    GLOBAL_ERROR = INVALID_ARGUMENTS;
    return -1;
  }

  List *list = NULL;
  if ((list = initializeList()) == NULL)
  {
    dprintf(STDERR_FILENO, "[!] Cannot initialize list.\n");
    GLOBAL_ERROR = INVALID_ARGUMENTS;
    return -1;
  }

  DIR *directory;
  int directoryCount = 0;
  struct dirent *dir;
  directory = opendir(servantVariables.directoryPath);
  if (directory)
  {
    while ((dir = readdir(directory)) != NULL)
    {
      if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
      {
        continue;
      }

      directoryCount++;
      char *temp = malloc(sizeof(char) * (strlen(dir->d_name) + 1));
      strcpy(temp, dir->d_name);
      addNode(list, temp);

      // printf("%s\n", dir->d_name);
    }
    closedir(directory);
  }
  sortList(list, compareString);

  // check if directory count is equal to start and end city
  if (directoryCount - 2 < servantVariables.cityEnd - servantVariables.cityStart + 1)
  {
    dprintf(STDERR_FILENO, "[!] Directory count is not equal to start and end city.\n");
    GLOBAL_ERROR = INVALID_ARGUMENTS;
    return -1;
  }

  servantVariables.cities = NULL;
  if ((servantVariables.cities = getListInRangeString(list, servantVariables.cityStart - 1, servantVariables.cityEnd - 1)) == NULL)
  {
    dprintf(STDERR_FILENO, "[!] Cannot get cities.\n");
    GLOBAL_ERROR = INVALID_ARGUMENTS;
    return -1;
  }
  freeList(list, freeString);
  // printList(servantVariables.cities, printString);

  // print all
  // dprintf(STDOUT_FILENO, "%s: Input file 1: %s\n", getTime(), argumentInfo.directoryPath);
  // dprintf(STDOUT_FILENO, "%s: Input file 2: %s\n", getTime(), argumentInfo.cities);
  // dprintf(STDOUT_FILENO, "%s: IP address: %s\n", getTime(), argumentInfo.ipAddress);
  // dprintf(STDOUT_FILENO, "%s: Port: %d\n", getTime(), argumentInfo.port);
  // dprintf(STDOUT_FILENO, "%s: City start: %d\n", getTime(), argumentInfo.cityStart);
  // dprintf(STDOUT_FILENO, "%s: City end: %d\n", getTime(), argumentInfo.cityEnd);
  // dprintf(STDOUT_FILENO, "%s: Directory count: %d\n", getTime(), directoryCount);

  return 1;
}

int init(int argc, char *argv[])
{
  servantVariables.pid = getOwnPid();
  servantVariables.cities = NULL;
  servantVariables.citiesStruct = NULL;
  servantVariables.directoryPath = NULL;
  servantVariables.ipAddress = NULL;
  servantVariables.port = 0;
  servantVariables.cityStart = 0;
  servantVariables.cityEnd = 0;
  servantVariables.totalRequestHandled = 0;
  servantVariables.cityInterval = NULL;

  if (detectArguments(argc, argv) != 1)
  {
    printUsage();
    printError(STDERR_FILENO, GLOBAL_ERROR);
  }

  if ((GLOBAL_ERROR = initializeSignalAndAtexit(SIGINT, signalHandler, atexitHandler) != 0))
  {
    printError(STDERR_FILENO, GLOBAL_ERROR);
  }

  servantVariables.citiesStruct = initializeList();

  Node *tempNode = servantVariables.cities->head;
  while (tempNode != NULL)
  {
    getCityInformation(tempNode->data);
    tempNode = tempNode->next;
  }

  dprintf(STDOUT_FILENO, "%s: Servant %d: loaded dataset. cities %s-%s\n", getTime(), servantVariables.pid, (char *)listGetFirst(servantVariables.cities), (char *)listGetLast(servantVariables.cities));
  dprintf(STDOUT_FILENO, "%s: Servant %d: listenint at port %d\n", getTime(), servantVariables.pid, servantVariables.port);

  // printList(servantVariables.citiesStruct, printCity);

  while (!sigintReceived)
    ;

  return 0;
}

void printUsage()
{
  dprintf(STDOUT_FILENO, "Usage: ./servant -d <directory path> -c <cities file> -r <ip address> -p <port>\n");
}

Record *createRecord(char *line)
{
  Record *record = malloc(sizeof(Record));
  record->id = 0;
  record->price = 0;
  record->surfaceArea = 0;
  record->day = 0;
  record->month = 0;
  record->year = 0;
  record->streetName = NULL;
  record->realEstateType = NULL;
  if (record == NULL)
  {
    dprintf(STDERR_FILENO, "[!] Cannot allocate memory for record.\n");
    GLOBAL_ERROR = INVALID_MALLOC;
    return NULL;
  }
  // split string by space
  char *token = strtok(line, " ");
  if (token == NULL)
  {
    dprintf(STDERR_FILENO, "[!] Cannot split string.\n");
    GLOBAL_ERROR = INVALID_ARGUMENTS;
    return NULL;
  }

  int i = 0;
  while (token != NULL)
  {
    switch (i)
    {
    case 0:
      record->id = atoi(token);
      break;
    case 1:
      record->realEstateType = malloc(sizeof(char) * (strlen(token) + 1));
      strcpy(record->realEstateType, token);
      break;
    case 2:
      record->streetName = malloc(sizeof(char) * (strlen(token) + 1));
      strcpy(record->streetName, token);
      break;
    case 3:
      record->surfaceArea = atoi(token);
      break;
    case 4:
      record->price = atoi(token);
      break;
    }
    token = strtok(NULL, " ");
    i++;
  }

  return record;
}

int getCityInformation(char *cityName)
{
  City *city = malloc(sizeof(City));
  city->name = malloc(sizeof(char) * (strlen(cityName) + 1));
  strcpy(city->name, cityName);
  city->records = initializeList();
  addNode(servantVariables.citiesStruct, city);

  char directoryPath[100];
  sprintf(directoryPath, "%s/%s", servantVariables.directoryPath, cityName);

  DIR *dir = opendir(directoryPath);
  if (dir == NULL)
  {
    dprintf(STDERR_FILENO, "[!] Cannot open directory.\n");
    return -1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
  {
    int day, month, year;
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;
    char filePath[400];
    snprintf(filePath, 400, "%s/%s", directoryPath, entry->d_name);
    char *fileNameParse = malloc(sizeof(char) * (strlen(entry->d_name) + 1));
    strcpy(fileNameParse, entry->d_name);

    char *token = strtok(fileNameParse, "-");
    if (token == NULL)
    {
      dprintf(STDERR_FILENO, "[!] Cannot split string.\n");
      GLOBAL_ERROR = INVALID_ARGUMENTS;
      return -1;
    }

    int i = 0;
    while (token != NULL)
    {
      switch (i)
      {
      case 0:
        day = atoi(token);
        break;
      case 1:
        month = atoi(token);
        break;
      case 2:
        year = atoi(token);
        break;
      }
      token = strtok(NULL, "-");
      i++;
    }
    free(fileNameParse);

    int fd = open(filePath, O_RDONLY, 666);
    if (fd == -1)
    {
      dprintf(STDERR_FILENO, "[!] Cannot open file.\n");
      return -1;
    }

    // read whole file
    int fileSize = getFileSize(filePath);
    char *buffer = malloc(sizeof(char) * fileSize + 1);
    int bytesRead = read(fd, buffer, fileSize);
    if (bytesRead == -1)
    {
      dprintf(STDERR_FILENO, "[!] Cannot read file.\n");
      return -1;
    }
    buffer[bytesRead] = '\0';
    close(fd);

    char *line = strtok(buffer, "\n");
    List *lines = initializeList();
    while (line != NULL)
    {
      char *tempStr = malloc(sizeof(char) * (strlen(line) + 1));
      strcpy(tempStr, line);
      addNode(lines, tempStr);
      line = strtok(NULL, "\n");
    }

    Node *tempLineNode = lines->head;

    while (tempLineNode != NULL)
    {
      char *line = tempLineNode->data;
      Record *record = createRecord(line);
      if (record == NULL)
      {
        dprintf(STDERR_FILENO, "[!] Cannot create record.\n");
        return -1;
      }
      record->day = day;
      record->month = month;
      record->year = year;
      addNode(city->records, record);
      tempLineNode = tempLineNode->next;
    }

    free(buffer);
    freeList(lines, freeString);
  }

  closedir(dir);

  return 0;
}

void freeRecord(void *record)
{
  Record *recordPtr = (Record *)record;
  free(recordPtr->realEstateType);
  free(recordPtr->streetName);
  free(recordPtr);
}

void freeCity(void *city)
{
  City *cityPtr = (City *)city;
  free(cityPtr->name);
  freeList(cityPtr->records, freeRecord);
  free(cityPtr);
}

void printCity(void *city)
{
  City *cityPtr = (City *)city;
  dprintf(STDOUT_FILENO, "-----------------------------------------------\n");
  dprintf(STDOUT_FILENO, "%s: Servant %d: city %s\n", getTime(), servantVariables.pid, cityPtr->name);
  printList(cityPtr->records, printRecord);
  dprintf(STDOUT_FILENO, "-----------------------------------------------\n");
}

void printRecord(void *record)
{
  Record *recordPtr = (Record *)record;
  dprintf(STDOUT_FILENO, "%s: Servant %d: Record id:%d\n", getTime(), servantVariables.pid, recordPtr->id);
  dprintf(STDOUT_FILENO, "%s: Servant %d: Record type:%s\n", getTime(), servantVariables.pid, recordPtr->realEstateType);
  dprintf(STDOUT_FILENO, "%s: Servant %d: Record street name:%s\n", getTime(), servantVariables.pid, recordPtr->streetName);
  dprintf(STDOUT_FILENO, "%s: Servant %d: Record surface area:%d\n", getTime(), servantVariables.pid, recordPtr->surfaceArea);
  dprintf(STDOUT_FILENO, "%s: Servant %d: Record price:%d\n", getTime(), servantVariables.pid, recordPtr->price);
  dprintf(STDOUT_FILENO, "%s: Servant %d: Record date: %d-%d-%d\n", getTime(), servantVariables.pid, recordPtr->day, recordPtr->month, recordPtr->year);
}